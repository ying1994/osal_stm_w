#ifndef BDCOMM_H
#define BDCOMM_H

#include <QMutex>
#include "../serialport/bdserialport.h"
#include "commrxthread.h"
#include "commtxthread.h"
#include "commprotocolanalysisthread.h"

/** 数据物理帧起始标志 */
#define PHY_DATA_FLAG1 0x55
#define PHY_DATA_FLAG2 0xee

/** 使用发送数据缓冲区开关 */
#define USE_TX_MSG_FIFO 1

/** 数据区的最大数据长度 */
#define MAX_MSG_DATA_SIZE 1024
/** 物理层数据区的最大数据长度 */
#define MAX_MSG_PHY_DATA_SIZE (10*(MAX_MSG_DATA_SIZE+20))

/** 数据发送FIFO大小 */
#define MAX_MSG_FIFO_SIZE 10
/** 消息帧大小 */
#define BD_COMM_MSG_SISE MAX_MSG_DATA_SIZE + 8

/**
 * @brief COMM通道枚举
 */
typedef enum
{
    BD_COMM_CHANNEL0 = 0,	/*!< COMM通道0 */
    BD_COMM_CHANNEL1,		/*!< COMM通道1 */
    BD_COMM_CHANNEL2,		/*!< COMM通道2 */
    BD_COMM_CHANNEL3,		/*!< COMM通道3 */
    BD_COMM_CHANNEL4,		/*!< COMM通道4 */
    BD_COMM_CHANNEL_SIZE,	/*!< COMM通道总通道数 */
}BD_COMM_CHANNEL;

/**
 * @brief BD数据协议的数据帧结构
 */
struct BD_MsgTypeDef
{
     UINT16 uAddr;					/*!< 数据发送的源地址 */
     UINT16 blockID;					/*!< 消息功能单元 */
     UINT16 functionID;				/*!< 消息操作功能ID */
     UCHAR opType;					/*!< 操作状态类型码 */
     UINT16 len;					/*!< 数据长度 */
     UCHAR data[MAX_MSG_DATA_SIZE];	/*!< 用户数据 */

     BD_MsgTypeDef()
     {
         memset(this, 0, sizeof(BD_MsgTypeDef));
     }
     BD_MsgTypeDef(BD_MsgTypeDef& msg)
     {
         this->uAddr = msg.uAddr;
         this->blockID = msg.blockID;
         this->functionID = msg.functionID;
         this->opType = msg.opType;
         this->len = msg.len;
         memcpy(this->data, msg.data, msg.len);
     }
     BD_MsgTypeDef& operator = (BD_MsgTypeDef& msg)
     {
         if (this == &msg)
             return *this;
         this->uAddr = msg.uAddr;
         this->blockID = msg.blockID;
         this->functionID = msg.functionID;
         this->opType = msg.opType;
         this->len = msg.len;
         memcpy(this->data, msg.data, msg.len);
         return *this;
     }

     ~BD_MsgTypeDef(){}
};


/**
 * @brief COMM 回调函数
 * @param uIDE: 帧类型标识符（标准帧或扩展帧）
 * @param uID: 数据帧标识符
 * @param pMsg: 数据帧内容
 * @param size: 数据帧大小
 * @retval None
 */
typedef void (*BD_COMM_RX_BASE_FUNC)(BD_MsgTypeDef* pMsg);

/**
 * @brief COMM操作结构定义
 */
class BDComm : public QObject
{
    Q_OBJECT
public:
    friend class CommRxThread;
#if USE_TX_MSG_FIFO
    friend class CommTxThread;
#endif //USE_TX_MSG_FIFO
    friend class CommProtocolAnalysisThread;

private:
    explicit BDComm();
public:
    virtual ~BDComm();

    /**
     * @brief COMM初始化
     * @param hSerialPort:  串口实例句柄
     * @retval None
     */
    virtual void init(BDSerialPort* hSerialPort);

    /**
     * @brief COMM资源注销
     * @param None
     * @retval None
     */
    virtual void deInit(void);

    /**
     * @brief COMM消息发送
     * @param pMsg: 数据帧内容
     * @param len: 数据帧大小
     * @retval  发送成功返回TRUE，失败返回FALSE
     */
    virtual BOOL transmit(BD_MsgTypeDef* pMsg);

#if USE_TX_MSG_FIFO
    /**
     * @brief 通过FIFO的方式发送COMM消息
     * @param pMsg: 数据帧内容
     * @param len: 数据帧大小
     * @retval  发送成功返回TRUE，失败返回FALSE
     */
    virtual BOOL transmitByFIFO(BD_MsgTypeDef* pMsg);
#endif

public:
    /**
     * @brief 获取COMM实例
     * @param none
     * @retval None
     */
    static BDComm* getInstance(void);
    /**
     * @brief 获取COMM协议消息句柄
     * @param none
     * @retval 可用的COMM协议消息句柄
     */
    virtual BD_MsgTypeDef* getTxMessage(void);
    /**
     * @brief 数据长度校验
     * @param nLen 数据长度值
     * @return 返回计算的校验结果
     */
    virtual UINT16 getCheckLenght(UINT16 uLen);

    /**
     * @brief 计算数据帧校验值(异或检验)
     * @param pMsg 接收到的消息句柄
     * @return 返回计算校验值
     */
    virtual UCHAR getCheckValue(BD_MsgTypeDef* pMsg);


    virtual void setCommMsgEvent(void);

signals:
    void AppendCommMsg();                       //接收到协议消息信号，解决线程发送消息问题
    void CommMessageAchieve(BD_MsgTypeDef &msg);//对外发送协议消息信号

private slots:
    void OnAppendCommMsg();//接收到协议消息槽
private:

#if USE_TX_MSG_FIFO

    /**
     * @brief 从FIFO中移除消息
     * @param pMsg 移除的消息句柄
     * @return FIFO移除操作结果
     * @retval 读取成功返回TRUE，失败返回FALSE
     */
    virtual BOOL removeMsgFromFIFO(BD_MsgTypeDef* pMsg);
    /**
     * @brief 读发送数据FIFO
     * @param pMsg 存储发送的消息句柄
     * @return 读FIFO操作结果
     * @retval 读取成功返回TRUE，失败返回FALSE
     */
    virtual BOOL readTxFIFO(BD_MsgTypeDef* pMsg);

    /**
     * @brief 写发送数据FIFO
     * @param pMsg 发送消息句柄
     * @return 写FIFO操作结果
     * @retval 写入成功返回TRUE，失败返回FALSE
     */
    virtual BOOL writeTxFIFO(BD_MsgTypeDef* pMsg);
#endif //USE_TX_MSG_FIFO


private:
    BD_MsgTypeDef m_rxMessage; /*!< 接收消息 */
    BD_MsgTypeDef m_txMessage; /*!< 发送消息 */
    BDSerialPort* m_hSerialPort;/*!< 打开的串口句柄 */
    //BD_COMM_RX_BASE_FUNC m_hCommRxObser;/*!< 数据接收观察者 */

    UCHAR m_rxDataBuffer[MAX_MSG_PHY_DATA_SIZE]; /*!< 数据接收缓冲区 */
    UINT16 m_uReadPriter;/*!< 数据缓冲区读指针 */
    UINT16 m_uWritePriter; /*!< 数据缓冲区写指针 */

#if USE_TX_MSG_FIFO
    BD_MsgTypeDef m_TX_FIFO_MsgBuffer[MAX_MSG_FIFO_SIZE];/*!< 数据接收缓冲区 */
    UINT16 m_uFIFO_ReadPriter;/*!< 数据缓冲区读指针 */
    UINT16 m_uFIFO_WritePriter;/*!< 数据缓冲区写指针 */
#endif //USE_TX_MSG_FIFO

    CommRxThread *m_hCommRxThread;
#if USE_TX_MSG_FIFO
    CommTxThread *m_hCommTxThread;
#endif //USE_TX_MSG_FIFO
    CommProtocolAnalysisThread *m_hCommProAnsThread;

     static QMutex m_CommMutex;
     static QMutex m_rxBufferMutex;
     static QMutex m_txFiFoMutex;

};



#endif // BDCOMM_H
