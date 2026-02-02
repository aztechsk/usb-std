/*
 * usb_std_def.c
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

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <gentyp.h>
#include "sysconf.h"
#include "udp.h"
#include "usb_std_def.h"

/**
 * find_usb_endp_desc
 */
const struct usb_endp_desc *find_usb_endp_desc(const void *conf_descs, int conf_descs_sz)
{
	static const uint8_t *dsc;
        static boolean_t last;
	const uint8_t *end, *ret;

	end = (const uint8_t *) conf_descs + conf_descs_sz - 1;
	if (dsc == NULL) {
		dsc = conf_descs;
		last = FALSE;
	} else {
		if (last) {
			dsc = NULL;
			return (NULL);
		}
	}
	while (TRUE) {
		ret = dsc;
		if (dsc + *dsc - 1 < end) {
			dsc += *dsc;
			if (*(ret + 1) == USB_ENDP_DESC) {
				return ((const struct usb_endp_desc *) ret);
			}
		} else {
			if (*(ret + 1) == USB_ENDP_DESC) {
				last = TRUE;
				return ((const struct usb_endp_desc *) ret);
			} else {
				dsc = NULL;
				return (NULL);
			}
		}
	}
}

/**
 * usb_endp_desc_get_ep_type
 */
enum udp_endp_type usb_endp_desc_get_ep_type(const struct usb_endp_desc *desc)
{
	if (desc->b_endpoint_address & 0x80) {
		switch (desc->bm_attributes & 3) {
		case 1 :
			return (UDP_ISO_IN_ENDP);
		case 2 :
			return (UDP_BULK_IN_ENDP);
		case 3 :
			return (UDP_INT_IN_ENDP);
		}
	} else {
		switch (desc->bm_attributes & 3) {
		case 1 :
			return (UDP_ISO_OUT_ENDP);
		case 2 :
			return (UDP_BULK_OUT_ENDP);
		case 3 :
			return (UDP_INT_OUT_ENDP);
		}
	}
        return (UDP_CTRL_ENDP);
}
