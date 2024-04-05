/*
 * usb_hid_def.h
 *
 * Copyright (c) 2023 Jan Rusnak <jan@rusnak.sk>
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

#ifndef USB_HID_DEF_H
#define USB_HID_DEF_H

#define USB_HID_REL_1_11_VER_BCD 0x0111

#define USB_HID_CLASS 0x03
#define USB_HID_SUBCLASS_NO_BOOT 0x00
#define USB_HID_SUBCLASS_BOOT 0x01

#define USB_HID_DESC 0x21
#define USB_HID_REPORT_DESC 0x22
#define USB_HID_PHYSICAL_DESC 0x23

enum usb_hid_report_type {
	USB_HID_REPORT_IN = 1,
	USB_HID_REPORT_OUT,
	USB_HID_REPORT_FEAT
};

enum usb_hid_ctl_req_code {
	USB_HID_GET_REPORT = 0x01,
        USB_HID_GET_IDLE,
        USB_HID_GET_PROTOCOL,
        USB_HID_SET_REPORT = 0x09,
        USB_HID_SET_IDLE,
        USB_HID_SET_PROTOCOL
};

// HID descriptor.
struct usb_hid_desc {
	uint8_t size;
	uint8_t type;
	uint16_t bcd_hid;
	uint8_t country_code;
	uint8_t num_descs;
	uint8_t rep_desc_type;
	uint16_t rep_desc_size;
} __attribute__ ((packed));

#endif
