#ifndef __TS_TYPES__
#define __TS_TYPES__

#include <stdlib.h>
#include <inttypes.h>

typedef unsigned char               uint8;
typedef unsigned short              uint16;
typedef unsigned int                uint32;

typedef signed char                 sint8;
typedef signed short                sint16;
typedef signed int                  sint32;

#ifdef _WIN32
typedef unsigned __int64            uint64;
typedef signed __int64              uint64;
#else
typedef unsigned long long          uint64;
typedef signed long long            sint64;
#endif

#define true                        1
#define false                       0

#define INVALID_VAL                 -1

#define TS_PKT_LEN                  188
#define TS_SYNC_BYTE                0x47
#define STUFFING_BYTE               0xFF
#define PES_START_CODE              0x010000    // PES分组起始标志0x000001
#define CRC32_LEN                   4


#define PID_PAT                     0x0000
#define PID_CAT                     0x0001
#define PID_TSDT                    0x0002
#define PID_DVB_NIT                 0x0010
#define PID_DVB_SDT                 0x0011
#define PID_DVB_EIT                 0x0012
#define PID_DVB_RST                 0x0013
#define PID_DVB_TDT                 0x0014
#define PID_DVB_SYNC                0x0015
#define PID_DVB_INBAND              0x001c
#define PID_DVB_MEASUREMENT         0x001d
#define PID_DVB_DIT                 0x001e
#define PID_DVB_SIT                 0x001f
#define PID_NULL                    0x1fff
#define PID_UNSPEC                  0xffff

#define PES_STREAM_VIDEO            0xE0
#define PES_STREAM_AUDIO            0xC0

#define ES_TYPE_MPEG1V              0x01
#define ES_TYPE_MPEG2V              0x02
#define ES_TYPE_MPEG1A              0x03
#define ES_TYPE_MPEG2A              0x04
#define ES_TYPE_PRIVATESECTS        0x05
#define ES_TYPE_PRIVATEDATA         0x06
#define ES_TYPE_MHEG                0x07
#define ES_TYPE_DSMCC               0x08
#define ES_TYPE_AUXILIARY           0x09
#define ES_TYPE_DSMCC_ENCAP         0x0a
#define ES_TYPE_DSMCC_UN            0x0b
#define ES_TYPE_AAC                 0x0f
#define ES_TYPE_MPEG4V              0x10
#define ES_TYPE_LATM_AAC            0x11
#define ES_TYPE_MPEG4_GENERIC       0x12
/* Unknown 0x13 */
#define ES_TYPE_DSMCC_DOWNLOAD      0x14
/* Unknown 0x15 */
/* Unknown 0x16 */
/* Unknown 0x17 */
/* Unknown 0x1a */
#define ES_TYPE_H264                0x1b
#define ES_TYPE_DIGICIPHER2V        0x80
#define ES_TYPE_AC3                 0x81
#define ES_TYPE_DCA                 0x82
#define ES_TYPE_LPCM                0x83
#define ES_TYPE_SDDS                0x84
#define ES_TYPE_ATSC_PROGID         0x85
#define ES_TYPE_DTSHD               0x86
#define ES_TYPE_EAC3                0x87
#define ES_TYPE_DTS                 0x8a
#define ES_TYPE_DVB_SLICE           0x90
#define ES_TYPE_AC3B                0x91
#define ES_TYPE_SUBTITLE            0x92
#define ES_TYPE_SDDSB               0x94
#define ES_TYPE_MSCODEC             0xa0
#define ES_TYPE_DIRAC               0xd1
#define ES_TYPE_VC1                 0xea

#define TID_PAT                     0x00
#define TID_CAT                     0x01
#define TID_PMT                     0x02
#define TID_DVB_NIT                 0x40
#define TID_DVB_ONIT                0x41

#define DESC_VIDEO                  0x02
#define DESC_AUDIO                  0x03
#define DESC_HIERARCHY              0x04
#define DESC_REGISTRATION           0x05
#define DESC_DSA                    0x06 /* data stream alignment */
#define DESC_TBG                    0x07 /* target background grid */
#define DESC_VIDEOWINDOW            0x08
#define DESC_CA                     0x09
#define DESC_LANGUAGE               0x0a
#define DESC_CLOCK                  0x0b
#define DESC_MBU                    0x0c /* multiplex buffer utilisation */
#define DESC_COPYRIGHT              0x0d
#define DESC_MAXBITRATE             0x0e

#define PT_UNSPEC                   0
#define PT_SECTIONS                 1
#define PT_PES                      2
#define PT_DATA                     3
#define PT_NULL                     4

#define PST_UNSPEC                  0
#define PST_VIDEO                   1
#define PST_AUDIO                   2
#define PST_INTERACTIVE             3
#define PST_CC                      4
#define PST_IP                      5
#define PST_SI                      6
#define PST_NI                      7

#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif

#pragma pack(push, 1)  // 1字节对齐
typedef struct TSHdrFixedPart
{
    uint8  sync_byte:8;
#ifdef BIG_ENDIAN
    uint8 transport_error_indicator:1;
    uint8 payload_unit_start_indicator:1;
    uint8 transport_priority:1;
    uint8 pid12_8:5;
    uint8 pid7_0:8;
    uint8 transport_scrambling_control:2;
    uint8 adaptation_field_control:2;
    uint8 continuity_counter:4;
#else // LITTLE_ENDIAN
    uint8 pid12_8:5;
    uint8 transport_priority:1;
    uint8 payload_unit_start_indicator:1;
    uint8 transport_error_indicator:1;
    uint8 pid7_0:8;
    uint8 continuity_counter:4;
    uint8 adaptation_field_control:2;
    uint8 transport_scrambling_control:2;
#endif
}TSHdrFixedPart;

typedef struct AdaptFixedPart
{
    uint8 adaptation_field_length:8;
#ifdef BIG_ENDIAN
    uint8 discontinuity_indicator:1;
    uint8 random_access_indicator:1;
    uint8 elementary_stream_priority_indicator:1;
    uint8 PCR_flag:1;
    uint8 OPCR_flag:1;
    uint8 splicing_point_flag:1;
    uint8 transport_private_data_flag:1;
    uint8 adaptation_field_extension_flag:1;
#else // LITTLE_ENDIAN
    uint8 adaptation_field_extension_flag:1;
    uint8 transport_private_data_flag:1;
    uint8 splicing_point_flag:1;
    uint8 OPCR_flag:1;
    uint8 PCR_flag:1;
    uint8 elementary_stream_priority_indicator:1;
    uint8 random_access_indicator:1;
    uint8 discontinuity_indicator:1;
#endif
}AdaptFixedPart;

typedef struct PCR
{
    uint8 pcr_base32_25:8;
    uint8 pcr_base24_17:8;
    uint8 pcr_base16_9:8;
    uint8 pcr_base8_1:8;
#ifdef BIG_ENDIAN
    uint8 pcr_base0:1;
    uint8 reserved:6;
    uint8 pcr_extension8:1;
#else // LITTLE_ENDIAN
    uint8 pcr_extension8:1;
    uint8 reserved:6;
    uint8 pcr_base0:1;
#endif
    uint8 pcr_extension7_0:8;
}PCR;

typedef struct PESHdrFixedPart
{
    uint32 packet_start_code_prefix:24;
    uint8  stream_id:8;
    uint16 PES_packet_length:16;
}PESHdrFixedPart;

typedef struct OptionPESHdrFixedPart
{
#ifdef BIG_ENDIAN
    uint8 fix_2bits:2;              // "10"
    uint8 PES_scrambling_control:2; // "00" Not scramble; "01","10","11" User-defined
    uint8 PES_priority:1;
    uint8 data_alignment_indicator:1;
    uint8 copyright:1;
    uint8 original_or_copy:1;

    uint8 PTS_DTS_flags:2;
    uint8 ESCR_flag:1;
    uint8 ES_rate_flag:1;
    uint8 DSM_trick_mode_flag:1;
    uint8 additional_copy_info_flag:1;
    uint8 PES_CRC_flag:1;
    uint8 PES_extension_flag:1;
#else // LITTLE_ENDIAN
    uint8 original_or_copy:1;
    uint8 copyright:1;
    uint8 data_alignment_indicator:1;
    uint8 PES_priority:1;
    uint8 PES_scrambling_control:2;
    uint8 fix_10:2;

    uint8 PES_extension_flag:1;
    uint8 PES_CRC_flag:1;
    uint8 additional_copy_info_flag:1;
    uint8 DSM_trick_mode_flag:1;
    uint8 ES_rate_flag:1;
    uint8 ESCR_flag:1;
    uint8 PTS_DTS_flags:2;
#endif
    uint8 PES_Hdr_data_length:8;
}OptionPESHdrFixedPart;

typedef struct PTS_DTS
{
#ifdef BIG_ENDIAN
    uint8 fix_4bits:4;  // PTS is "0010" or "0011", DTS is "0001"
    uint8 ts32_30:3;
    uint8 marker_bit1:1;
    uint8 ts29_22:8;
    uint8 ts21_15:7;
    uint8 marker_bit2:1;
    uint8 ts14_7:8;
    uint8 ts6_0:7;
    uint8 marker_bit3:1;
#else // LITTLE_ENDIAN
    uint8 marker_bit1:1;
    uint8 ts32_30:3;
    uint8 fix_4bits:4;  // PTS is "0010" or "0011", DTS is "0001"
    uint8 ts29_22:8;
    uint8 marker_bit2:1;
    uint8 ts21_15:7;
    uint8 ts14_7:8;
    uint8 marker_bit3:1;
    uint8 ts6_0:7;
#endif
}PTS_DTS;

typedef struct PATHdrFixedPart
{
    uint8 table_id:8;
#ifdef BIG_ENDIAN
    uint8 section_syntax_indicator:1;
    uint8 zero_bit:1; // '0'
    uint8 reserved1:2;
    uint8 section_length11_8:4;
    uint8 section_length7_0:8;
    uint16 transport_stream_id:16;
    uint8 reserved2:2;
    uint8 version_number:5;
    uint8 current_next_indicator:1;
#else // LITTLE_ENDIAN
    uint8 section_length11_8:4;
    uint8 reserved1:2;
    uint8 zero_bit:1; // '0'
    uint8 section_syntax_indicator:1;
    uint8 section_length7_0:8;
    uint16 transport_stream_id:16;
    uint8 current_next_indicator:1;
    uint8 version_number:5;
    uint8 reserved2:2;
#endif
    uint8 section_number:8;
    uint8 last_section_number:8;
}PATHdrFixedPart;

typedef struct PATSubSection
{
    uint16 program_number:16;
#ifdef BIG_ENDIAN
    uint8  reserved:3;
    uint8  pid12_8:5;
#else // LITTLE_ENDIAN
    uint8  pid12_8:5;
    uint8  reserved:3;
#endif
    uint8  pid7_0:8;
}PATSubSection;

typedef struct CATHdrFixedPart
{
    uint8 table_id:8;
#ifdef BIG_ENDIAN
    uint8 section_syntax_indicator:1;
    uint8 zero_bit:1; // '0'
    uint8 reserved1:2;
    uint8 section_length11_8:4;
    uint8 section_length7_0:8;
    uint16 reserved2:16;
    uint8 reserved3:2;
    uint8 version_number:5;
    uint8 current_next_indicator:1;
#else // LITTLE_ENDIAN
    uint8 section_length11_8:4;
    uint8 reserved1:2;
    uint8 zero_bit:1; // '0'
    uint8 section_syntax_indicator:1;
    uint8 section_length7_0:8;
    uint16 reserved2:16;
    uint8 current_next_indicator:1;
    uint8 version_number:5;
    uint8 reserved3:2;
#endif
    uint8 section_number:8;
    uint8 last_section_number:8;
}CATHdrFixedPart;

typedef struct PMTHdrFixedPart
{
    uint8 table_id:8;
#ifdef BIG_ENDIAN
    uint8 section_syntax_indicator:1;
    uint8 zero_bit:1; // '0'
    uint8 reserved1:2;
    uint8 section_length11_8:4;
    uint8 section_length7_0:8;
    uint16 program_number:16;
    uint8 reserved2:2;
    uint8 version_number:5;
    uint8 current_next_indicator:1;
    uint8 section_number:8;
    uint8 last_section_number:8;
    uint8 reserved3:3;
    uint8 PCR_PID12_8:5;
    uint8 PCR_PID7_0:8;
    uint8 reserved4:4;
    uint8 program_info_length11_8:4;
    uint8 program_info_length7_0:8;
#else // LITTLE_ENDIAN
    uint8 section_length11_8:4;
    uint8 reserved1:2;
    uint8 zero_bit:1; // '0'
    uint8 section_syntax_indicator:1;
    uint8 section_length7_0:8;
    uint16 transport_stream_id:16;
    uint8 current_next_indicator:1;
    uint8 version_number:5;
    uint8 reserved2:2;
    uint8 section_number:8;
    uint8 last_section_number:8;
    uint8 PCR_PID12_8:5;
    uint8 reserved3:3;
    uint8 PCR_PID7_0:8;
    uint8 program_info_length11_8:4;
    uint8 reserved4:4;
    uint8 program_info_length7_0:8;
#endif
}PMTHdrFixedPart;

typedef struct PMTSubSectionFixedPart
{
    uint8 stream_type:8;
#ifdef BIG_ENDIAN
    uint8 reserved1:3;
    uint8 elementaryPID12_8:5;
    uint8 elementaryPID7_0:8;
    uint8 reserved2:4;
    uint8 ES_info_lengh11_8:4;
    uint8 ES_info_lengh7_0:8;
#else // LITTLE_ENDIAN
    uint8 elementaryPID12_8:5;
    uint8 reserved1:3;
    uint8 elementaryPID7_0:8;
    uint8 ES_info_lengh11_8:4;
    uint8 reserved2:4;
    uint8 ES_info_lengh7_0:8;
#endif
}PMTSubSectionFixedPart;
#pragma pack(pop)   //恢复对齐状态

#endif //__TS_TYPES__

