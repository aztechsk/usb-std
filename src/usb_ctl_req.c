/*
 * usb_ctl_req.c
 *
 * Copyright (c) 2024 Jan Rusnak <jan@rusnak.sk>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <gentyp.h>
#include "sysconf.h"
#include "msgconf.h"
#include "criterr.h"
#include "udp.h"
#include "usb_ctl_req.h"

enum trans_state {
	STP_TRANS_IDLE,
	STP_TRANS_DATA_IN,
        STP_TRANS_DATA_IN_STATUS,
	STP_TRANS_DATA_OUT,
	STP_TRANS_DATA_OUT_STATUS,
        STP_TRANS_NO_DATA,
        STP_TRANS_NO_DATA_STATUS,
        STP_TRANS_STALL
};

static struct usb_ctl_req_stats stats;
static struct usb_stp_pkt stp_pkt;
static enum trans_state state;
static int16_t pkt_sz;
static boolean_t sent_zero_pkt;
static struct usb_ctl_req_clbks *p_std_clbks;
static struct usb_ctl_req_clbks *p_cls_clbks;
static struct usb_ctl_req_clbks *p_vnd_clbks;
static struct usb_ctl_req_clbks *p_clbks;
static struct usb_ctl_req ctl_req;

#if USB_LOG_CTL_REQ_EVENTS == 1
static logger_t usb_logger;
static BaseType_t dmy;
#endif

static void check_clbks(struct usb_ctl_req_clbks *clbks);
static void rxstp(void);
static void txcomp(void);
static void rxdata(int nmb);
static void stlsnt(void);
#if USB_LOG_CTL_REQ_EVENTS == 1
static void log_usb_ctl_req_event(const char *txt);
#endif

/**
 * init_usb_ctl_req
 */
void init_usb_ctl_req(logger_t *logger)
{
#if USB_LOG_CTL_REQ_EVENTS == 1
	usb_logger = *logger;
#endif
        pkt_sz = udp_endp0_pkt_sz();
        add_udp_endp0_rxstp_clbk(rxstp);
        add_udp_endp0_txcomp_clbk(txcomp);
        add_udp_endp0_rxdata_clbk(rxdata);
        add_udp_endp0_stlsnt_clbk(stlsnt);
}

/**
 * add_usb_ctl_req_std_clbks
 */
void add_usb_ctl_req_std_clbks(struct usb_ctl_req_clbks *clbks)
{
	check_clbks(clbks);
	p_std_clbks = clbks;
}

/**
 * add_usb_ctl_req_cls_clbks
 */
void add_usb_ctl_req_cls_clbks(struct usb_ctl_req_clbks *clbks)
{
	check_clbks(clbks);
	p_cls_clbks = clbks;
}

/**
 * add_usb_ctl_req_vnd_clbks
 */
void add_usb_ctl_req_vnd_clbks(struct usb_ctl_req_clbks *clbks)
{
	check_clbks(clbks);
	p_vnd_clbks = clbks;
}

/**
 * check_clbks
 */
static void check_clbks(struct usb_ctl_req_clbks *clbks)
{
	if (!(clbks->stp_clbk && clbks->in_req_ack_clbk &&
	      clbks->out_req_rec_clbk && clbks->out_req_ack_clbk)) {
		crit_err_exit(BAD_PARAMETER);
	}
}

/**
 * rxstp
 */
static void rxstp(void)
{
	udp_endp0_disable_stl();
	ctl_req.valid = FALSE;
        p_clbks = NULL;
	read_udp_endp0_fifo(&stp_pkt, sizeof(stp_pkt));
	switch ((stp_pkt.bm_request_type >> 5) & 3) {
	case USB_STANDARD_REQUEST :
		if (p_std_clbks) {
			p_clbks = p_std_clbks;
		}
		break;
	case USB_CLASS_REQUEST :
		if (p_cls_clbks) {
			p_clbks = p_cls_clbks;
		}
		break;
	case USB_VENDOR_REQUEST :
		if (p_vnd_clbks) {
			p_clbks = p_vnd_clbks;
		}
		break;
	default :
#if USB_LOG_CTL_REQ_EVENTS == 1
		log_usb_ctl_req_event("stp !bad request type!");
#endif
		stats.bad_stp_req_cnt++;
		break;
	}
        if (p_clbks) {
		ctl_req = p_clbks->stp_clbk(&stp_pkt);
	}
	if (ctl_req.valid) {
		if (ctl_req.trans_dir == UDP_CTL_TRANS_IN) {
			udp_endp0_rxstp_done(UDP_CTL_TRANS_IN);
			state = STP_TRANS_DATA_IN;
		} else {
			udp_endp0_rxstp_done(UDP_CTL_TRANS_OUT);
			if (ctl_req.nmb == 0) {
				state = STP_TRANS_NO_DATA;
			} else {
				state = STP_TRANS_DATA_OUT;
			}
		}
	} else {
		udp_endp0_rxstp_done(UDP_CTL_TRANS_OUT);
                state = STP_TRANS_STALL;
	}
	if (state == STP_TRANS_DATA_IN) {
		if (ctl_req.trans_nmb > ctl_req.nmb) {
			sent_zero_pkt = (ctl_req.nmb % pkt_sz) ? FALSE : TRUE;
		} else {
			sent_zero_pkt = FALSE;
		}
		if (ctl_req.nmb >= pkt_sz) {
			write_udp_endp0_fifo(ctl_req.buf, pkt_sz);
			ctl_req.nmb -= pkt_sz;
			ctl_req.buf += pkt_sz;
		} else {
			write_udp_endp0_fifo(ctl_req.buf, ctl_req.nmb);
			ctl_req.nmb = 0;
		}
		udp_endp0_tx_pkt_rdy();
	} else if (state == STP_TRANS_NO_DATA) {
		udp_endp0_tx_pkt_rdy();
                state = STP_TRANS_NO_DATA_STATUS;
	} else if (state == STP_TRANS_STALL) {
		udp_endp0_req_stl();
	}
}

/**
 * txcomp
 */
static void txcomp(void)
{
	switch (state) {
	case STP_TRANS_DATA_IN :
		if (ctl_req.nmb == 0) {
			if (sent_zero_pkt) {
				udp_endp0_tx_pkt_rdy();
				sent_zero_pkt = FALSE;
			} else {
				udp_endp0_txcomp_accept();
                                state = STP_TRANS_DATA_IN_STATUS;
			}
		} else {
			if (ctl_req.nmb >= pkt_sz) {
				write_udp_endp0_fifo(ctl_req.buf, pkt_sz);
				ctl_req.nmb -= pkt_sz;
				ctl_req.buf += pkt_sz;
			} else {
				write_udp_endp0_fifo(ctl_req.buf, ctl_req.nmb);
				ctl_req.nmb = 0;
			}
			udp_endp0_tx_pkt_rdy();
		}
		break;
	case STP_TRANS_NO_DATA_STATUS :
		/* FALLTHRU */
	case STP_TRANS_DATA_OUT_STATUS :
		udp_endp0_txcomp_accept();
                p_clbks->out_req_ack_clbk();
                state = STP_TRANS_IDLE;
		break;
	default :
#if USB_LOG_CTL_REQ_EVENTS == 1
		log_usb_ctl_req_event("unexp txcomp");
#endif
		stats.unexp_udp_evnt_cnt++;
		udp_endp0_txcomp_accept();
		break;
	}
}

/**
 * rxdata
 */
static void rxdata(int nmb)
{
	switch (state) {
	case STP_TRANS_DATA_IN_STATUS :
		if (nmb != 0) {
#if USB_LOG_CTL_REQ_EVENTS == 1
			log_usb_ctl_req_event("data_in !not zero handshake pkt!");
#endif
                        stats.nzr_hs_pkt_cnt++;
		}
                p_clbks->in_req_ack_clbk();
                udp_endp0_rxdata_done();
                state = STP_TRANS_IDLE;
		break;
	case STP_TRANS_DATA_OUT :
		if (nmb == pkt_sz) {
			if (nmb < ctl_req.nmb) {
				read_udp_endp0_fifo(ctl_req.buf, pkt_sz);
				ctl_req.nmb -= pkt_sz;
				ctl_req.buf += pkt_sz;
				udp_endp0_rxdata_done();
				return;
			} else if (nmb == ctl_req.nmb) {
				read_udp_endp0_fifo(ctl_req.buf, pkt_sz);
				udp_endp0_rxdata_done();
				if (p_clbks->out_req_rec_clbk()) {
					udp_endp0_tx_pkt_rdy();
					state = STP_TRANS_DATA_OUT_STATUS;
				} else {
					udp_endp0_req_stl();
                                        state = STP_TRANS_STALL;
				}
				return;
			} else {
#if USB_LOG_CTL_REQ_EVENTS == 1
				log_usb_ctl_req_event("data_out !more bytes received!");
#endif
				stats.unexp_data_sz_cnt++;
			}
		} else if (nmb < pkt_sz) {
			if (nmb == ctl_req.nmb) {
				read_udp_endp0_fifo(ctl_req.buf, nmb);
                                udp_endp0_rxdata_done();
				if (p_clbks->out_req_rec_clbk()) {
					udp_endp0_tx_pkt_rdy();
					state = STP_TRANS_DATA_OUT_STATUS;
				} else {
					udp_endp0_req_stl();
                                        state = STP_TRANS_STALL;
				}
				return;
#if USB_LOG_CTL_REQ_EVENTS == 1
			} else if (nmb > ctl_req.nmb) {
				log_usb_ctl_req_event("data_out !more bytes received!");
			} else {
				log_usb_ctl_req_event("data_out !fewer bytes received!");
			}
#else
			}
#endif
			stats.unexp_data_sz_cnt++;
		} else {
#if USB_LOG_CTL_REQ_EVENTS == 1
			log_usb_ctl_req_event("data_out !udp pkt_sz error!");
#endif
			stats.udp_pkt_sz_err_cnt++;
		}
		udp_endp0_rxdata_done();
		udp_endp0_req_stl();
		state = STP_TRANS_STALL;
		break;
	default :
#if USB_LOG_CTL_REQ_EVENTS == 1
		log_usb_ctl_req_event("unexp rxdata");
#endif
		stats.unexp_udp_evnt_cnt++;
		udp_endp0_rxdata_done();
		break;
	}
}

/**
 * stlsnt
 */
static void stlsnt(void)
{
	switch (state) {
	case STP_TRANS_STALL :
		udp_endp0_stlsnt_accept();
		break;
	default :
#if USB_LOG_CTL_REQ_EVENTS == 1
		log_usb_ctl_req_event("unexp stlsnt");
#endif
		stats.unexp_udp_evnt_cnt++;
		udp_endp0_stlsnt_accept();
		break;
	}
}

/**
 * get_usb_ctl_req_stats
 */
struct usb_ctl_req_stats *get_usb_ctl_req_stats(void)
{
	return (&stats);
}

#if TERMOUT == 1
/**
 * log_usb_ctl_req_stats
 */
void log_usb_ctl_req_stats(void)
{
	if (stats.unexp_data_sz_cnt) {
        	msg(INF, "usb_ctl_req.c: unexp_data_sz=%hu\n", stats.unexp_data_sz_cnt);
	}
	if (stats.nzr_hs_pkt_cnt) {
		msg(INF, "usb_ctl_req.c: nzr_hs_pkt=%hu\n", stats.nzr_hs_pkt_cnt);
	}
	if (stats.bad_stp_req_cnt) {
		msg(INF, "usb_ctl_req.c: bad_stp_req=%hu\n", stats.bad_stp_req_cnt);
	}
        if (stats.unexp_udp_evnt_cnt) {
		msg(INF, "usb_ctl_req.c: unexp_udp_evnt=%hu\n", stats.unexp_udp_evnt_cnt);
	}
	if (stats.udp_pkt_sz_err_cnt) {
		msg(INF, "usb_ctl_req.c: udp_pkt_sz_err=%hu\n", stats.udp_pkt_sz_err_cnt);
	}
}
#endif

#if USB_LOG_CTL_REQ_EVENTS == 1
/**
 * fmt_usb_ctl_req_event
 */
static void fmt_usb_ctl_req_event(struct usb_ctl_req_event *p);
static void fmt_usb_ctl_req_event(struct usb_ctl_req_event *p)
{
	msg(INF, "usb_ctl_req.c: %s\n", p->txt);
}

static struct usb_ctl_req_event ucre = {
	.type = USB_CTL_REQ_EVENT_TYPE,
	.fmt = fmt_usb_ctl_req_event
};

/**
 * log_usb_ctl_req_event
 */
static void log_usb_ctl_req_event(const char *txt)
{
	ucre.txt = txt;
	if (pdTRUE != xQueueSendFromISR(usb_logger.que, &ucre, &dmy)) {
		usb_logger.que_err();
	}
}
#endif
