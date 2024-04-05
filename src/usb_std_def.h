/*
 * usb_std_def.h
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

#ifndef USB_STD_DEF_H
#define USB_STD_DEF_H

#define USB_STD_USB2_00_VER_BCD 0x0200

#define USB_STD_BUS_POWER_NO_RWAKE  0x80
#define USB_STD_SELF_POWER_NO_RWAKE 0xC0
#define USB_STD_BUS_POWER_RWAKE     0xA0
#define USB_STD_SELF_POWER_RWAKE    0xE0
#define usb_std_max_power_mamp(ma) ((ma) / 2)

#define USB_STD_IN_ENDP 1
#define USB_STD_OUT_ENDP 0
#define usb_std_endp_addr(num, dir) (((dir & 0x01) << 7) | ((num) & 0x0F))

#define USB_STD_TRANS_CONTROL  0
#define USB_STD_TRANS_ISOCHRONOUS 1
#define USB_STD_TRANS_BULK  2
#define USB_STD_TRANS_INTERRUPT  3
#define USB_STD_ISOCH_NO_SYNC (0 << 2)
#define USB_STD_ISOCH_ASYNCHRO (1 << 2)
#define USB_STD_ISOCH_ADAPTIVE (2 << 2)
#define USB_STD_ISOCH_SYNCHRO (3 << 2)
#define USB_STD_ISOCH_DATA_ENDP (0 << 4)
#define USB_STD_ISOCH_FEEDBK_ENDP (1 << 4)
#define USB_STD_ISOCH_IMPL_FEEDBK_DATA_ENDP (2 << 4)

#define usb_std_unicode(c) (c), 0
#define USB_STD_EN_US_CODE 0x09, 0x04
#define usb_std_str_desc_size(c_num) ((c_num) * 2 + 2)

enum usb_desc_type {
	USB_DEV_DESC = 0x01,
	USB_CONF_DESC = 0x02,
        USB_STR_DESC = 0x03,
        USB_IFACE_DESC = 0x04,
	USB_ENDP_DESC = 0x05,
        USB_DEV_QUAL_DESC = 0x06,
        USB_ALT_SPEED_CONF_DESC = 0x07,
        USB_IFACE_POWER_DESC = 0x08,
        USB_OTG_DESC = 0x09,
        USB_DBG_DESC = 0x0A,
        USB_IFACE_ASSOC_DESC = 0x0B
};

// Device descriptor.
struct usb_dev_desc {
	uint8_t size;
        uint8_t type;
        uint16_t bcd_usb;
        uint8_t b_device_class;
        uint8_t b_device_subclass;
        uint8_t b_device_protocol;
        uint8_t b_max_packet_size0;
        uint16_t id_vendor;
        uint16_t id_product;
        uint16_t bcd_device;
        uint8_t i_manufacturer;
        uint8_t i_product;
        uint8_t i_serial_number;
        uint8_t b_num_configurations;
} __attribute__ ((__packed__));

// Device qualifier descriptor.
struct usb_dev_qual_desc {
	uint8_t size;
        uint8_t type;
        uint16_t bcd_usb;
        uint8_t b_device_class;
	uint8_t b_device_subclass;
	uint8_t b_device_protocol;
	uint8_t b_max_packet_size0;
	uint8_t b_num_configurations;
        uint8_t b_reserved;
} __attribute__ ((__packed__));

// Configuration descriptor.
struct usb_conf_desc {
	uint8_t size;
	uint8_t type;
	uint16_t w_total_size;
        uint8_t b_num_interfaces;
        uint8_t b_configuration_value;
        uint8_t i_configuration;
        uint8_t bm_attributes;
        uint8_t b_max_power;
} __attribute__ ((__packed__));

// Interface descriptor.
struct usb_iface_desc {
	uint8_t size;
	uint8_t type;
        uint8_t b_interface_number;
        uint8_t b_alternate_setting;
        uint8_t b_num_endpoints;
        uint8_t b_interface_class;
        uint8_t b_interface_subclass;
        uint8_t b_interface_protocol;
        uint8_t i_interface;
} __attribute__ ((__packed__));

// Interface association descriptor.
struct usb_iface_assoc_desc {
	uint8_t size;
	uint8_t type;
        uint8_t b_first_interface;
        uint8_t b_interface_count;
        uint8_t b_function_class;
        uint8_t b_function_subclass;
        uint8_t b_function_protocol;
        uint8_t i_function;
} __attribute__ ((__packed__));

// Endpoint descriptor.
struct usb_endp_desc {
	uint8_t size;
	uint8_t type;
        uint8_t b_endpoint_address;
        uint8_t bm_attributes;
        uint16_t w_max_packet_size;
        uint8_t b_interval;
} __attribute__ ((__packed__));

// Generic descriptor.
struct usb_gen_desc {
	uint8_t size;
	uint8_t type;
} __attribute__ ((__packed__));

/**
 * find_usb_endp_desc
 */
const struct usb_endp_desc *find_usb_endp_desc(const void *conf_descs, int conf_descs_sz);

/**
 * usb_endp_desc_get_ep_type
 */
enum udp_endp_type usb_endp_desc_get_ep_type(const struct usb_endp_desc *desc);

#endif
