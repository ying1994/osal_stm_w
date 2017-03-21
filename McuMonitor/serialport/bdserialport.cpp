#include "bdserialport.h"
#include "../errorcode.h"

/**
 * @brief 构造函数
 */
BDSerialPort::BDSerialPort(): m_strPortName("")
{
    m_PortSettings.BaudRate = QSerialPort::Baud115200;
    m_PortSettings.DataBits = QSerialPort::Data8;
    m_PortSettings.Parity = QSerialPort::NoParity;
    m_PortSettings.StopBits = QSerialPort::OneStop;
    m_PortSettings.FlowControl = QSerialPort::NoFlowControl;
    m_PortSettings.Timeout = 0;

    m_serialport = NULL;
}

BDSerialPort::BDSerialPort(const QString &name): m_strPortName(name)
{
    m_PortSettings.BaudRate = QSerialPort::Baud115200;
    m_PortSettings.DataBits = QSerialPort::Data8;
    m_PortSettings.Parity = QSerialPort::NoParity;
    m_PortSettings.StopBits = QSerialPort::OneStop;
    m_PortSettings.FlowControl = QSerialPort::NoFlowControl;
    m_PortSettings.Timeout = 0;

    m_serialport = NULL;

    openSerialPort();
}

/**
 * @brief 构造函数
 * @param name 串口端口号
 * @param
 * @return void
 */
BDSerialPort::BDSerialPort(const QString &name, const PortSettingsDef &settings): m_strPortName(name)
{
    m_PortSettings.BaudRate = settings.BaudRate;
    m_PortSettings.DataBits = settings.DataBits;
    m_PortSettings.Parity = settings.Parity;
    m_PortSettings.StopBits = settings.StopBits;
    m_PortSettings.FlowControl = settings.FlowControl;
    m_PortSettings.Timeout = settings.Timeout;

    m_serialport = NULL;

    openSerialPort();
}

/**
 * @brief 析构函数
 */
BDSerialPort::~BDSerialPort()
{
    closeSerialPort();
    if (m_serialport != NULL)
        delete m_serialport;
}

/**
 * @brief 设置串口DCB
 * @param none
 * @return void
 */
bool BDSerialPort::setupDcb(void)
{
    return true;
}

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
int BDSerialPort::openSerialPort(const QString &name, uint baudrate, DataBitsType databit, ParityType parity, StopBitsType stopbit, ulong timeout)
{
    m_strPortName = name;
    m_PortSettings.BaudRate = baudrate;
    m_PortSettings.DataBits = databit;
    m_PortSettings.Parity = parity;
    m_PortSettings.StopBits = stopbit;
    m_PortSettings.FlowControl = QSerialPort::NoFlowControl;
    m_PortSettings.Timeout = timeout;

    return openSerialPort();
}

/**
 * @brief 打开串口
 * @param none
 * @return bool
 * @retval TRUE 串口打开成功，FALSE 串口打开失败
 */
int BDSerialPort::openSerialPort(void)
{
    int error = 0;
    if (m_serialport != NULL && m_serialport->isOpen())//串口已经打开
    {
        m_serialport->close();
    }
    if (NULL == m_serialport)
        m_serialport = new QSerialPort();

    m_serialport->setPortName(m_strPortName);
    if (!m_serialport->open(QIODevice::ReadWrite))
    {
        error = m_serialport->error();
        m_serialport->close();
        return error;
    }
    if (!m_serialport->setBaudRate(m_PortSettings.BaudRate))
    {
        error = m_serialport->error();
        m_serialport->close();
        return error;
    }
    if (!m_serialport->setDataBits(m_PortSettings.DataBits))
    {
        error = m_serialport->error();
        m_serialport->close();
        return error;
    }
    if (!m_serialport->setParity(m_PortSettings.Parity))
    {
        error = m_serialport->error();
        m_serialport->close();
        return error;
    }
    if (!m_serialport->setStopBits(m_PortSettings.StopBits))
    {
        error = m_serialport->error();
        m_serialport->close();
        return error;
    }
    if (!m_serialport->setFlowControl(QSerialPort::NoFlowControl))
    {
        error = m_serialport->error();
        m_serialport->close();
        return error;
    }

    return ERR_NO_ERROR;
}

/**
 * @brief 关闭串口
 * @return void
 */
void BDSerialPort::closeSerialPort(void)
{
    if (m_serialport != NULL)
        m_serialport->close();
}

/**
 * @brief 检查串口是否已打开
 * @return 串口已打开返回true, 否则返回false
 */
bool BDSerialPort::isOpen(void)
{
    if (m_serialport != NULL)
        return m_serialport->isOpen();
    return false;
}

/**
 * @brief 设置串口端口号
 * @name 串口端口号
 * @return bool
 * @retval TRUE 参数设置成功，FALSE 参数设置失败
 */
bool BDSerialPort::setPortName(const QString &name)
{
    if (m_strPortName == name)
        return true;
    m_strPortName = name;
    return openSerialPort();
}

/**
 * @brief 获取当前打开的串口端口号
 * @param none
 * @return QString 当前串口端口号
 * @attention 若串口未打开，返回的端口号是无效的。
 */
QString BDSerialPort::getPortName(void)
{
    return QString(m_strPortName);
}


/**
 * @brief 设置串口波特率
 * @param baudrate 串口波特率 @ref BaudRateType
 * @return bool
 * @retval TRUE 参数设置成功，FALSE 参数设置失败
 */
bool BDSerialPort::setBaudRate(ulong baudrate)
{
    if (NULL == m_serialport)
        return false;
    m_PortSettings.BaudRate = baudrate;
    return m_serialport->setBaudRate(m_PortSettings.BaudRate);
}

/**
 * @brief 获取当前串口波特率
 * @param none
 * @return ulong 当前串口波特率 @ref BaudRateType
 * @attention 若串口未打开，返回的波特率是无效的。
 */
ulong BDSerialPort::getBaudRate(void) const
{
    return m_PortSettings.BaudRate;
}

/**
 * @brief 设置串口数据位
 * @param databit 串口数据位 @ref DataBitsType
 * @return bool
 * @retval TRUE 参数设置成功，FALSE 参数设置失败
 */
bool BDSerialPort::setDataBits(DataBitsType databit)
{
    if (NULL == m_serialport)
        return false;
    m_PortSettings.DataBits = databit;
    return m_serialport->setDataBits(m_PortSettings.DataBits);
}

/**
 * @brief 获取当前串口数据位
 * @param none
 * @return DataBitsType 当前串口数据位 @ref DataBitsType
 * @attention 若串口未打开，返回的数据位是无效的。
 */
DataBitsType BDSerialPort::getDataBits(void) const
{
    return m_PortSettings.DataBits;
}

/**
 * @brief 设置串口校验位
 * @param parity 串口校验位 @ref ParityType
 * @return bool
 * @retval TRUE 参数设置成功，FALSE 参数设置失败
 */
bool BDSerialPort::setParity(ParityType parity)
{
    if (NULL == m_serialport)
        return false;
    m_PortSettings.Parity = parity;
    return m_serialport->setParity(m_PortSettings.Parity);
}

/**
 * @brief 获取当前串口校验位
 * @param none
 * @return ParityType 当前串口校验位 @ref ParityType
 * @attention 若串口未打开，返回的校验位是无效的。
 */
ParityType BDSerialPort::getParity(void) const
{
    return m_PortSettings.Parity;
}

/**
 * @brief 设置串口停止位
 * @param stopbit 串口停止位 @ref StopBitsType
 * @return bool
 * @retval TRUE 参数设置成功，FALSE 参数设置失败
 */
bool BDSerialPort::setStopBits(StopBitsType stopbit)
{
    if (NULL == m_serialport)
        return false;
    m_PortSettings.StopBits = stopbit;
   return  m_serialport->setStopBits(m_PortSettings.StopBits);
}

/**
 * @brief 获取当前串口停止位
 * @param none
 * @return StopBitsType 当前串口停止位 @ref StopBitsType
 * @attention 若串口未打开，返回的停止位是无效的。
 */
StopBitsType BDSerialPort::getStopBits(void) const
{
    return m_PortSettings.StopBits;
}

/**
 * @brief 设置串口数据流控制方式
 * @param flow 串口数据流控制方式 @ref FlowType
 * @return bool
 * @retval TRUE 参数设置成功，FALSE 参数设置失败
 */
bool BDSerialPort::setFlowControl(FlowControlType flow)
{
    if (NULL == m_serialport)
        return false;
    m_PortSettings.FlowControl = flow;
    return m_serialport->setFlowControl(m_PortSettings.FlowControl);
}

/**
 * @brief 获取当前串口数据流控制方式
 * @param none
 * @return FlowType 当前串口数据流控制方式 @ref FlowType
 * @attention 若串口未打开，返回的数据流控制方式是无效的。
 */
FlowControlType BDSerialPort::getflowControl(void) const
{
    return m_PortSettings.FlowControl;
}

/**
 * @brief 设置串口数据传输超时
 * @param timeout 串口数据传输超时时间(ms)
 * @return bool
 * @retval TRUE 参数设置成功，FALSE 参数设置失败
 */
bool BDSerialPort::setTimeout(ulong timeout)
{
    m_PortSettings.Timeout = timeout;
    return setTimeout();
}

/**
 * @brief 设置串口数据传输超时
 * @param none
 * @return void
 */
bool BDSerialPort::setTimeout(void)
{
    return true;
}

/**
 * @brief 向串口写数据
 * @param pdata 写入数据句柄
 * @param ulen 数据长度
 * @return int
 * @retval 实际写入的数据长度
 */
int BDSerialPort::write(const char* pdata, uint len)
{
    if (NULL == m_serialport)
        return 0;
    if (!m_serialport->isOpen())//串口未打开
        return 0;
    int retVal = 0;
    retVal = m_serialport->write(pdata, len);
    return retVal;
}
/**
 * @brief 读串口数据
 * @param pdata 数据存储句柄
 * @param len 读取数据长度
 * @return uint
 * @retval 实际读取的数据长度
 */
int BDSerialPort::read(char* pdata, uint len)
{
    if (NULL == m_serialport)
        return 0;
    if (!m_serialport->isOpen())//串口未打开
        return 0;
    int retVal = 0;
    retVal = m_serialport->read(pdata, len);
    return retVal;
}
