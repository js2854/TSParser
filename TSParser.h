#ifndef __TS_PARSER_H__
#define __TS_PARSER_H__

#include "types.h"
#include <string>
using namespace std;

typedef enum TS_ERR
{
    TS_OK = 0,
    TS_IN_PARAM_ERR,
    TS_SYNC_BYTE_ERR,
    TS_FILE_OPEN_FAIL,
    TS_FILE_SEEK_FAIL,
}TS_ERR;

// PID种类
typedef enum E_PKT_TYPE
{
    E_PAT       = 0,
    E_PMT       = 1,
    E_PCR       = 2,
    E_AUDIO     = 3,
    E_VIDEO     = 4,
    E_MAX       = 5
}E_PKT_TYPE;

class TSPacket
{
public:
    TSPacket()
        : m_pBuf(NULL)
        , m_pHdr(NULL)
        , m_u16PID(PID_UNSPEC)
        , m_u8CC(0)
        , m_u16PMTPID(PID_UNSPEC)
        , m_u8StreamId(0)
        , m_s64PCR(INVALID_VAL)
        , m_s64PTS(INVALID_VAL)
        , m_s64DTS(INVALID_VAL)
    {}
    ~TSPacket() {}

    TS_ERR Parse(const char *pBuf, uint16 u16BufLen);

    uint16 GetPID() const { return m_u16PID; }
    uint8  GetCC() const { return m_u8CC; }

    bool   IsPAT() { return (PID_PAT == m_u16PID); }
    uint16 GetPMTPID() const { return m_u16PMTPID; }

    bool   IsPMT() { return (PID_UNSPEC != m_u16PID) && (s_au16PIDs[E_PMT] == m_u16PID); }
    bool   IsVideo() { return (s_au16PIDs[E_VIDEO] == m_u16PID); }
    bool   IsAudio() { return (s_au16PIDs[E_AUDIO] == m_u16PID); }

    sint64 GetPCR() const { return m_s64PCR; }
    sint64 GetPTS() const { return m_s64PTS; }
    sint64 GetDTS() const { return m_s64DTS; }

public:
    static uint16 s_au16PIDs[E_MAX]; // 记录所有pid

private:
    bool   __HasAdaptField();
    bool   __HasPayload();
    AdaptFixedPart* __GetAdaptField();
    uint8  __GetAdaptLen();
    sint64 __GetPCR();
    bool   __IsVideoStream(uint8 u8StreamType);
    bool   __IsAudioStream(uint8 u8StreamType);
    uint8  __GetPayloadOffset();
    uint8  __GetTableStartPos();
    sint64 __GetPTS(const OptionPESHdrFixedPart *pHdr);
    sint64 __GetDTS(const OptionPESHdrFixedPart *pHdr);
    TS_ERR __ParsePAT();
    TS_ERR __ParsePMT();
    TS_ERR __ParsePES();

private:
    const char      *m_pBuf;
    TSHdrFixedPart  *m_pHdr;
    uint16          m_u16PID;
    uint8           m_u8CC;
    uint16          m_u16PMTPID;
    uint8           m_u8StreamId;
    sint64          m_s64PCR;
    sint64          m_s64PTS;
    sint64          m_s64DTS;
};

class TSParser
{
public:
    TSParser(const char *pFilePath);
    ~TSParser();

    TS_ERR Parse();    

private:
    TS_ERR __OpenFile();
    TS_ERR __CloseFile();
    bool   __SeekToFirstPkt();
    void   __PrintPacketInfo(TSPacket &tPkt, uint64 u64Offset);

private:
    string m_strFile;
    FILE*  m_pFd;
};

#define DELETER_BUFFER(p)   if (NULL != p) { delete p; p = NULL;}

class AutoDelCharBuf
{
public:
    AutoDelCharBuf(char *pBuf) : m_pBuf(pBuf) {}
    ~AutoDelCharBuf() { DELETER_BUFFER(m_pBuf); }

    char *Ptr() { return m_pBuf; }
private:
    char *m_pBuf;
};

#endif //__TS_PARSER_H__

