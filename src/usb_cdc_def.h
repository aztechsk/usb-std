/*
 * usb_cdc_def.h
 *
 * Copyright (c) 2018 Jan Rusnak <jan@rusnak.sk>
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

#ifndef USB_CDC_DEF_H
#define USB_CDC_DEF_H

#define USB_CDC_CDC1_10_VER_BCD 0x0110
#define USB_CDC_CDC1_20_VER_BCD 0x0120

#define USB_CDC_DEV_CLASS    0x02
#define USB_CDC_DEV_SUBCLASS 0x00
#define USB_CDC_DEV_PROT     0x00

#define USB_CDC_COMM_IFACE_CLASS 0x02
#define USB_CDC_COMM_IFACE_ABST_CTL_MODEL 0x02
#define USB_CDC_COMM_IFACE_NO_PROTOCOL 0x00
#define USB_CDC_COMM_IFACE_ITU_T_V_250 0x01

#define USB_CDC_DATA_IFACE_CLASS 0x0A
#define USB_CDC_DATA_IFACE_SUBCLASS 0x00
#define USB_CDC_DATA_IFACE_NO_PROTOCOL 0x00

#define USB_CDC_CS_IFACE 0x24
#define USB_CDC_CS_ENDP 0x25

#define USB_CDC_CALL_MNG_NO (0 << 0)
#define USB_CDC_CALL_MNG_SELF (1 << 0)
#define USB_CDC_CALL_MNG_COMM_IFACE (0 << 1)
#define USB_CDC_CALL_MNG_DATA_IFACE (1 << 1)

#define USB_CDC_ABST_CTL_MNG_NO_CAP 0
#define USB_CDC_ABST_CTL_MNG_COMM_FEAT (1 << 0)
#define USB_CDC_ABST_CTL_MNG_LINE (1 << 1)
#define USB_CDC_ABST_CTL_MNG_SENDBREAK (1 << 2)
#define USB_CDC_ABST_CTL_MNG_NETCONN (1 << 3)

enum usb_cdc_desc_subtype {
	USB_CDC_HEAD_DESC = 0x00,
	USB_CDC_CALL_MNG_DESC = 0x01,
	USB_CDC_ABST_CTL_MNG_DESC = 0x02,
	USB_CDC_UNION_DESC = 0x06
};

// CDC header descriptor.
struct usb_cdc_head_desc {
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint16_t bcd_cdc;
} __attribute__ ((packed));

// CDC call management descriptor.
struct usb_cdc_call_mng_desc {
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint8_t bm_capabilities;
    uint8_t b_data_interface;
} __attribute__ ((packed));

// CDC abstract control management descriptor.
struct usb_cdc_abst_ctl_mng_desc {
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint8_t bm_capabilities;
} __attribute__ ((packed));

// CDC union descriptor.
struct usb_cdc_union_desc {
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint8_t b_master_interface;
    uint8_t b_slave_interface0;
} __attribute__ ((packed));

enum usb_cdc_mngm_req_code {
	USB_CDC_MNGM_SEND_ENCAPSULATED_COMMAND = 0x00,
	USB_CDC_MNGM_GET_ENCAPSULATED_RESPONSE = 0x01,
	USB_CDC_MNGM_SET_COMM_FEATURE = 0x02,
	USB_CDC_MNGM_GET_COMM_FEATURE = 0x03,
	USB_CDC_MNGM_CLEAR_COMM_FEATURE = 0x04,
	USB_CDC_MNGM_SET_AUX_LINE_STATE = 0x10,
	USB_CDC_MNGM_SET_HOOK_STATE = 0x11,
	USB_CDC_MNGM_PULSE_SETUP = 0x12,
	USB_CDC_MNGM_SEND_PULSE = 0x13,
	USB_CDC_MNGM_SET_PULSE_TIME = 0x14,
	USB_CDC_MNGM_RING_AUX_JACK = 0x15,
	USB_CDC_MNGM_SET_LINE_CODING = 0x20,
	USB_CDC_MNGM_GET_LINE_CODING = 0x21,
	USB_CDC_MNGM_SET_CONTROL_LINE_STATE = 0x22,
	USB_CDC_MNGM_SEND_BREAK = 0x23,
	USB_CDC_MNGM_SET_RINGER_PARMS = 0x30,
	USB_CDC_MNGM_GET_RINGER_PARMS = 0x31,
	USB_CDC_MNGM_SET_OPERATION_PARMS = 0x32,
	USB_CDC_MNGM_GET_OPERATION_PARMS = 0x33,
	USB_CDC_MNGM_SET_LINE_PARMS = 0x34,
	USB_CDC_MNGM_GET_LINE_PARMS = 0x35,
	USB_CDC_MNGM_DIAL_DIGITS = 0x36,
	USB_CDC_MNGM_SET_UNIT_PARAMETER = 0x37,
	USB_CDC_MNGM_GET_UNIT_PARAMETER = 0x38,
	USB_CDC_MNGM_CLEAR_UNIT_PARAMETER = 0x39,
	USB_CDC_MNGM_GET_PROFILE = 0x3A,
	USB_CDC_MNGM_SET_ETH_MULTICAST_FILTERS = 0x40,
	USB_CDC_MNGM_SET_ETH_PWR_MNGM_PAT_FILTER = 0x41,
	USB_CDC_MNGM_GET_ETH_PWR_MNGM_PAT_FILTER = 0x42,
	USB_CDC_MNGM_SET_ETH_PACKET_FILTER = 0x43,
	USB_CDC_MNGM_GET_ETH_STATISTIC = 0x44,
	USB_CDC_MNGM_SET_ATM_DATA_FORMAT = 0x50,
	USB_CDC_MNGM_GET_ATM_DEVICE_STATISTICS = 0x51,
	USB_CDC_MNGM_SET_ATM_DEFAULT_VC = 0x52,
	USB_CDC_MNGM_GET_ATM_VC_STATISTICS = 0x53,
	USB_CDC_MNGM_GET_NTB_PARAMETERS = 0x80,
	USB_CDC_MNGM_GET_NET_ADDRESS = 0x81,
	USB_CDC_MNGM_SET_NET_ADDRESS = 0x82,
	USB_CDC_MNGM_GET_NTB_FORMAT = 0x83,
	USB_CDC_MNGM_SET_NTB_FORMAT = 0x84,
	USB_CDC_MNGM_GET_NTB_INPUT_SIZE = 0x85,
	USB_CDC_MNGM_SET_NTB_INPUT_SIZE = 0x86,
	USB_CDC_MNGM_GET_MAX_DATAGRAM_SIZE = 0x87,
	USB_CDC_MNGM_SET_MAX_DATAGRAM_SIZE = 0x88,
	USB_CDC_MNGM_GET_CRC_MODE = 0x89,
	USB_CDC_MNGM_SET_CRC_MODE = 0x8A
};

// Line Coding.
enum usb_cdc_ln_char_fmt {
	USB_CDC_LN_CHAR_FMT_1_STOP_BIT,
	USB_CDC_LN_CHAR_FMT_1_5_STOP_BIT,
        USB_CDC_LN_CHAR_FMT_2_STOP_BIT
};

enum usb_cdc_ln_par_type {
	USB_CDC_LN_PAR_TYPE_NONE,
	USB_CDC_LN_PAR_TYPE_ODD,
        USB_CDC_LN_PAR_TYPE_EVEN,
        USB_CDC_LN_PAR_TYPE_MARK,
        USB_CDC_LN_PAR_TYPE_SPACE
};

enum usb_cdc_ln_Data_bits {
	USB_CDC_LN_DATA_BITS_5 = 5,
        USB_CDC_LN_DATA_BITS_6,
        USB_CDC_LN_DATA_BITS_7,
        USB_CDC_LN_DATA_BITS_8,
        USB_CDC_LN_DATA_BITS_16 = 16
};

struct usb_cdc_line_coding {
	uint32_t dw_dte_rate;
        uint8_t b_char_format;
        uint8_t b_parity_type;
        uint8_t b_data_bits;
} __attribute__ ((packed));

#endif
