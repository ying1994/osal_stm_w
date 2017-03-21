#include "updatedialog.h"
#include "ui_updatedialog.h"

#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QThread>
#include <QVector>

#include "unit.h"
#include "function.h"
#include "optype.h"
#include "errorcode.h"

#include "crc/osal_crc.h"
#include "socket/osal_socket.h"

#include "updatedatainfo.h"
#include "updatethread.h"

#include "updatefileoperator/updatefileoperator.h"

UpdateDialog::UpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDialog),
    m_strFileName("")
{
    ui->setupUi(this);
    this->setWindowTitle("UpDate Unit");

    this->ui->progressBarUpdate->setRange(0, 100);
    this->ui->progressBarUpdate->setValue(0);

    m_hUpdateThread = new UpdateThread();


    m_pFileDialog = NULL;
    m_pFileInfo = NULL;
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
    delete m_pFileDialog;
    delete m_pFileInfo;
    delete m_hUpdateThread;
}

//调试单元消息处理
void UpdateDialog::OnDebugUnitMsgEvent(BD_MsgTypeDef &msg)
{
    QString text;
    switch (msg.functionID)
    {
    case GET_APP_VERSION:
        msg.data[msg.len] = '\0';
        text.sprintf("%s", msg.data);
        if (text != this->ui->lineEditAppVersion->text())
            this->ui->lineEditAppVersion->setText(text);
        break;
    case GET_BIOS_VERSION:
        msg.data[msg.len] = '\0';
        text.sprintf("%s", msg.data);
        if (text != this->ui->lineEditBiosVersion->text())
            this->ui->lineEditBiosVersion->setText(text);
        break;
    case GET_CHIP_ID:
        break;
    default:
        break;
    }
}

//升级单元消息处理
void UpdateDialog::OnUpDateUnitMsgEvent(BD_MsgTypeDef &msg)
{
    quint32 offset = 0;   //文件偏移量
    quint16 filecount = 0;//文件数量
    quint16 fileindex = 0;//文件序号
    quint32 filesize = 0; //文件大小
    quint16 crc16 = 0; //文件CRC16
    quint16 crcval = 0;

    UpdateDataInfo* pUpdateinfo = UpdateDataInfo::getInstance();

    //DBG(TRACE("UpdateStage: %04X", msg.functionID));

    pUpdateinfo->m_FileMutex.lock();
    switch (msg.functionID)
    {
    case START:
        filecount = msg.data[0];
        filecount = (filecount << 8) | msg.data[1];
        filecount = bd_sock_ntohs(filecount);
        if (filecount == pUpdateinfo->getFileCount())
        {
            pUpdateinfo->setUpdateStage(READY);//转向下一个阶段
        }
        pUpdateinfo->setMsgSendFlag(false);
        break;
    case READY:
        fileindex = msg.data[0];
        fileindex = (fileindex << 8) | msg.data[1];
        fileindex = bd_sock_ntohs(fileindex);
        filesize = msg.data[2];
        filesize = (filesize << 8) | msg.data[3];
        filesize = (filesize << 8) | msg.data[4];
        filesize = (filesize << 8) | msg.data[5];
        filesize = bd_sock_ntohl(filesize);
        crc16 = msg.data[6];
        crc16 = (crc16 << 8) | msg.data[7];
        crc16 = bd_sock_ntohs(crc16);

        DBG(TRACE("%d %d %04x", fileindex, filesize, crc16));
        if (fileindex == pUpdateinfo->getFileIndex() && filesize == pUpdateinfo->getUpdateDataSize() && crc16 == pUpdateinfo->getFileCRCValue())
        {
            pUpdateinfo->setUpdateStage(UPDATING);//转向下一个阶段
        }
        pUpdateinfo->setMsgSendFlag(false);
        break;
    case UPDATING:
        if (msg.data[4])//数据写入成功
        {
            offset = msg.data[0];
            offset = (offset << 8) | msg.data[1];
            offset = (offset << 8) | msg.data[2];
            offset = (offset << 8) | msg.data[3];
            offset = bd_sock_ntohl(offset);

            //DBG(TRACE("UPDATING: offset = %04X size = %04X" , offset, m_nSendDataSize));
            this->ui->progressBarUpdate->setValue(100 * (offset+m_nSendDataSize) / pUpdateinfo->getUpdateDataSize());

            QString text;
            text.sprintf("Transfer complete %d%%", 100 * (offset+m_nSendDataSize) / pUpdateinfo->getUpdateDataSize());
            this->ui->lineEditUpdateinfo->setText(text);

            if ((offset+m_nSendDataSize) >= pUpdateinfo->getUpdateDataSize())
            {
                pUpdateinfo->setUpdateStage(RESULT);//转向下一个阶段
            }
            pUpdateinfo->setMsgSendFlag(false);
        }
        break;
    case RESULT:
        this->m_uUpdateResult = msg.data[0];
        this->m_uUpdateResult = (this->m_uUpdateResult << 8) | msg.data[1];
        this->m_uUpdateResult = bd_sock_ntohs(this->m_uUpdateResult);
        crcval = msg.data[2];
        crcval = (crcval << 8) | msg.data[3];
        crcval = bd_sock_ntohs(crcval);
        DBG(TRACE("RESULT ErrorCode: %04X %04X", this->m_uUpdateResult, crcval));
        DBG(TRACE("Send data CRC: %04X %04X", bd_crc_CRC16(0, pUpdateinfo->getUpDateData(), pUpdateinfo->getUpdateDataSize()), m_uCRC16));
        //if (ERR_NO_ERROR == m_bUpdateResult)
        {
            pUpdateinfo->setFileIndex(pUpdateinfo->getFileIndex()+1);

            if (pUpdateinfo->getFileIndex() >= pUpdateinfo->getFileCount())
            {
                pUpdateinfo->setUpdateStage(LAST_RESULT);//转向下一个阶段
            }
        }
        pUpdateinfo->setMsgSendFlag(false);
        break;
    case LAST_RESULT:
        this->m_uUpdateLastResult = msg.data[0];
        this->m_uUpdateLastResult = (this->m_uUpdateLastResult << 8) | msg.data[1];
        this->m_uUpdateLastResult = bd_sock_ntohs(this->m_uUpdateLastResult);
        //if (ERR_NO_ERROR == m_uUpdateLastResult)
        {
            pUpdateinfo->setUpdateStage(END);//转向下一个阶段
        }
        pUpdateinfo->setMsgSendFlag(false);
        break;
    case END:
        if (msg.data[0] && ERR_NO_ERROR == this->m_uUpdateLastResult)//程序升级成功
        {
            this->ui->lineEditUpdateinfo->setText("Transfer succeed!");
        }
        else            //程序升级失败
        {
            this->ui->lineEditUpdateinfo->setText("Transfer failure!");
        }

        pUpdateinfo->setUpdateStage(STOP);
        pUpdateinfo->setMsgSendFlag(false);

        break;
    default:
        break;
    }

    pUpdateinfo->m_FileMutex.unlock();
}

//协议消息事件回调函数
void UpdateDialog::OnCommMsgEvent(BD_MsgTypeDef &msg)
{
    //DBG(TRACE("%02x %02x %02x %04x %02x %04x %s", msg.uAddr, msg.srcAddr, msg.blockID, msg.functionID, msg.opType, msg.len,msg.data));
    switch (msg.blockID)
    {
    case UPDATE_UNIT:
        OnUpDateUnitMsgEvent(msg);
        break;
    case DEBUG_UNIT:
        OnDebugUnitMsgEvent(msg);
        break;
    default:
        break;
    }
}

//重置对话框
void UpdateDialog::reset()
{
    UpdateDataInfo *pUdateInfo = UpdateDataInfo::getInstance();
    this->ui->progressBarUpdate->setValue(0);
    if (m_strFileName.isEmpty() || m_strFileName == "" || NULL == m_pFileInfo)
    {
        this->ui->pushButtonUpdate->setEnabled(false);
        QString text;
        text.sprintf("Size: B\tCRC: ");
        this->ui->lineEditFileInfo->setText(text);
    }
    else
    {
        m_pFileInfo->setFile(m_strFileName);
        if (!checkFile(m_strFileName))
            return;
        if (!readFile())
            return;

        QString text;
        text.sprintf("Size: %dB\tCRC: %04X", pUdateInfo->getUpdateDataSize(), pUdateInfo->getFileCRCValue());
        this->ui->lineEditFileInfo->setText(text);
        this->ui->lineEditFileName->setText(m_strFileName);
        this->ui->pushButtonUpdate->setEnabled(true);
    }
}

bool UpdateDialog::readFile(void)
{
    int retVal = 0;
    QByteArray baData;
    retVal = UpdateFileOperator::getInstance()->readFile(m_strFileName, baData);

    if (UpdateFileOperator::EnFileOp_FileNotExist == retVal)
    {
        QMessageBox::warning(this, tr("Update"),
                             tr("文件不存在！"),
                             QMessageBox::Yes);
        return false;
    }
    else if (UpdateFileOperator::EnFileOp_OpenFileError == retVal)
    {
        QMessageBox::warning(this, tr("Update"),
                             tr("文件打开失败！"),
                             QMessageBox::Yes);
        return false;
    }
    else  if (UpdateFileOperator::EnFileOp_FileDataError == retVal)
    {
        QMessageBox::warning(this, tr("Update"),
                             tr("文件数据格式错误！"),
                             QMessageBox::Yes);
        return false;
    }
    else if (UpdateFileOperator::EnFileOp_FileFormatError == retVal)
    {
        QMessageBox::warning(this, tr("Update"),
                             tr("文件类型错误！"),
                             QMessageBox::Yes);
        return false;
    }
    else
    {
        UpdateDataInfo::getInstance()->setUpDateData(baData);
        return true;
    }
    return false;
}

//检查文件是否存在
bool UpdateDialog::checkFile(QString &filename)
{
    if (NULL == m_pFileInfo)
    {
        m_pFileInfo = new QFileInfo;
    }
    m_pFileInfo->setFile(filename);
    if (!m_pFileInfo->exists())
    {
        return false;
    }

    m_strFileName = filename;
    UpdateDataInfo::getInstance()->setFileSize(m_pFileInfo->size());

    return true;
}

//设置升级结点地址
void UpdateDialog::setUpdateNodeAddr(quint16 addr)
{
    UpdateDataInfo::getInstance()->setUpdateNodeAddr(addr);
}

//窗口关闭事件响应函数
void UpdateDialog::closeEvent( QCloseEvent *event)
{
    UpdateDataInfo *pUpdateInfo = UpdateDataInfo::getInstance();
    if (pUpdateInfo->getUpdateStage() != END)
    {
        pUpdateInfo->setUpdateStage(END);
        this->m_uUpdateResult = ERR_UNKNOW_ERROR; //程序升级结果
        this->m_uUpdateLastResult = ERR_UNKNOW_ERROR; //程序升级结果

        m_txMessage.uAddr = pUpdateInfo->getUpdateNodeAddr();
        m_txMessage.blockID = UPDATE_UNIT;
        m_txMessage.functionID = END;
        m_txMessage.opType = OP_SETGET;

        m_txMessage.len = 1;
        m_txMessage.data[0] = false;

        BDComm::getInstance()->transmit(&m_txMessage);
    }

    QObject::disconnect(BDComm::getInstance(), SIGNAL(CommMessageAchieve(BD_MsgTypeDef&)), this, SLOT(OnCommMsgEvent(BD_MsgTypeDef&)));

    QObject::disconnect(m_hUpdateThread, SIGNAL(SendUpdate_start()), this, SLOT(OnUpdate_start()));
    QObject::disconnect(m_hUpdateThread, SIGNAL(SendUpdate_ready()), this, SLOT(OnUpdate_ready()));
    QObject::disconnect(m_hUpdateThread, SIGNAL(SendUpdate_updating()), this, SLOT(OnUpdate_updating()));
    QObject::disconnect(m_hUpdateThread, SIGNAL(SendUpdate_result()), this, SLOT(OnUpdate_result()));
    QObject::disconnect(m_hUpdateThread, SIGNAL(SendUpdate_lastresult()), this, SLOT(OnUpdate_lastresult()));
    QObject::disconnect(m_hUpdateThread, SIGNAL(SendUpdate_end()), this, SLOT(OnUpdate_end()));

    pUpdateInfo->setUpdateStage(STOP);

    m_hUpdateThread->stop();

    QDialog::closeEvent(event);
}

//窗口显示事件响应函数
void UpdateDialog::showEvent( QShowEvent *event)
{
    this->reset();
    UpdateDataInfo::getInstance()->setUpdateStage(STOP);

    QObject::connect(BDComm::getInstance(), SIGNAL(CommMessageAchieve(BD_MsgTypeDef&)), this, SLOT(OnCommMsgEvent(BD_MsgTypeDef&)));

    QObject::connect(m_hUpdateThread, SIGNAL(SendUpdate_start()), this, SLOT(OnUpdate_start()));
    QObject::connect(m_hUpdateThread, SIGNAL(SendUpdate_ready()), this, SLOT(OnUpdate_ready()));
    QObject::connect(m_hUpdateThread, SIGNAL(SendUpdate_updating()), this, SLOT(OnUpdate_updating()));
    QObject::connect(m_hUpdateThread, SIGNAL(SendUpdate_result()), this, SLOT(OnUpdate_result()));
    QObject::connect(m_hUpdateThread, SIGNAL(SendUpdate_lastresult()), this, SLOT(OnUpdate_lastresult()));
    QObject::connect(m_hUpdateThread, SIGNAL(SendUpdate_end()), this, SLOT(OnUpdate_end()));

    QString text = "";
    text.sprintf("Transfer complete %d%%", 0);
    this->ui->lineEditUpdateinfo->setText(text);

    if (m_hUpdateThread != NULL)
    {
        m_hUpdateThread->setIsThreadRun();
        m_hUpdateThread->start();
    }

    QDialog::showEvent(event);
}

void UpdateDialog::on_pushButtonOpenFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                               tr("MCU Update"), "",
                               tr("update files (*.bin *.hex)"));
    if (filename.isEmpty())
        return;
    if (!checkFile(filename))
        return;
    if (!readFile())
        return;

    QString text;
    text.sprintf("Size: %dB\tCRC: %04X", UpdateDataInfo::getInstance()->getUpdateDataSize(), UpdateDataInfo::getInstance()->getFileCRCValue());
    this->ui->lineEditFileInfo->setText(text);
    this->ui->lineEditFileName->setText(filename);
    this->ui->pushButtonUpdate->setEnabled(true);
}


void UpdateDialog::on_pushButtonUpdate_clicked()
{
    UpdateDataInfo::getInstance()->setMsgSendFlag(true);
    UpdateDataInfo::getInstance()->setUpdateStage(STOP);
    UpdateDataInfo::getInstance()->setFileIndex(0);
    UpdateDataInfo::getInstance()->setUpdateStage(START);
    UpdateDataInfo::getInstance()->setOffset(0);
    UpdateDataInfo::getInstance()->setMsgSendFlag(false);

    this->m_uUpdateResult = ERR_UNKNOW_ERROR; //程序升级结果
    this->m_uUpdateLastResult = ERR_UNKNOW_ERROR; //程序升级结果

m_uCRC16 = 0;
}


void UpdateDialog::OnUpdate_start()
{
    qDebug("OnUpdate_start");

    UpdateDataInfo *pUpdateInfo = UpdateDataInfo::getInstance();

    this->m_txMessage.uAddr = pUpdateInfo->getUpdateNodeAddr();
    this->m_txMessage.blockID = UPDATE_UNIT;
    this->m_txMessage.functionID = START;
    this->m_txMessage.opType = OP_SETGET;

    this->m_txMessage.len = 2;
    quint16 filecount = bd_sock_htons(pUpdateInfo->getFileCount());
    this->m_txMessage.data[0] = (UCHAR)(filecount >> 8);
    this->m_txMessage.data[1] = (UCHAR)(filecount);

    BDComm::getInstance()->transmit(&this->m_txMessage);
}

void UpdateDialog::OnUpdate_ready()
{
    qDebug("OnUpdate_ready");

    UpdateDataInfo *pUpdateInfo = UpdateDataInfo::getInstance();

    this->m_txMessage.uAddr = pUpdateInfo->getUpdateNodeAddr();;
    this->m_txMessage.blockID = UPDATE_UNIT;
    this->m_txMessage.functionID = READY;
    this->m_txMessage.opType = OP_SETGET;

    this->m_txMessage.len = 8;
    quint16 fileindex = bd_sock_htons(pUpdateInfo->getFileIndex()); //文件序号
    this->m_txMessage.data[0] = (UCHAR)(fileindex >> 8);
    this->m_txMessage.data[1] = (UCHAR)(fileindex);

    quint32 filesize = bd_sock_htonl(pUpdateInfo->getUpdateDataSize()); //文件大小
    this->m_txMessage.data[2] = (UCHAR)(filesize >> 24);
    this->m_txMessage.data[3] = (UCHAR)(filesize >> 16);
    this->m_txMessage.data[4] = (UCHAR)(filesize >> 8);
    this->m_txMessage.data[5] = (UCHAR)(filesize);

    quint16 crc16 = bd_sock_htons((quint16)pUpdateInfo->getFileCRCValue()); //文件CRC16
    this->m_txMessage.data[6] = (UCHAR)(crc16 >> 8);
    this->m_txMessage.data[7] = (UCHAR)(crc16);
    //qDebug("%04x %04x", m_uFileCRCValue, crc16);
    BDComm::getInstance()->transmit(&this->m_txMessage);
    //this->m_CommMutex.unlock();
}

void UpdateDialog::OnUpdate_updating()
{
    UpdateDataInfo *pUpdateInfo = UpdateDataInfo::getInstance();
    quint32 uOffset = pUpdateInfo->getOffset();
    quint32 uDataSize = pUpdateInfo->getUpdateDataSize();
    quint8 *pUpdateData = pUpdateInfo->getUpDateData();

    if (NULL == pUpdateData)
        return;

    //qDebug("OnUpdate_updating: offset = %04X" , uOffset);

    this->m_txMessage.uAddr = pUpdateInfo->getUpdateNodeAddr();;
    this->m_txMessage.blockID = UPDATE_UNIT;
    this->m_txMessage.functionID = UPDATING;
    this->m_txMessage.opType = OP_SETGET;

    quint32 offset = 0;
    offset = bd_sock_htonl(uOffset);
    this->m_txMessage.data[0] = (UCHAR)(offset >> 24);
    this->m_txMessage.data[1] = (UCHAR)(offset >> 16);
    this->m_txMessage.data[2] = (UCHAR)(offset >> 8);
    this->m_txMessage.data[3] = (UCHAR)(offset);

    if (uOffset + UpdateDialog::UPDATE_DATA_MAX <= uDataSize)
    {
        m_nSendDataSize = UpdateDialog::UPDATE_DATA_MAX;
    }
    else
    {
        m_nSendDataSize = uDataSize - uOffset;
    }

    if (m_nSendDataSize > 0)
    {
        memcpy(&this->m_txMessage.data[4], &pUpdateData[uOffset], m_nSendDataSize);

        m_uCRC16 = bd_crc_CRC16(m_uCRC16, &m_txMessage.data[4], m_nSendDataSize);

        this->m_txMessage.len = m_nSendDataSize + 4;
        BDComm::getInstance()->transmit(&this->m_txMessage);

        uOffset += m_nSendDataSize;
        pUpdateInfo->setOffset(uOffset);
    }
}

void UpdateDialog::OnUpdate_result()
{
    qDebug("OnUpdate_result");

    UpdateDataInfo *pUpdateInfo = UpdateDataInfo::getInstance();

    this->m_txMessage.uAddr = pUpdateInfo->getUpdateNodeAddr();;
    this->m_txMessage.blockID = UPDATE_UNIT;
    this->m_txMessage.functionID = RESULT;
    this->m_txMessage.opType = OP_SETGET;

    this->m_txMessage.len = 0;

    BDComm::getInstance()->transmit(&this->m_txMessage);
}

void UpdateDialog::OnUpdate_lastresult()
{
    qDebug("OnUpdate_lastresult");

    UpdateDataInfo *pUpdateInfo = UpdateDataInfo::getInstance();

    this->m_txMessage.uAddr = pUpdateInfo->getUpdateNodeAddr();;
    this->m_txMessage.blockID = UPDATE_UNIT;
    this->m_txMessage.functionID = LAST_RESULT;
    this->m_txMessage.opType = OP_SETGET;

    this->m_txMessage.len = 0;

    BDComm::getInstance()->transmit(&this->m_txMessage);
}

void UpdateDialog::OnUpdate_end()
{
    qDebug("OnUpdate_end");

    UpdateDataInfo *pUpdateInfo = UpdateDataInfo::getInstance();

    this->m_txMessage.uAddr = pUpdateInfo->getUpdateNodeAddr();;
    this->m_txMessage.blockID = UPDATE_UNIT;
    this->m_txMessage.functionID = END;
    this->m_txMessage.opType = OP_SETGET;

    this->m_txMessage.len = 1;
    //this->m_txMessage.data[0] = (ERR_NO_ERROR == this->m_bUpdateLastResult ? true : false);
    this->m_txMessage.data[0] = TRUE;

    BDComm::getInstance()->transmit(&this->m_txMessage);
}


