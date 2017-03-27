#include "TSParser.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#define MAX_READ_PKT_NUM                10000
#define MAX_CHECK_PKT_NUM               3
#define MAX_TIME_STR_LEN                20

#define MK_WORD(high,low)               (((high)<<8)|(low))
#define MK_PCR(b1,b2,b3,b4,b5)          (((sint64)(b1)<<25)|((sint64)(b2)<<17)|((sint64)(b3)<<9)|((sint64)(b4)<<1)|(b5))
#define MK_PTS_DTS(b1,b2,b3,b4,b5)      (((sint64)(b1)<<30)|((sint64)(b2)<<22)|((sint64)(b3)<<15)|((sint64)(b4)<<7)|(b5))

#define MIN(a,b)                        (((a) < (b)) ? (a) : (b))
#define RETURN_IF_NOT_OK(ret)           if (TS_OK != ret) { return ret; }
#define PRINT(fmt,...)                  printf(fmt, ## __VA_ARGS__)
#define PRINT_LINE(fmt,...)             printf(fmt" -- [%s:%d]\n", ## __VA_ARGS__, __FILE__, __LINE__)

// 记录所有pid
uint16 TSPacket::s_au16PIDs[E_MAX] = {PID_UNSPEC,PID_UNSPEC,PID_UNSPEC,PID_UNSPEC,PID_UNSPEC};

/*****************************************************************************
 * 函 数 名  : TSPacket.Parse
 * 函数功能  : 解析TS包
 * 参    数  : const uint8 *pBuf
               uint16 u16BufLen
 * 返 回 值  : TS_ERR
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
TS_ERR TSPacket::Parse(const uint8 *pBuf, uint16 u16BufLen)
{
    assert(NULL != pBuf);

    TS_ERR ret = TS_OK;
    if ((NULL == pBuf) || (TS_PKT_LEN != u16BufLen))
    {
        return TS_IN_PARAM_ERR;
    }

    if (TS_SYNC_BYTE != pBuf[0])
    {
        return TS_SYNC_BYTE_ERR;
    }

    m_pBuf = pBuf;

    m_pHdr = (TSHdrFixedPart*)pBuf;
    m_u16PID = MK_WORD(m_pHdr->pid12_8,m_pHdr->pid7_0);
    m_u8CC   = m_pHdr->continuity_counter;

    if (IsPAT())
    {
        ret = __ParsePAT();
    }
    else if (IsPMT())
    {
        ret = __ParsePMT();
    }
    else if (m_u16PID == s_au16PIDs[E_PCR])
    {
        m_s64PCR = __GetPCR();
    }

    if (m_pHdr->payload_unit_start_indicator)
    {
        ret = __ParsePES();
    }

    return ret;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__HasAdaptField
 * 函数功能  : 判断是否存在适应区域
 * 参    数  : 无
 * 返 回 值  : bool
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
bool TSPacket::__HasAdaptField()
{
    assert(NULL != m_pHdr);
    return 0 != (m_pHdr->adaptation_field_control & 0x2);
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__HasPayload
 * 函数功能  : 判断是否存在负载
 * 参    数  : 无
 * 返 回 值  : bool
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
bool TSPacket::__HasPayload()
{
    assert(NULL != m_pHdr);
    return m_pHdr->payload_unit_start_indicator || (m_pHdr->adaptation_field_control & 0x1);
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__GetAdaptField
 * 函数功能  : 获取适应区域指针;适应区域不存在时返回NULL
 * 参    数  : 无
 * 返 回 值  : AdaptFixedPart*
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
AdaptFixedPart* TSPacket::__GetAdaptField()
{
    assert(NULL != m_pBuf);
    assert(NULL != m_pHdr);

    AdaptFixedPart *pAdpt = NULL;

    if (__HasAdaptField())
    {
        pAdpt = (AdaptFixedPart*)(m_pBuf + sizeof(TSHdrFixedPart));
    }

    return pAdpt;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__GetAdaptLen
 * 函数功能  : 获取适应区域的长度
 * 参    数  : 无
 * 返 回 值  : uint8
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
uint8 TSPacket::__GetAdaptLen()
{
    uint8 u8AdaptLen = 0;

    AdaptFixedPart *pAdpt = __GetAdaptField();

    if (NULL != pAdpt)
    {
        // "adaptation_field_length" field is 1 byte
        u8AdaptLen = pAdpt->adaptation_field_length + 1;
    }

    return u8AdaptLen;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__GetPCR
 * 函数功能  : 存在PCR字段时,获取PCR的值;不存在时返回-1
 * 参    数  : 无
 * 返 回 值  : sint64
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
sint64 TSPacket::__GetPCR()
{
    assert(NULL != m_pBuf);
    assert(NULL != m_pHdr);

    sint64 s64PCR = INVALID_VAL;
    if (__HasAdaptField())
    {
        AdaptFixedPart *pAdpt = (AdaptFixedPart*)(m_pBuf + sizeof(TSHdrFixedPart));
        if (pAdpt->adaptation_field_length > 0 && pAdpt->PCR_flag)
        {
            PCR *pcr = (PCR*)((const char*)pAdpt + sizeof(AdaptFixedPart));
            s64PCR = MK_PCR(pcr->pcr_base32_25,
                                pcr->pcr_base24_17,
                                pcr->pcr_base16_9,
                                pcr->pcr_base8_1,
                                pcr->pcr_base0);
        }
    }
    return s64PCR;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__IsVideoStream
 * 函数功能  : 根据StreamType判断是否视频流
 * 参    数  : uint8 u8StreamType
 * 返 回 值  : bool
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
bool TSPacket::__IsVideoStream(uint8 u8StreamType)
{
    return ((ES_TYPE_MPEG1V == u8StreamType)
        || (ES_TYPE_MPEG2V == u8StreamType)
        || (ES_TYPE_MPEG4V == u8StreamType)
        || (ES_TYPE_H264 == u8StreamType));
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__IsAudioStream
 * 函数功能  : 根据StreamType判断是否音频流
 * 参    数  : uint8 u8StreamType
 * 返 回 值  : bool
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
bool TSPacket::__IsAudioStream(uint8 u8StreamType)
{
    return ((ES_TYPE_MPEG1A == u8StreamType)
        || (ES_TYPE_MPEG2A == u8StreamType)
        || (ES_TYPE_AC3 == u8StreamType)
        || (ES_TYPE_AAC == u8StreamType)
        || (ES_TYPE_DTS == u8StreamType));
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__GetPayloadOffset
 * 函数功能  : 获取负载相对于TS包头的偏移
 * 参    数  : 无
 * 返 回 值  : uint8
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
uint8 TSPacket::__GetPayloadOffset()
{
    uint8 u8Pos = sizeof(TSHdrFixedPart);
    if (__HasAdaptField())
    {
        u8Pos += __GetAdaptLen();;
    }
    return u8Pos;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__GetTableStartPos
 * 函数功能  : 获取PAT/PMT表相对于TS包头的偏移
 * 参    数  : 无
 * 返 回 值  : uint8
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
uint8 TSPacket::__GetTableStartPos()
{
    assert(NULL != m_pBuf);

    uint8 u8Pos = __GetPayloadOffset();
    if (__HasPayload())
    {
        // "pointer_field" field is 1 byte,
        // and whose value is the number of bytes before payload
        uint8 u8PtrFieldLen = m_pBuf[u8Pos] + 1;
        u8Pos += u8PtrFieldLen;
    }
    return u8Pos;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__GetPTS
 * 函数功能  : 存在PTS字段时,获取PTS的值;不存在时返回-1
 * 参    数  : const OptionPESHdrFixedPart *pHdr
 * 返 回 值  : sint64
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
sint64 TSPacket::__GetPTS(const OptionPESHdrFixedPart *pHdr)
{
    assert(NULL != pHdr);

    sint64 s64PTS = INVALID_VAL;
    if (pHdr->PTS_DTS_flags & 0x2)
    {
        PTS_DTS *pPTS = (PTS_DTS*)((char*)pHdr + sizeof(OptionPESHdrFixedPart));
        s64PTS = MK_PTS_DTS(pPTS->ts32_30, pPTS->ts29_22, pPTS->ts21_15, pPTS->ts14_7, pPTS->ts6_0);
    }

    return s64PTS;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__GetDTS
 * 函数功能  : 存在DTS字段时,获取DTS的值;不存在时返回-1
 * 参    数  : const OptionPESHdrFixedPart *pHdr
 * 返 回 值  : sint64
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
sint64 TSPacket::__GetDTS(const OptionPESHdrFixedPart *pHdr)
{
    assert(NULL != pHdr);

    sint64 s64DTS = INVALID_VAL;
    if (pHdr->PTS_DTS_flags & 0x1)
    {
        PTS_DTS *pDTS = (PTS_DTS*)((char*)pHdr + sizeof(OptionPESHdrFixedPart) + sizeof(PTS_DTS));
        s64DTS = MK_PTS_DTS(pDTS->ts32_30, pDTS->ts29_22, pDTS->ts21_15, pDTS->ts14_7, pDTS->ts6_0);
    }

    return s64DTS;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__ParsePAT
 * 函数功能  : 解析PAT表,获取PMT信息
 * 参    数  : 无
 * 返 回 值  : TS_ERR
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
TS_ERR TSPacket::__ParsePAT()
{
    assert(NULL != m_pBuf);

    const uint8 *pPATBuf = m_pBuf + __GetTableStartPos();
    PATHdrFixedPart *pPAT = (PATHdrFixedPart*)pPATBuf;
    uint16 u16SectionLen = MK_WORD(pPAT->section_length11_8, pPAT->section_length7_0);
    uint16 u16AllSubSectionLen = u16SectionLen - (sizeof(PATHdrFixedPart) - HDR_LEN_NOT_INCLUDE) - CRC32_LEN;

    uint16 u16SubSectionLen = sizeof(PATSubSection);
    const uint8 *ptr = pPATBuf + sizeof(PATHdrFixedPart);
    for (uint16 i = 0; i < u16AllSubSectionLen; i+= u16SubSectionLen)
    {
        PATSubSection *pDes = (PATSubSection*)(ptr + i);
        uint16 u16ProgNum = pDes->program_number;
        uint16 u16PID = MK_WORD(pDes->pid12_8, pDes->pid7_0);
        if (0x00 == u16ProgNum)
        {
            uint16 u16NetworkPID = u16PID;
        }
        else
        {
            m_u16PMTPID = u16PID;// program_map_PID
            break;
        }
    }

    s_au16PIDs[E_PMT] = m_u16PMTPID;
    return TS_OK;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__ParsePMT
 * 函数功能  : 解析PMT表,获取PCR,视频和音频pid信息
 * 参    数  : 无
 * 返 回 值  : TS_ERR
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
TS_ERR TSPacket::__ParsePMT()
{
    assert(NULL != m_pBuf);

    const uint8 *pPMTBuf = m_pBuf + __GetTableStartPos();
    PMTHdrFixedPart *pPMT = (PMTHdrFixedPart*)pPMTBuf;
    s_au16PIDs[E_PCR] = MK_WORD(pPMT->PCR_PID12_8, pPMT->PCR_PID7_0);
    uint16 u16SectionLen = MK_WORD(pPMT->section_length11_8, pPMT->section_length7_0);
    // n * program_info_descriptor的长度
    uint16 u16ProgInfoLen = MK_WORD(pPMT->program_info_length11_8, pPMT->program_info_length7_0);
    uint16 u16AllSubSectionLen = u16SectionLen - (sizeof(PMTHdrFixedPart) - HDR_LEN_NOT_INCLUDE) - u16ProgInfoLen - CRC32_LEN;

    uint16 u16SubSectionLen = sizeof(PMTSubSectionFixedPart);
    const uint8 *ptr = pPMTBuf + sizeof(PMTHdrFixedPart) + u16ProgInfoLen;
    for (uint16 i = 0; i < u16AllSubSectionLen; i += u16SubSectionLen)
    {
        PMTSubSectionFixedPart *pSec = (PMTSubSectionFixedPart*)(ptr + i);
        uint16 u16ElementaryPID = MK_WORD(pSec->elementaryPID12_8, pSec->elementaryPID7_0);
        uint16 u16ESInfoLen = MK_WORD(pSec->ES_info_lengh11_8, pSec->ES_info_lengh7_0);
        u16SubSectionLen += u16ESInfoLen;

        if (__IsVideoStream(pSec->stream_type))
        {
            s_au16PIDs[E_VIDEO] = u16ElementaryPID;
        }
        else if (__IsAudioStream(pSec->stream_type))
        {
            s_au16PIDs[E_AUDIO] = u16ElementaryPID;
        }
    }
    return TS_OK;
}

/*****************************************************************************
 * 函 数 名  : TSPacket.__ParsePES
 * 函数功能  : 解析PES,获取PTS和DTS
 * 参    数  : 无
 * 返 回 值  : TS_ERR
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
TS_ERR TSPacket::__ParsePES()
{
    assert(NULL != m_pBuf);

    const uint8 *pPESBuf = m_pBuf + __GetPayloadOffset();
    PESHdrFixedPart *pPES = (PESHdrFixedPart*)pPESBuf;

    if (PES_START_CODE == pPES->packet_start_code_prefix)
    {
        m_u8StreamId = pPES->stream_id;
        if ((m_u8StreamId & PES_STREAM_VIDEO) || (m_u8StreamId & PES_STREAM_AUDIO))
        {
            OptionPESHdrFixedPart *pHdr = (OptionPESHdrFixedPart*)(pPESBuf + sizeof(PESHdrFixedPart));
            m_s64PTS = __GetPTS(pHdr);
            m_s64DTS = __GetDTS(pHdr);
        }
    }
    return TS_OK;
}

TSParser::TSParser(const char *pFilePath) : m_strFile(""), m_pFd(NULL)
{
    m_strFile = pFilePath;
}
TSParser::~TSParser()
{
    __CloseFile();
}

/*****************************************************************************
 * 函 数 名  : TSParser.Parse
 * 函数功能  : 解析TS文件
 * 参    数  : 无
 * 返 回 值  : TS_ERR
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
TS_ERR TSParser::Parse()
{
    TS_ERR ret = TS_OK;
    ret = __OpenFile();
    RETURN_IF_NOT_OK(ret);

    if (!__SeekToFirstPkt())
    {
        PRINT_LINE("Seek to first packet failed!");
        return TS_FILE_SEEK_FAIL;
    }

    sint64 s64CurPos = ftello64(m_pFd);
    PRINT_LINE("Seek to first packet, offset: 0x%08llX", s64CurPos);

    uint32 u32ReadBufLen = MAX_READ_PKT_NUM * TS_PKT_LEN;
    uint8 *pReadBuf = new uint8[u32ReadBufLen];
    AutoDelCharBuf tBuf(pReadBuf);
    uint32 u32PktNo = 0;
    while (0 == feof(m_pFd))
    {
        sint32 s32ReadLen = fread(pReadBuf, 1, u32ReadBufLen, m_pFd);
        if (s32ReadLen >= 0)
        {
            uint32 u32Count = s32ReadLen / TS_PKT_LEN;
            for (uint32 i = 0; i < u32Count; i++)
            {
                if (TS_SYNC_BYTE == pReadBuf[i*TS_PKT_LEN])
                {
                    TSPacket tPkt;
                    ret = tPkt.Parse(pReadBuf + i*TS_PKT_LEN, TS_PKT_LEN);
                    RETURN_IF_NOT_OK(ret);

                    __PrintPacketInfo(tPkt, s64CurPos, u32PktNo);
                    s64CurPos += TS_PKT_LEN;
                    u32PktNo++;
                }
                else
                {
                    PRINT_LINE("###### PktNo: %08d, Offset: 0x%08X, Sync byte error! First byte<0x%02X>",
                        u32PktNo, s64CurPos, pReadBuf[i*TS_PKT_LEN]);
                    if (!__SeekToFirstPkt(s64CurPos))
                    {
                        PRINT_LINE("###### Seek to next packet failed!");
                        return TS_FILE_SEEK_FAIL;
                    }
                    s64CurPos = ftello64(m_pFd);
                    break;
                }
            }
        }
        else
        {
            PRINT_LINE("###### Read file error, ret<%d>", s32ReadLen);
            break;
        }
    }

    PRINT_LINE("Parse file complete!");
    return ret;
}

/*****************************************************************************
 * 函 数 名  : TSParser.__OpenFile
 * 函数功能  : 打开TS文件
 * 参    数  : 无
 * 返 回 值  : TS_ERR
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
TS_ERR TSParser::__OpenFile()
{
    m_pFd = fopen(m_strFile.c_str(), "rb");
    if (NULL == m_pFd)
    {
        PRINT_LINE("###### Open file<%s> failed! errno<%d>", m_strFile.c_str(), errno);
        return TS_FILE_OPEN_FAIL;
    }

    PRINT_LINE("Open file<%s> success.", m_strFile.c_str());
    return TS_OK;
}

/*****************************************************************************
 * 函 数 名  : TSParser.__CloseFile
 * 函数功能  : 关闭文件
 * 参    数  : 无
 * 返 回 值  : TS_ERR
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
TS_ERR TSParser::__CloseFile()
{
    if (NULL != m_pFd)
    {
        fclose(m_pFd);
        m_pFd = NULL;
        PRINT_LINE("Close file<%s>", m_strFile.c_str());
    }
    return TS_OK;
}

/*****************************************************************************
 * 函 数 名  : TSParser.__SeekToFirstPkt
 * 函数功能  : 将文件读取指针偏移至第一个合法的TS包
 * 参    数  : uint64 u64Offset
 * 返 回 值  : bool
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
bool TSParser::__SeekToFirstPkt(uint64 u64Offset)
{
    fseek(m_pFd, u64Offset, SEEK_SET);

    uint32 u32ReadBufLen = MAX_READ_PKT_NUM * TS_PKT_LEN;
    uint8 *pReadBuf = new uint8[u32ReadBufLen];
    AutoDelCharBuf tBuf(pReadBuf);
    sint32 s32ReadLen = fread(pReadBuf, 1, u32ReadBufLen, m_pFd);
    if (s32ReadLen > 0)
    {
        uint32 u32PktCount = s32ReadLen / TS_PKT_LEN;
        uint32 u32Count = MIN(MAX_CHECK_PKT_NUM, u32PktCount);
        for (uint32 i = 0; i < s32ReadLen - u32Count*TS_PKT_LEN; i++)
        {
            if (TS_SYNC_BYTE == pReadBuf[i])
            {
                uint32 n = 0;
                for (; n < u32Count; n++)
                {
                    if (TS_SYNC_BYTE != pReadBuf[i + n*TS_PKT_LEN])
                    {
                        break;
                    }
                }

                if (u32Count == n)
                {
                    return (0 == fseek(m_pFd, u64Offset+i, SEEK_SET));
                }
            }
        }
    }
    else
    {
        PRINT_LINE("###### Read file error, ret<%d>", s32ReadLen);
    }

    return false;
}

/*****************************************************************************
 * 函 数 名  : TSParser.__PrintPacketInfo
 * 函数功能  : 打印TS包的信息
 * 参    数  : TSPacket &tPkt
               uint64 u64Offset
               uint32 u32PktNo
 * 返 回 值  : void
 * 作    者  : JiaSong
 * 创建日期  : 2015-8-29
*****************************************************************************/
void TSParser::__PrintPacketInfo(TSPacket &tPkt, uint64 u64Offset, uint32 u32PktNo)
{
    PRINT("PktNo: %08u, Offset: 0x%08llX, PID: 0x%04X, CC: %02u",
          u32PktNo, u64Offset, tPkt.GetPID(), tPkt.GetCC());

    if (tPkt.IsPAT())
    {
        PRINT(", PAT");
    }
    else if (tPkt.IsPMT())
    {
        PRINT(", PMT");
    }
    else if (tPkt.GetPCR() >= 0)
    {
        PRINT(", PCR: %lld(%s)", tPkt.GetPCR(), __TSTimeToStr(tPkt.GetPCR()));
    }
    else if (PID_NULL == tPkt.GetPID())
    {
        PRINT(", Null Packet");
    }

    if (tPkt.GetPTS() >= 0)
    {
        PRINT(", PTS: %lld(%s)", tPkt.GetPTS(), __TSTimeToStr(tPkt.GetPTS()));
    }
    if (tPkt.GetDTS() >= 0)
    {
        PRINT(", DTS: %lld(%s)", tPkt.GetDTS(), __TSTimeToStr(tPkt.GetDTS()));
    }

    if (tPkt.IsVideo())
    {
        PRINT(", Video");
    }
    else if (tPkt.IsAudio())
    {
        PRINT(", Audio");
    }

    PRINT_LINE("");
}

/*****************************************************************************
 * 函 数 名  : TSParser.__PrintPacketInfo
 * 函数功能  : TS时戳转化成可读时间字符串
 * 参    数  : sint64 s64Time
 * 返 回 值  : const char *
 * 作    者  : JiaSong
 * 创建日期  : 2016-4-11
*****************************************************************************/
const char *TSParser::__TSTimeToStr(sint64 s64Time)
{
    static char s_acTimeStr[MAX_TIME_STR_LEN] = {0};
    sint64 s64MiliSecond = s64Time / 90;
    sint64 s64Second = s64MiliSecond / 1000;
    snprintf(s_acTimeStr, sizeof(s_acTimeStr), "%02d:%02d:%02d.%03d",
        s64Second/3600, (s64Second%3600)/60, s64Second%60, s64MiliSecond%1000);
    return s_acTimeStr;
}

