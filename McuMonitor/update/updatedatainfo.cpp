#include "updatedatainfo.h"
#include "crc/osal_crc.h"

QMutex UpdateDataInfo::m_FileMutex(QMutex::Recursive);

UpdateDataInfo::UpdateDataInfo()
{
    m_pdata = NULL;
}

UpdateDataInfo::~UpdateDataInfo()
{
    if (m_pdata != NULL)
    {
        delete[] m_pdata;
        m_pdata = NULL;
    }
}

UpdateDataInfo* UpdateDataInfo::getInstance()
{
    static UpdateDataInfo stUpdateDataInfoInstance;
    return &stUpdateDataInfoInstance;
}

void UpdateDataInfo::setFileSize(quint64 filesize)
{
    m_ulFileSize = filesize;
}
quint64 UpdateDataInfo::getFileSize(void)
{
    return m_ulFileSize;
}

void UpdateDataInfo::setFileCount(quint16 filecount)
{
    m_uFileCount = filecount;
}

quint16 UpdateDataInfo::getFileCount(void)
{
    return m_uFileCount;
}

void UpdateDataInfo::setFileCRCValue(quint16 crcval)
{
    m_uFileCRCValue = crcval;
}

quint16 UpdateDataInfo::getFileCRCValue(void)
{
    return m_uFileCRCValue;
}

void UpdateDataInfo::setFileIndex(quint16 fileindex)
{
    m_uFileIndex = fileindex;
}
quint16 UpdateDataInfo::getFileIndex(void)
{
    return m_uFileIndex;
}

void UpdateDataInfo::setUpdateNodeAddr(quint16 addr)
{
    m_uUpdateNodeAddr = addr;
}
quint16 UpdateDataInfo::getUpdateNodeAddr(void)
{
    return m_uUpdateNodeAddr;
}

void UpdateDataInfo::setUpdateStage(quint16 stage)
{
    m_uUpdateStage = stage;
}
quint16 UpdateDataInfo::getUpdateStage(void)
{
    return m_uUpdateStage;
}

void UpdateDataInfo::setUpDateData(const QByteArray data)
{
    if (m_pdata != NULL)
    {
        delete[] m_pdata;
        m_pdata = NULL;
    }
    int datasize = data.size();
    if (datasize > 0)
    {
        m_pdata = new quint8[datasize];
        memcpy(m_pdata, data.data(), datasize);
        m_uDataSize = datasize;
        m_uFileCRCValue = bd_crc_CRC16(0, m_pdata, m_uDataSize);
    }
    else
    {
        m_uDataSize = 0;
        m_uFileCRCValue = 0;
    }
}
quint8* UpdateDataInfo::getUpDateData(void)
{
    return m_pdata;
}

quint32 UpdateDataInfo::getUpdateDataSize(void)
{
    return m_uDataSize;
}

void UpdateDataInfo::setOffset(quint32 offset)
{
    m_uOffset = offset;
}

quint32 UpdateDataInfo::getOffset(void)
{
    return m_uOffset;
}

void UpdateDataInfo::setMsgSendFlag(bool bOn)
{
    m_bMsgSendFlag = bOn;
}
bool UpdateDataInfo::getMsgSendFlag(void)
{
    return m_bMsgSendFlag;
}

