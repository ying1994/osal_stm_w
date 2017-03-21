#ifndef BDSERIALPORT_H
#define BDSERIALPORT_H
#include <QSerialPort>

#include <string>
#include "../types.h"


typedef QSerialPort::BaudRate BaudRateType;
typedef QSerialPort::DataBits DataBitsType;
typedef QSerialPort::Parity ParityType;
typedef QSerialPort::StopBits StopBitsType;
typedef QSerialPort::FlowControl FlowControlType;

#define isDataBit(x) (QSerialPort::Data5 == (x) || QSerialPort::Data6 == (x) || QSerialPort::Data7 == (x) || QSerialPort::Data8 == (x))
#define isParity(x) (QSerialPort::NoParity == (x) || QSerialPort::EvenParity == (x) || QSerialPort::OddParity == (x) || QSerialPort::SpaceParity == (x) || QSerialPort::MarkParity == (x))
#define isStopBit(x) (QSerialPort::OneStop == (x) || QSerialPort::OneAndHalfStop == (x) || QSerialPort::TwoStop == (x))
#define isFlowControl(x) (QSerialPort::NoFlowControl == (x) || QSerialPort::HardwareControl == (x) || QSerialPort::SoftwareControl == (x))

/**
 * @brief 串口基本控制信息结构
 */
struct PortSettingsDef
{
    ulong BaudRate;             /*!< 波特率 */
    DataBitsType DataBits;      /*!< 数据位 */
    ParityType Parity;          /*!< 校验位 */
    StopBitsType StopBits;      /*!< 停止位 */
    FlowControlType FlowControl;       /*!< 流控制 */
    ulong Timeout;              /*!< 超时 */
};


/**
 * @brief 串口控制类，实现串口的基本操作及串口资源管理
 */
class BDSerialPort
{
public:

    /**
     * @brief 构造函数
     */
    BDSerialPort();
    /**
     * @brief 析构函数
     */
    virtual ~BDSerialPort();

    /**
     * @brief 打开串口
     * @param none
     * @return bool
     * @retval TRUE 串口打开成功，FALSE 串口打开失败
     */
    virtual int openSerialPort(void);
    /**
     * @brief 打开串口
     * @param name 串口端口号
     * @param baudrate 串口波特率 @ref BaudRateType
     * @param databit 数据位 @ref DataBitsType
     * @param parity 校验位 @ref ParityType
     * @param stopbit 停止位 @ref StopBitsType
     * @param timeout 超时时间
     * @return bool
     * @retval TRUE 串口打开成功，FALSE 串口打开失败
     */
    virtual int openSerialPort(const QString &name, uint baudrate, DataBitsType databit, ParityType parity, StopBitsType stopbit, ulong timeout);

    /**
     * @brief 关闭串口
     * @return void
     */
    virtual void closeSerialPort(void);
    /**
     * @brief 检查串口是否已打开
     * @return 串口已打开返回true, 否则返回false
     */
    virtual bool isOpen(void);

    /**
     * @brief 设置串口端口号
     * @name 串口端口号
     * @return void
     */
    virtual bool setPortName(const QString & name);
    /**
     * @brief 获取当前打开的串口端口号
     * @param none
     * @return QString 当前串口端口号
     * @attention 若串口未打开，返回的端口号是无效的。
     */
    virtual QString getPortName(void);

    /**
     * @brief 设置串口波特率
     * @param baudrate 串口波特率 @ref BaudRateType
     * @return bool
     * @retval TRUE 参数设置成功，FALSE 参数设置失败
     */
    virtual bool setBaudRate(ulong baudrate);
    /**
     * @brief 获取当前串口波特率
     * @param none
     * @return ulong 当前串口波特率 @ref BaudRateType
     * @attention 若串口未打开，返回的波特率是无效的。
     */
    virtual ulong getBaudRate(void) const;

    /**
     * @brief 设置串口数据位
     * @param databit 串口数据位 @ref DataBitsType
     * @return bool
     * @retval TRUE 参数设置成功，FALSE 参数设置失败
     */
    virtual bool setDataBits(DataBitsType databit);
    /**
     * @brief 获取当前串口数据位
     * @param none
     * @return DataBitsType 当前串口数据位 @ref DataBitsType
     * @attention 若串口未打开，返回的数据位是无效的。
     */
    virtual DataBitsType getDataBits(void) const;

    /**
     * @brief 设置串口校验位
     * @param parity 串口校验位 @ref ParityType
     * @return bool
     * @retval TRUE 参数设置成功，FALSE 参数设置失败
     */
    virtual bool setParity(ParityType parity);
    /**
     * @brief 获取当前串口校验位
     * @param none
     * @return ParityType 当前串口校验位 @ref ParityType
     * @attention 若串口未打开，返回的校验位是无效的。
     */
    virtual ParityType getParity(void) const;

    /**
     * @brief 设置串口停止位
     * @param stopbit 串口停止位 @ref StopBitsType
     * @return bool
     * @retval TRUE 参数设置成功，FALSE 参数设置失败
     */
    virtual bool setStopBits(StopBitsType stopbit);
    /**
     * @brief 获取当前串口停止位
     * @param none
     * @return StopBitsType 当前串口停止位 @ref StopBitsType
     * @attention 若串口未打开，返回的停止位是无效的。
     */
    virtual StopBitsType getStopBits(void) const;

    /**
     * @brief 设置串口数据流控制方式
     * @param flow 串口数据流控制方式 @ref FlowType
     * @return bool
     * @retval TRUE 参数设置成功，FALSE 参数设置失败
     */
    virtual bool setFlowControl(FlowControlType flow);
    /**
     * @brief 获取当前串口数据流控制方式
     * @param none
     * @return FlowType 当前串口数据流控制方式 @ref FlowType
     * @attention 若串口未打开，返回的数据流控制方式是无效的。
     */
    virtual FlowControlType getflowControl(void) const;

    /**
     * @brief 设置串口数据传输超时
     * @param timeout 串口数据传输超时时间(ms)
     * @return bool
     * @retval TRUE 参数设置成功，FALSE 参数设置失败
     */
    virtual bool setTimeout(ulong timeout);

    /**
     * @brief 向串口写数据
     * @param pdata 写入数据句柄
     * @param ulen 数据长度
     * @return int
     * @retval 实际写入的数据长度
     */
    virtual int write(const char* pdata, uint len);
    /**
     * @brief 读串口数据
     * @param pdata 数据存储句柄
     * @param len 读取数据长度
     * @return uint
     * @retval 实际读取的数据长度
     */
    virtual int read(char* pdata, uint len);

private:
    /**
     * @brief 构造函数
     * @param name 串口端口号
     * @return void
     */
    BDSerialPort(const QString &name);
    /**
     * @brief 构造函数
     * @param name 串口端口号
     * @param
     * @return void
     */
    BDSerialPort(const QString &name, const PortSettingsDef &settings);

    /**
     * @brief 设置串口DCB
     * @param none
     * @return void
     */
    virtual bool setupDcb(void);

    /**
     * @brief 设置串口数据传输超时
     * @param none
     * @return void
     */
    virtual bool setTimeout(void);

private:
    QString m_strPortName;                    /*!< 串口端口号 */
    PortSettingsDef m_PortSettings;         /*!< 串口控制结构 */
    QSerialPort *m_serialport;
};

#endif // BDSERIALPORT_H
