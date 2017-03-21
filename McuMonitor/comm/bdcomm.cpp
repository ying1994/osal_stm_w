#include "bdcomm.h"
#include "socket/osal_socket.h"


//静态成员初始化
QMutex BDComm::m_CommMutex(QMutex::Recursive);
QMutex BDComm::m_rxBufferMutex(QMutex::Recursive);
QMutex BDComm::m_txFiFoMutex(QMutex::Recursive);

BDComm::BDComm() : QObject(NULL)/*,m_CommMutex(QMutex::Recursive),m_rxBufferMutex(QMutex::Recursive),m_txFiFoMutex(QMutex::Recursive)*/
{
    memset(&m_rxMessage, 0, sizeof(BD_MsgTypeDef));
    memset(&m_txMessage, 0, sizeof(BD_MsgTypeDef));
    m_hSerialPort = NULL;
    //m_hCommRxObser = NULL;

    m_uReadPriter = 0;
    m_uWritePriter = 0;
    memset(m_rxDataBuffer, 0, MAX_MSG_PHY_DATA_SIZE);

#if USE_TX_MSG_FIFO
    m_uFIFO_ReadPriter = 0;
    m_uFIFO_WritePriter = 0;
    memset(m_TX_FIFO_MsgBuffer, 0, MAX_MSG_FIFO_SIZE * BD_COMM_MSG_SISE);
#endif //USE_TX_MSG_FIFO

    m_hCommRxThread = new CommRxThread();
#if USE_TX_MSG_FIFO
    m_hCommTxThread = new CommTxThread();
#endif //USE_TX_MSG_FIFO
    m_hCommProAnsThread = new CommProtocolAnalysisThread();

    QObject::connect(this, SIGNAL(AppendCommMsg()), this, SLOT(OnAppendCommMsg()));
}
/**
 * @brief 获取COMM实例
 * @param none
 * @retval None
 */
BDComm* BDComm::getInstance(void)
{
    static BDComm bdcommInstance;
    return &bdcommInstance;
}

BDComm::~BDComm()
{
    m_CommMutex.lock();
    deInit();
    m_hSerialPort = NULL;
    //m_hCommRxObser = NULL;
    if (m_hCommRxThread != NULL)
    {
        delete m_hCommRxThread;
        m_hCommRxThread = NULL;
    }
#if USE_TX_MSG_FIFO
    if (m_hCommTxThread != NULL)
    {
        delete m_hCommTxThread;
        m_hCommTxThread = NULL;
    }
#endif //USE_TX_MSG_FIFO
    if (m_hCommProAnsThread != NULL)
    {
        delete m_hCommProAnsThread;
        m_hCommProAnsThread = NULL;
    }
    m_CommMutex.unlock();
}
/**
 * @brief COMM初始化
 * @param hSerialPort:  串口实例句柄
 * @retval None
 */
void BDComm::init(BDSerialPort* hSerialPort)
{
    m_CommMutex.lock();
    m_hSerialPort = hSerialPort;

    if (m_hCommRxThread != NULL && !m_hCommRxThread->getIsThreadRun())
    {
        m_hCommRxThread->setThreadRunFlag();
        m_hCommRxThread->start();
    }
#if USE_TX_MSG_FIFO
    if (m_hCommTxThread != NULL && !m_hCommTxThread->getIsThreadRun())
    {
        m_hCommTxThread->setThreadRunFlag();
        m_hCommTxThread->start();
    }
#endif //USE_TX_MSG_FIFO
    if (m_hCommProAnsThread != NULL && !m_hCommProAnsThread->getIsThreadRun())
    {
        m_hCommProAnsThread->setThreadRunFlag();
        m_hCommProAnsThread->start();
    }
    m_CommMutex.unlock();

}

/**
 * @brief COMM资源注销
 * @param None
 * @retval None
 */
void BDComm::deInit(void)
{
    m_CommMutex.lock();
    m_hSerialPort = NULL;
    //m_hCommRxObser = NULL;

    if (m_hCommRxThread != NULL)
    {
        m_hCommRxThread->stop();
        //m_hCommRxThread->wait();
    }
#if USE_TX_MSG_FIFO
    if (m_hCommTxThread != NULL)
    {
        m_hCommTxThread->stop();
        //m_hCommTxThread->wait();
    }
#endif //USE_TX_MSG_FIFO
    if (m_hCommProAnsThread != NULL)
    {
        m_hCommProAnsThread->stop();
        //m_hCommProAnsThread->wait();
    }
    m_CommMutex.unlock();
}

/**
 * @brief 获取COMM协议消息句柄
 * @param none
 * @retval 可用的COMM协议消息句柄
 */
BD_MsgTypeDef* BDComm::getTxMessage(void)
{
    return &m_txMessage;
}

/**
 * @brief COMM消息发送
 * @param pMsg: 数据帧内容
 * @param len: 数据帧大小
 * @retval  发送成功返回TRUE，失败返回FALSE
 */
BOOL BDComm::transmit(BD_MsgTypeDef* pMsg)
{
    if ((m_hSerialPort != NULL) && (m_hSerialPort->isOpen()))
    {
        //DBG(TRACE("BDComm::transmit: %02x %02x %04x %02x %04x %s", pMsg->uAddr, pMsg->blockID, pMsg->functionID, pMsg->opType, pMsg->len,pMsg->data));

        m_CommMutex.lock();
        UCHAR txData = 0;
        UINT16 uAddr = 0;
        UINT16 uBlockID = 0;
        UINT16 uDataLen = 0;
        UINT16 uFuncID = 0;

        txData = PHY_DATA_FLAG1;				//起始标志1
        m_hSerialPort->write((const char*)&txData, 1);

        txData = PHY_DATA_FLAG2;				//起始标志2
        m_hSerialPort->write((const char*)&txData, 1);

        uAddr = bd_sock_htons(pMsg->uAddr);
        txData = (UCHAR)(uAddr >> 8);					//地址高8位
        m_hSerialPort->write((const char*)&txData, 1);
        txData = (UCHAR)(uAddr);					//地址低8位
        m_hSerialPort->write((const char*)&txData, 1);

        uBlockID = bd_sock_htons(pMsg->blockID);
        txData = (UCHAR)(uBlockID >> 8);                 //功能单元高8位
        m_hSerialPort->write((const char*)&txData, 1);
        txData = (UCHAR)(uBlockID);                 //功能单元低8位
        m_hSerialPort->write((const char*)&txData, 1);

        uFuncID = bd_sock_htons(pMsg->functionID);
        txData = (UCHAR)(uFuncID >> 8);//功能ID高8位
        m_hSerialPort->write((const char*)&txData, 1);
        txData = (UCHAR)(uFuncID);		//功能ID低8位
        m_hSerialPort->write((const char*)&txData, 1);

        txData = pMsg->opType;					//操作码
        m_hSerialPort->write((const char*)&txData, 1);

        uDataLen = getCheckLenght(pMsg->len);//数据长度
        uDataLen = bd_sock_htons(uDataLen);
        txData = (UCHAR)(uDataLen >> 8);		//高8位
        m_hSerialPort->write((const char*)&txData, 1);
        txData = (UCHAR)uDataLen;				//低8位
        m_hSerialPort->write((const char*)&txData, 1);

        m_hSerialPort->write((const char*)pMsg->data, pMsg->len);	//用户数据

        txData = getCheckValue(pMsg);			//数据校验
        m_hSerialPort->write((const char*)&txData, 1);

        m_CommMutex.unlock();
        return TRUE;
    }

    return FALSE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief 从FIFO中移除消息
 * @param pMsg 移除的消息句柄
 * @return FIFO移除操作结果
 * @retval 读取成功返回TRUE，失败返回FALSE
 */
BOOL BDComm::removeMsgFromFIFO(BD_MsgTypeDef* pMsg)
{
    m_txFiFoMutex.lock();
    BOOL bRetVal = FALSE;
    for (UINT16 i = m_uFIFO_ReadPriter; i != m_uFIFO_WritePriter; i = (i + 1) % MAX_MSG_FIFO_SIZE)
    {
        if (pMsg->uAddr == m_TX_FIFO_MsgBuffer[i].uAddr ||		//地址
            pMsg->blockID == m_TX_FIFO_MsgBuffer[i].blockID ||       //功能单元
            pMsg->functionID == m_TX_FIFO_MsgBuffer[i].functionID)	//功能ID
        {
            for (UINT16 j = i; j != m_uFIFO_WritePriter; j = (j + 1) % MAX_MSG_FIFO_SIZE)
            {
                memcpy(&m_TX_FIFO_MsgBuffer[j], &m_TX_FIFO_MsgBuffer[(j + 1) % MAX_MSG_FIFO_SIZE], sizeof(BD_MsgTypeDef));
            }
            bRetVal = TRUE;
            break;
        }
    }
    m_txFiFoMutex.unlock();

    return bRetVal;
}

/**
 * @brief 读发送数据FIFO
 * @param pMsg 存储发送的消息句柄
 * @return 读FIFO操作结果
 * @retval 读取成功返回TRUE，失败返回FALSE
 */
BOOL BDComm::readTxFIFO(BD_MsgTypeDef* pMsg)
{
    m_txFiFoMutex.lock();
    UINT16 index = 0;
    if (NULL == pMsg)		//参数非法
    {
        m_txFiFoMutex.unlock();
        return FALSE;
    }
    if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFO为空
    {
        m_txFiFoMutex.unlock();
        return FALSE;
    }

    index = m_uFIFO_ReadPriter;
    memcpy(pMsg, &m_TX_FIFO_MsgBuffer[index], sizeof(BD_MsgTypeDef));

    ++m_uFIFO_ReadPriter;
    m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
    m_txFiFoMutex.unlock();

    return TRUE;
}

/**
 * @brief 写发送数据FIFO
 * @param pMsg 发送消息句柄
 * @return 写FIFO操作结果
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL BDComm::writeTxFIFO(BD_MsgTypeDef* pMsg)
{
    m_txFiFoMutex.lock();
    if (NULL == pMsg)
    {
        m_txFiFoMutex.unlock();
        return FALSE;
    }
    UINT16 index = m_uFIFO_WritePriter;
    memcpy(&m_TX_FIFO_MsgBuffer[index], pMsg, sizeof(BD_MsgTypeDef));

    ++m_uFIFO_WritePriter;
    m_uFIFO_WritePriter = m_uFIFO_WritePriter % MAX_MSG_FIFO_SIZE;

    if (m_uFIFO_WritePriter == m_uFIFO_ReadPriter)
    {
        ++m_uFIFO_ReadPriter;
        m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
    }
    m_txFiFoMutex.unlock();
    return TRUE;
}

/**
 * @brief 通过FIFO的方式发送COMM消息
 * @param pMsg: 数据帧内容
 * @param len: 数据帧大小
 * @retval  发送成功返回TRUE，失败返回FALSE
 */
BOOL BDComm::transmitByFIFO(BD_MsgTypeDef* pMsg)
{
    return writeTxFIFO(pMsg);
}

#endif

/**
 * @brief 数据长度校验
 * @param nLen 数据长度值
 * @return 返回计算的校验结果
 */
UINT16 BDComm::getCheckLenght(UINT16 uLen)
{
    //m_CommMutex.lock();
    UINT16 uCheckLen = ~uLen;
    uCheckLen <<= 12; //高4位为长度校验值，将低4位左移到高4位
    uCheckLen |= (uLen & 0x0fff); //合成完整的数据长度
    //m_CommMutex.unlock();
    return uCheckLen; //合成完整的数据长度
}

/**
 * @brief 计算数据帧校验值(异或检验)
 * @param pMsg 接收到的消息句柄
 * @return 返回计算校验值
 */
UCHAR BDComm::getCheckValue(BD_MsgTypeDef* pMsg)
{
    m_CommMutex.lock();
    UCHAR checkValue = 0;
    UINT16 checkLen = 0;
    UINT16 i = 0;
    UINT16 uAddr = bd_sock_htons(pMsg->uAddr);
    UINT16 blockID = bd_sock_htons(pMsg->blockID);
    UINT16 functionID = bd_sock_htons(pMsg->functionID);

    checkValue = (UCHAR)(uAddr >> 8);		//地址高8位
    checkValue ^= (UCHAR)(uAddr);				//地址低8位
    checkValue ^= (UCHAR)(blockID >> 8);		//功能单元高8位
    checkValue ^= (UCHAR)(blockID);			//功能单元低8位
    checkValue ^= (UCHAR)(functionID >> 8);	//功能ID高8位
    checkValue ^= (UCHAR)(functionID);		//功能ID低8位
    checkValue ^= pMsg->opType;						//操作码

    checkLen = bd_sock_htons(getCheckLenght(pMsg->len));			//数据长度校验
    checkValue ^= (UCHAR)(checkLen >> 8);			//数据长度高8位
    checkValue ^= (UCHAR)(checkLen);				//数据长度低8位

    for (i = 0; i < pMsg->len; i++)					//用户数据
    {
        checkValue ^= pMsg->data[i];
    }

    m_CommMutex.unlock();
    return checkValue;
}

void BDComm::setCommMsgEvent(void)
{
    emit AppendCommMsg();
}


void BDComm::OnAppendCommMsg()
{
    emit CommMessageAchieve(m_rxMessage);
}
