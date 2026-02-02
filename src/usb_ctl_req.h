/*
 * usb_ctl_req.h
 *
 * Copyright (c) 2020 Jan Rusnak <jan@rusnak.sk>
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

#ifndef USB_CTL_REQ_H
#define USB_CTL_REQ_H

struct usb_stp_pkt {
	uint8_t bm_request_type;
        uint8_t b_request;
	uint16_t w_value;
        uint16_t w_index;
        uint16_t w_length;
} __attribute__ ((__packed__));

enum usb_ctl_req_code {
	USB_GET_STATUS,
        USB_CLEAR_FEATURE,
        USB_SET_FEATURE = 3,
        USB_SET_ADDRESS = 5,
        USB_GET_DESCRIPTOR,
        USB_SET_DESCRIPTOR,
        USB_GET_CONFIGURATION,
        USB_SET_CONFIGURATION,
        USB_GET_INTERFACE,
        USB_SET_INTERFACE,
        USB_SYNCH_FRAME
};

enum usb_ctl_req_type {
	USB_STANDARD_REQUEST,
	USB_CLASS_REQUEST,
	USB_VENDOR_REQUEST
};

enum usb_ctl_req_recp {
	USB_DEVICE_RECIPIENT,
        USB_IFACE_RECIPIENT,
        USB_ENDP_RECIPIENT,
        USB_OTHER_RECIPIENT
};

enum usb_ctl_std_feat {
	USB_ENDP_HALT_FEAT,
	USB_DEV_REM_WKUP_FEAT,
        USB_TEST_MODE_FEAT
};

struct usb_ctl_req {
	boolean_t valid;
	uint8_t *buf;
	short nmb;
	short trans_nmb;
        int8_t trans_dir;
};

struct usb_ctl_req_clbks {
	struct usb_ctl_req (*stp_clbk)(struct usb_stp_pkt *stp_pkt);
	void (*in_req_ack_clbk)(void);
	boolean_t (*out_req_rec_clbk)(void);
	void (*out_req_ack_clbk)(void);
};

#define USB_CTL_REQ_EVENT_TYPE 8

struct usb_ctl_req_event {
	int8_t type;
        const char *txt;
        void (*fmt)(struct usb_ctl_req_event *);
};

struct usb_ctl_req_stats {
	unsigned short unexp_data_sz_cnt;
        unsigned short nzr_hs_pkt_cnt;
        unsigned short bad_stp_req_cnt;
        unsigned short unexp_udp_evnt_cnt;
        unsigned short udp_pkt_sz_err_cnt;
};

/**
 * init_usb_ctl_req
 */
void init_usb_ctl_req(logger_t *logger);

/**
 * add_usb_ctl_req_std_clbks
 */
void add_usb_ctl_req_std_clbks(struct usb_ctl_req_clbks *clbks);

/**
 * add_usb_ctl_req_cls_clbks
 */
void add_usb_ctl_req_cls_clbks(struct usb_ctl_req_clbks *clbks);

/**
 * add_usb_ctl_req_vnd_clbks
 */
void add_usb_ctl_req_vnd_clbks(struct usb_ctl_req_clbks *clbks);

/**
 * get_usb_ctl_req_stats
 */
struct usb_ctl_req_stats *get_usb_ctl_req_stats(void);

#if TERMOUT == 1
/**
 * log_usb_ctl_req_stats
 */
void log_usb_ctl_req_stats(void);
#endif

#endif
