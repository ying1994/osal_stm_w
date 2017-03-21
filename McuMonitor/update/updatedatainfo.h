#ifndef UPDATEDATAINFO_H
#define UPDATEDATAINFO_H

#include <QMutex>
#include <QVector>
#include <QByteArray>
#include "comm/bdcomm.h"

class UpdateDataInfo
{
public:
    static QMutex m_FileMutex;
public:
    ~UpdateDataInfo();
    static UpdateDataInfo* getInstance();

    void setFileSize(quint64 filesize);
    quint64 getFileSize(void);

    void setFileCount(quint16 filecount);
    quint16 getFileCount(void);

    quint16 getFileCRCValue(void);

    void setFileIndex(quint16 fileindex);
    quint16 getFileIndex(void);

    void setUpdateNodeAddr(quint16 addr);
    quint16 getUpdateNodeAddr(void);

    void setUpdateStage(quint16 stage);
    quint16 getUpdateStage(void);

    void setUpDateData(const QByteArray data);
    quint8* getUpDateData(void);

    quint32 getUpdateDataSize(void);

    void setOffset(quint32 offset);
    quint32 getOffset(void);

    void setMsgSendFlag(bool bOn);
    bool getMsgSendFlag(void);

private:
    UpdateDataInfo();
    void setFileCRCValue(quint16 crcval);

private:
    qint64 m_ulFileSize;        //文件大小
    quint16 m_uFileCount;        //文件大小
    quint16 m_uFileCRCValue;    //文件CRC校验
    quint16 m_uFileIndex;        //升级文件序号

    quint16 m_uUpdateNodeAddr; //升级结点地址
    quint16 m_uUpdateStage; //当前升级阶段

    quint8* m_pdata;    //升级文件数据
    quint32 m_uDataSize;//数据大小

    QVector<quint32> m_vBaseAddrs; //数据下载基地址
    quint32 m_uOffset; //数据下载指针偏移

    BD_MsgTypeDef m_txMessage;

    bool m_bMsgSendFlag;//消息发送标志
};

#endif // UPDATEFILEINFO

