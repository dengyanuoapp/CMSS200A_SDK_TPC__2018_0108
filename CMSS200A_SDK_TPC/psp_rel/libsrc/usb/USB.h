/*
 *********************************************************************************************************
 *                                       ACTOS GL5101
 *
 *
 *                            (c) Copyright 2002-2007, Actions Co,Ld.
 *                                    All Right Reserved
 *
 *  File   : USB.h
 *  By     : zhouxl
 *  Version: v1.00     first version     2007-4-20 13:35
 *********************************************************************************************************
 */

#ifndef _USB_H
#define _USB_H

#ifdef __C251__
/*
 *********************************************************************************************************
 *  This file holds USB constants and structures that are needed for USB
 *  device APIs.  These are used by the USB device model, which is defined
 *  in chapter 9 of the USB 2.0 specification.
 *  There's also "Wireless USB", using low power short range radios for
 *  peripheral interconnection but otherwise building on the USB framework.
 *********************************************************************************************************
 */

/* CONTROL REQUEST SUPPORT */

/*
 *********************************************************************************************************
 *  USB directions
 *  This bit flag is used in endpoint descriptors' bEndpointAddress field.
 *  It's also one of three fields in control requests bRequestType.
 *********************************************************************************************************
 */
#define USB_DIR_OUT                     0               /* to device */
#define USB_DIR_IN                      0x80            /* to host */

/*
 *********************************************************************************************************
 *  USB types, the second of three bRequestType fields
 *********************************************************************************************************
 */
#define USB_TYPE_MASK                   (0x03 << 5)
#define USB_TYPE_STANDARD               (0x00 << 5)
#define USB_TYPE_CLASS                  (0x01 << 5)
#define USB_TYPE_VENDOR                 (0x02 << 5)
#define USB_TYPE_RESERVED               (0x03 << 5)

/*
 *********************************************************************************************************
 *  USB recipients, the third of three bRequestType fields
 *********************************************************************************************************
 */
#define USB_RECIP_MASK                  0x1f
#define USB_RECIP_DEVICE                0x00
#define USB_RECIP_INTERFACE             0x01
#define USB_RECIP_ENDPOINT              0x02
#define USB_RECIP_OTHER                 0x03

/*
 *********************************************************************************************************
 *  Standard requests, for the bRequest field of a SETUP packet.
 *
 *  These are qualified by the bRequestType field, so that for example
 *  TYPE_CLASS or TYPE_VENDOR specific feature flags could be retrieved
 *  by a GET_STATUS request.
 *********************************************************************************************************
 */
#define USB_REQ_GET_STATUS              0x00
#define USB_REQ_CLEAR_FEATURE           0x01
#define USB_REQ_SET_FEATURE             0x03
#define USB_REQ_SET_ADDRESS             0x05
#define USB_REQ_GET_DESCRIPTOR          0x06
#define USB_REQ_SET_DESCRIPTOR          0x07
#define USB_REQ_GET_CONFIGURATION       0x08
#define USB_REQ_SET_CONFIGURATION       0x09
#define USB_REQ_GET_INTERFACE           0x0A
#define USB_REQ_SET_INTERFACE           0x0B
#define USB_REQ_SYNCH_FRAME             0x0C
#define BULK_ONLY_MASS_STORAGE_RESET    0xFF
#define GET_MAX_LUN					    0xFE

#define USB_REQ_SET_ENCRYPTION          0x0D    /* Wireless USB */
#define USB_REQ_GET_ENCRYPTION          0x0E
#define USB_REQ_SET_HANDSHAKE           0x0F
#define USB_REQ_GET_HANDSHAKE           0x10
#define USB_REQ_SET_CONNECTION          0x11
#define USB_REQ_SET_SECURITY_DATA       0x12
#define USB_REQ_GET_SECURITY_DATA       0x13
#define USB_REQ_SET_WUSB_DATA           0x14
#define USB_REQ_LOOPBACK_DATA_WRITE     0x15
#define USB_REQ_LOOPBACK_DATA_READ      0x16
#define USB_REQ_SET_INTERFACE_DS        0x17

/*
 *********************************************************************************************************
 *  USB feature flags are written using USB_REQ_{CLEAR,SET}_FEATURE, and
 *  are read as a bit array returned by USB_REQ_GET_STATUS.  (So there
 *  are at most sixteen features of each type.)
 *********************************************************************************************************
 */
#define USB_DEVICE_SELF_POWERED         0       /* (read only) */
#define USB_DEVICE_REMOTE_WAKEUP        1       /* dev may initiate wakeup */
#define USB_DEVICE_TEST_MODE            2       /* (wired high speed only) */
#define USB_DEVICE_BATTERY              2       /* (wireless) */
#define USB_DEVICE_B_HNP_ENABLE         3       /* (otg) dev may initiate HNP */
#define USB_DEVICE_WUSB_DEVICE          3       /* (wireless)*/
#define USB_DEVICE_A_HNP_SUPPORT        4       /* (otg) RH port supports HNP */
#define USB_DEVICE_A_ALT_HNP_SUPPORT    5       /* (otg) other RH port does */
#define USB_DEVICE_DEBUG_MODE           6       /* (special devices only) */

#define USB_ENDPOINT_HALT               0       /* IN/OUT will STALL */

/*
 *********************************************************************************************************
 *  SETUP data for a USB device control request
 *
 *  bRequestType : The request command type.
 *                   : bit7  : The direction data transfer: 0: HOST To Device; 1:Device To Host;
 *                   : bit6-5: The type of the command; 00: Standard request; 01: Species request;
 *                                                      10: User-defined; 11:Reserved
 *                   : bit4-0: The type of receiver; 00000: Device; 00001: Interface; 00010:Endpoint;
 *                                                   00011: Other receiver; Other: Reserved
 *  bRequest     : Request command code;
 *                   : 0:GET_STATUS; 1:CLEAR_FEATURE; 3:SET_FEATURE; 5:SET_ADDRESS; 6:GET_DESCRIPTOR; 7:SET_DESCRIPTOR;
 *                   : 8:GET_CONFIGURATION; 9:SET_CONFIGURATION; 10:GET_INTERFACE; 11:SET_INTERFACE; 12:SYNCH_FRAME.
 *  wValue       : Command Info;
 *  wIndex       : The Index info.
 *  wLength      : The length of the data to be transfered.
 *
 *  This structure is used to send control requests to a USB device.  It matches
 *  the different fields of the USB 2.0 Spec section 9.3, table 9-2.  See the
 *  USB spec for a fuller description of the different fields, and what they are used for.
 *
 *  Note that the driver for any interface can issue control requests. For most devices,
 *  interfaces don't coordinate with each other, so such requests may be made at any time.
 *********************************************************************************************************
 */
typedef struct
{
    char bmRequestType;
    char bRequest;
    int wValue;
    int wIndex;
    int wLength;
} usb_ctrlrequest;

/*
 *********************************************************************************************************
 *  STANDARD DESCRIPTORS ... as returned by GET_DESCRIPTOR, or
 *  (rarely) accepted by SET_DESCRIPTOR.
 *
 *  Note that all multi-byte values here are encoded in little endian
 *  byte order "on the wire".  But when exposed through Linux-USB APIs,
 *  they've been converted to cpu byte order.
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *  Descriptor types ... USB 2.0 spec table 9.5
 *********************************************************************************************************
 */
#define USB_DT_DEVICE                   0x01
#define USB_DT_CONFIG                   0x02
#define USB_DT_STRING                   0x03
#define USB_DT_INTERFACE                0x04
#define USB_DT_ENDPOINT                 0x05
#define USB_DT_DEVICE_QUALIFIER         0x06
#define USB_DT_OTHER_SPEED_CONFIG       0x07
#define USB_DT_INTERFACE_POWER          0x08
/*
 *********************************************************************************************************
 *  these are from a minor usb 2.0 revision (ECN)
 *********************************************************************************************************
 */
#define USB_DT_OTG                      0x09
#define USB_DT_DEBUG                    0x0a
#define USB_DT_INTERFACE_ASSOCIATION    0x0b
/*
 *********************************************************************************************************
 *  these are from the Wireless USB spec
 *********************************************************************************************************
 */
#define USB_DT_SECURITY                 0x0c
#define USB_DT_KEY                      0x0d
#define USB_DT_ENCRYPTION_TYPE          0x0e
#define USB_DT_BOS                      0x0f
#define USB_DT_DEVICE_CAPABILITY        0x10
#define USB_DT_WIRELESS_ENDPOINT_COMP   0x11

/*
 *********************************************************************************************************
 *  conventional codes for class-specific descriptors
 *********************************************************************************************************
 */
#define USB_DT_CS_DEVICE                0x21
#define USB_DT_CS_CONFIG                0x22
#define USB_DT_CS_STRING                0x23
#define USB_DT_CS_INTERFACE             0x24
#define USB_DT_CS_ENDPOINT              0x25

/*
 *********************************************************************************************************
 *  All standard descriptors have these 2 fields at the beginning
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;
} usb_descriptor_header;

/*
 *********************************************************************************************************
 *  Device descriptor
 *
 *  bLength      : The request command type.
 *                   : bit7  : The direction data transfer: 0: HOST To Device; 1:Device To Host;
 *                   : bit6-5: The type of the command; 00: Standard request; 01: Species request;
 *                                                      10: User-defined; 11:Reserved
 *                   : bit4-0: The type of receiver; 00000: Device; 00001: Interface;
 *                                                   00010: Endpoint; 00011: Other receiver; Other: Reserved
 *  bRequest     : Request command code;
 *                   : 0:GET_STATUS; 1:CLEAR_FEATURE; 3:SET_FEATURE; 5:SET_ADDRESS; 6:GET_DESCRIPTOR; 7:SET_DESCRIPTOR;
 *                   : 8:GET_CONFIGURATION; 9:SET_CONFIGURATION; 10:GET_INTERFACE; 11:SET_INTERFACE; 12:SYNCH_FRAME.
 *  wValue       : Command Info;
 *  wIndex       : The Index info.
 *  wLength      : The length of the data to be transfered.
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;

    int bcdUSB;
    char bDeviceClass;
    char bDeviceSubClass;
    char bDeviceProtocol;
    char bMaxPacketSize0;
    int idVendor;
    int idProduct;
    int bcdDevice;
    char iManufacturer;
    char iProduct;
    char iSerialNumber;
    char bNumConfigurations;
} usb_device_descriptor;

#define USB_DT_DEVICE_SIZE              18

/*
 *********************************************************************************************************
 *  Device and/or Interface Class codes as found in bDeviceClass or bInterfaceClass
 *  and defined by www.usb.org documents
 *********************************************************************************************************
 */
#define USB_CLASS_PER_INTERFACE         0       /* for DeviceClass */
#define USB_CLASS_AUDIO                 1
#define USB_CLASS_COMM                  2
#define USB_CLASS_HID                   3
#define USB_CLASS_PHYSICAL              5
#define USB_CLASS_STILL_IMAGE           6
#define USB_CLASS_PRINTER               7
#define USB_CLASS_MASS_STORAGE          8
#define USB_CLASS_HUB                   9
#define USB_CLASS_CDC_DATA              0x0a
#define USB_CLASS_CSCID                 0x0b    /* chip+ smart card */
#define USB_CLASS_CONTENT_SEC           0x0d    /* content security */
#define USB_CLASS_VIDEO                 0x0e
#define USB_CLASS_WIRELESS_CONTROLLER   0xe0
#define USB_CLASS_APP_SPEC              0xfe
#define USB_CLASS_VENDOR_SPEC           0xff

/*
 *********************************************************************************************************
 *  USB_DT_CONFIG: Configuration descriptor information.
 *
 *  USB_DT_OTHER_SPEED_CONFIG is the same descriptor, except that the
 *  descriptor type is different.  Highspeed-capable devices can look
 *  different depending on what speed they're currently running.  Only
 *  devices with a USB_DT_DEVICE_QUALIFIER have any OTHER_SPEED_CONFIG
 *  descriptors.
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;

    int wTotalLength;
    char bNumInterfaces;
    char bConfigurationValue;
    char iConfiguration;
    char bmAttributes;
    char bMaxPower;
} usb_config_descriptor;

#define USB_DT_CONFIG_SIZE              9

/*
 *********************************************************************************************************
 *  from config descriptor bmAttributes
 *********************************************************************************************************
 */
#define USB_CONFIG_ATT_ONE              (1 << 7)        /* must be set */
#define USB_CONFIG_ATT_SELFPOWER        (1 << 6)        /* self powered */
#define USB_CONFIG_ATT_WAKEUP           (1 << 5)        /* can wakeup */
#define USB_CONFIG_ATT_BATTERY          (1 << 4)        /* battery powered */

/*
 *********************************************************************************************************
 *  USB_DT_STRING: String descriptor
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;

    int wlanguageID; /* UTF-16LE encoded */
} usb_languageid_descriptor;

typedef struct
{
    char bLength;
    char bDescriptorType;

    char bSring[0x16]; /* UTF-16LE encoded */
} usb_manufacure_descriptor;

typedef struct
{
    char bLength;
    char bDescriptorType;

    char wString[0x28]; /* UTF-16LE encoded */
} usb_product_descriptor;

typedef struct
{
    char bLength;
    char bDescriptorType;
    char wString[0x40]; /* UTF-16LE encoded */
} usb_serialnum_descriptor;

/*
 *********************************************************************************************************
 *  note that "string" zero is special, it holds language codes that
 *  the device supports, not Unicode characters
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *  USB_DT_INTERFACE: Interface descriptor
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;

    char bInterfaceNumber;
    char bAlternateSetting;
    char bNumEndpoints;
    char bInterfaceClass;
    char bInterfaceSubClass;
    char bInterfaceProtocol;
    char iInterface;
} usb_interface_descriptor;

#define USB_DT_INTERFACE_SIZE           9

/*
 *********************************************************************************************************
 *  USB_DT_ENDPOINT: Endpoint descriptor
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;

    char bEndpointAddress;
    char bmAttributes;
    int wMaxPacketSize;
    char bInterval;
} usb_endpoint_descriptor;

typedef struct
{
    char bLength;
    char bDescriptorType;

    char bEndpointAddress;
    char bmAttributes;
    int wMaxPacketSize;
    char bInterval;

    /* NOTE:  these two are _only_ in audio endpoints. */
    /* use USB_DT_ENDPOINT_SIZE in bLength, not sizeof. */
    char bRefresh;
    char bSynchAddress;
} usb_audio_endpoint_descriptor;

#define USB_DT_ENDPOINT_SIZE            7
#define USB_DT_ENDPOINT_AUDIO_SIZE      9       /* Audio extension */

/*
 *********************************************************************************************************
 *  Endpoints
 *********************************************************************************************************
 */

#define USB_ENDPOINT_NUMBER_MASK        0x0f    /* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK           0x80

#define USB_ENDPOINT_XFERTYPE_MASK      0x03    /* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL       0
#define USB_ENDPOINT_XFER_ISOC          1
#define USB_ENDPOINT_XFER_BULK          2
#define USB_ENDPOINT_XFER_INT           3
#define USB_ENDPOINT_MAX_ADJUSTABLE     0x80

/*
 *********************************************************************************************************
 *  USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;

    int bcdUSB;
    char bDeviceClass;
    char bDeviceSubClass;
    char bDeviceProtocol;
    char bMaxPacketSize0;
    char bNumConfigurations;
    char bRESERVED;
} usb_qualifier_descriptor;

/*
 *********************************************************************************************************
 *  USB_DT_OTG (from OTG 1.0a supplement)
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;

    char bmAttributes; /* support for HNP, SRP, etc */
} usb_otg_descriptor;

/*
 *********************************************************************************************************
 *  From usb_otg_descriptor.bmAttribute
 *********************************************************************************************************
 */
#define USB_OTG_SRP             (1 << 0)
#define USB_OTG_HNP             (1 << 1)        /* swap host/device roles */

/*-------------------------------------------------------------------------*/

/*
 *********************************************************************************************************
 *  USB_DT_DEBUG:  for special highspeed devices, replacing serial console
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;

    /* bulk endpoints with 8 byte maxpacket */
    char bDebugInEndpoint;
    char bDebugOutEndpoint;
} usb_debug_descriptor;

/*
 *********************************************************************************************************
 *  USB_DT_INTERFACE_ASSOCIATION: groups interfaces
 *********************************************************************************************************
 */
typedef struct
{
    char bLength;
    char bDescriptorType;

    char bFirstInterface;
    char bInterfaceCount;
    char bFunctionClass;
    char bFunctionSubClass;
    char bFunctionProtocol;
    char iFunction;
} usb_interface_assoc_descriptor;

#endif /* __C251__ */

#endif  /* _USB_H */
