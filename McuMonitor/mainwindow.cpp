#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QObject>
#include <QCloseEvent>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QList>
#include <QSerialPortInfo>
#include <QDebug>

#include "unit.h"
#include "function.h"
#include "optype.h"
#include "errorcode.h"

#include "commhandlethread.h"
#include "update/updatedialog.h"
#include "hextobin/hextobindialog.h"
#include "socket/osal_socket.h"

#define DEFAULT_COMPORT "COM1"
#define DEFAULT_BAUDRATE 115200ul

#define MAX_MESSAGE_ROW 100

//消息类型定义
enum EN_ColumnTypeDef
{
    SOURCE_ADDR = 0,
    UNIT_ID,
    FUNCTION_ID,
    OPERTOR_ID,
    USER_DATA,
    ORIGINAL_MSG,
    COLUMN_SIZE
};

enum
{
    TIMER_TIMING_TIME = 1000,//定时器定时时间(ms)
    TIMER_NODE_ONLINE_TIME = 10 * 1000, //结点在线时间(ms)
    TIMER_NODE_ONLINE_COUNT_MAX = TIMER_NODE_ONLINE_TIME / TIMER_TIMING_TIME //结点在线心跳计数最大值
};

typedef enum _ESP8266_CONNST_STATUS
{
    ESP8266_CONNETED = 0,		/*!< 建立连接 */
    ESP8266_CONNET_LOST,		/*!< 失去连接 */
    ESP8266_CONNET_GETIP,		/*!< 获得IP */
    ESP8266_CONNET_LOST_WIFI	/*!< 物理掉线 */
}Esp8266ConnetStatus;

//静态成员初始化
QMutex MainWindow::m_Mutex(QMutex::Recursive);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_iniProFileOperator(".\\McuMonitor.ini"),
    m_uCurAccessNodeAddr(0),
    m_u16CommHandleCount(0)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);//禁止窗口最大化

    memset(&m_CommMessage, 1, sizeof(BD_MsgTypeDef));
    m_hCommPortInstance = new BDSerialPort();
    m_hCommHandle = BDComm::getInstance();
    //m_hCommHandle = new BDComm();

    m_pTimer = NULL;
    m_pUpdateDialog = NULL;
    m_pHexToBinDialog = NULL;
    this->setWindowIcon(QIcon(":/icon.ico"));

    ui->lineEditNodeAddr->setText(tr("0"));
    ui->lineEditNewNodeAddr->setText(tr("0"));
    //初始化状态栏
    initialStatusBar();

    //串口参数初始化
    initialComm();

    //初始化消息显示框
    initialMessageTable();

    //初始化定时器
    m_pTimer = NULL;
    //m_pTimer = new QTimer(this);
    //connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()));
    //m_pTimer->start(TIMER_TIMING_TIME); //xxx ms 定时
    m_pCommHandleThread = NULL;
    m_pCommHandleThread = new CommHandleThread();
    //QObject::connect(m_pCommHandleThread, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()));
    m_pCommHandleThread->start();

    //初始化信号和槽
    QObject::connect(BDComm::getInstance(), SIGNAL(CommMessageAchieve(BD_MsgTypeDef&)), this, SLOT(OnCommMsgEvent(BD_MsgTypeDef&)));
    QObject::connect(this->ui->quitAction, SIGNAL(triggered()), this, SLOT(OnActionQuit()));//退出
    QObject::connect(this->ui->updateAction, SIGNAL(triggered()), this, SLOT(OnActionUpdate()));//程序升级
    QObject::connect(this->ui->hexToBinAction, SIGNAL(triggered()), this, SLOT(OnActionHexToBin()));//Hex转Bin文件
    QObject::connect(this->ui->aboutAction, SIGNAL(triggered()), this, SLOT(about()));//关于
}

MainWindow::~MainWindow()
{
    saveComPortSetting();

    delete ui;
    m_hCommHandle = NULL;
    delete m_hCommPortInstance;
    delete m_pUpdateDialog;
    delete m_pHexToBinDialog;

    if (m_pTimer != NULL)
    {
        m_pTimer->stop();
        delete m_pTimer;
    }
    if (m_pCommHandleThread != NULL)
    {
        m_pCommHandleThread->stop();
        delete m_pCommHandleThread;
    }
}

//初始化状态栏
void MainWindow::initialStatusBar(void)
{

}

//初始化串口参数，通过读取INI文件设置串口参数
void MainWindow::readComPortSetting(void)
{
    m_strPortName = m_iniProFileOperator.getProfileString("COM", "Port");
    m_PortSettings.BaudRate = m_iniProFileOperator.getProfileInt("BAUDRATE", "Baudrate");
    m_PortSettings.DataBits = (DataBitsType)m_iniProFileOperator.getProfileInt("DATABIT", "DataBit");
    m_PortSettings.Parity = (ParityType)m_iniProFileOperator.getProfileInt("PARITY", "Parity");
    m_PortSettings.StopBits = (StopBitsType)m_iniProFileOperator.getProfileInt("STOPBIT", "StopBit");
    m_PortSettings.FlowControl = QSerialPort::NoFlowControl;
    m_PortSettings.Timeout = 0;

    DBG(TRACE("readComPortSetting: "));
    DBG(TRACE() << m_strPortName << m_PortSettings.BaudRate << m_PortSettings.DataBits << m_PortSettings.Parity << m_PortSettings.StopBits);
    if ("" == m_strPortName)//串口号合法性判断
    {
        m_strPortName = DEFAULT_COMPORT;
    }
    if (0 == m_PortSettings.BaudRate)//波特率合法性判断
    {
        m_PortSettings.BaudRate = DEFAULT_BAUDRATE;
    }
    if (!isDataBit(m_PortSettings.DataBits))//数据位
    {
        m_PortSettings.DataBits = QSerialPort::Data8;
    }
    if (!isParity(m_PortSettings.Parity))//校验位
    {
        m_PortSettings.Parity = QSerialPort::NoParity;
    }
    if (!isStopBit(m_PortSettings.StopBits))//停止位
    {
        m_PortSettings.StopBits = QSerialPort::OneStop;
    }
}

//保存串口参数
void MainWindow::saveComPortSetting(void)
{
    DBG(TRACE("saveComPortSetting: "));
    DBG(TRACE() << m_strPortName << m_PortSettings.BaudRate << m_PortSettings.DataBits << m_PortSettings.Parity << m_PortSettings.StopBits);

    m_iniProFileOperator.setProfileString("COM", "Port", m_strPortName);
    m_iniProFileOperator.setProfileInt("BAUDRATE", "Baudrate", m_PortSettings.BaudRate);
    m_iniProFileOperator.setProfileInt("DATABIT", "DataBit", m_PortSettings.DataBits);
    m_iniProFileOperator.setProfileInt("PARITY", "Parity", m_PortSettings.Parity);
    m_iniProFileOperator.setProfileInt("STOPBIT", "StopBit", m_PortSettings.StopBits);
}

//显示消息
void MainWindow::showMessage(BD_MsgTypeDef &msg)
{
    //TRACE("showMessage: ");
    char buffer[MAX_MSG_DATA_SIZE] = {0};
    QString strData;
    int curRow = 0;
    //得到当前选中的单元格
    int row = this->ui->tableWidgetMessage->currentIndex().row();   //行
    int col = this->ui->tableWidgetMessage->currentIndex().column();//列
    this->ui->tableWidgetMessage->insertRow(curRow);//插入新行
    if (this->ui->tableWidgetMessage->rowCount() >= MAX_MESSAGE_ROW)//当行数超过一定行数时自动删除多出的行
        this->ui->tableWidgetMessage->removeRow(MAX_MESSAGE_ROW);
    //设备地址
    sprintf(buffer, "%02X", msg.uAddr);
    this->ui->tableWidgetMessage->setItem(curRow, SOURCE_ADDR, new QTableWidgetItem(QString(buffer)));
    //功能单元
    sprintf(buffer, "%02X", msg.blockID);
    this->ui->tableWidgetMessage->setItem(curRow, UNIT_ID, new QTableWidgetItem(QString(buffer)));
    //功能码
    sprintf(buffer, "%04X", msg.functionID);
    this->ui->tableWidgetMessage->setItem(curRow, FUNCTION_ID, new QTableWidgetItem(QString(buffer)));
    //操作码
    sprintf(buffer, "%02X", msg.opType);
    this->ui->tableWidgetMessage->setItem(curRow, OPERTOR_ID, new QTableWidgetItem(QString(buffer)));
    //用户数据
    strData.clear();
    for (int i = 0; i < msg.len; ++i)
    {
        sprintf(buffer, "%02X ", msg.data[i]);
        strData.append(buffer);
    }
    this->ui->tableWidgetMessage->setItem(curRow, USER_DATA, new QTableWidgetItem(strData));
    //原始消息
    strData.clear();
    sprintf(buffer, "%02x %02x %04x %02x %04x ", msg.uAddr, msg.blockID, msg.functionID, msg.opType, msg.len);
    strData.append(buffer);
    for (int i = 0; i < msg.len; ++i)
    {
        sprintf(buffer, "%02X ", msg.data[i]);
        strData.append(buffer);
    }
    strData.append(buffer);
    this->ui->tableWidgetMessage->setItem(curRow, ORIGINAL_MSG, new QTableWidgetItem(strData));
    this->ui->tableWidgetMessage->setCurrentCell(row, col);

    this->ui->tableWidgetMessage->show();
}

//协议消息事件回调函数
void MainWindow::OnCommMsgEvent(BD_MsgTypeDef &msg)
{
    //DBG(TRACE("%02x %02x %04x %02x %04x %s", msg.uAddr, msg.blockID, msg.functionID, msg.opType, msg.len,msg.data));
    m_u16CommHandleCount = 0;
    showMessage(msg);
    switch (msg.blockID)
    {
    case ROUTER_UNIT:
        OnCommRouterMsg(msg);
        break;
    case DEBUG_UNIT:
        OnCommDebugrMsg(msg);
        break;
    default:
        break;
    }
}
\
//获取系统可用串口列表
void MainWindow::initialComm(void)
{
    char buffer[8];
    QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM\\";//串口系统注册表地址

    QSettings *settings = new QSettings(path, QSettings::NativeFormat);//获取串口信息
    //QStringList key = settings->allKeys();
    //int keysize = key.size();
    QStringList strlist ;
    QString strCommValue;

    readComPortSetting();//读取INI配置文件，初始化串口参数

    //Com端口号
    strlist.clear();
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>::iterator it = serialPortInfos.begin();
    for(; it != serialPortInfos.end(); it++)
    {
        strCommValue =it->portName();
        strlist << strCommValue;
    }
    this->ui->comboBoxComPort->addItems(strlist);
    this->ui->comboBoxComPort->setCurrentText(m_strPortName);

    //波特率
    strlist.clear();
    strlist << "1200" << "2400" << "4800" << "9600" << "19200" << "38400" <<
             "56000" << "57600" << "115200" << "128000" << "256000";
    this->ui->comboBoxBaudRate->addItems(strlist);
    //TRACE("%d", m_PortSettings.BaudRate);
    sprintf(buffer, "%ld", m_PortSettings.BaudRate);
    this->ui->comboBoxBaudRate->setCurrentText(buffer);

    delete settings;
}

//初始化消息显示框信息
void MainWindow::initialMessageTable(void)
{
    this->ui->tableWidgetMessage->setColumnCount(COLUMN_SIZE); //设置列数
    //this->ui->tableWidgetMessage->setRowCount(10);
    this->ui->tableWidgetMessage->horizontalHeader()->setDefaultSectionSize(80);//设置列宽
    //this->ui->tableWidgetMessage->horizontalHeader()->setFixedHeight(15); //设置表头的高度
    this->ui->tableWidgetMessage->horizontalHeader()->resizeSection(USER_DATA,150); //设置表头第5列的宽度为150，列号从0开始
    this->ui->tableWidgetMessage->horizontalHeader()->resizeSection(ORIGINAL_MSG,150); //设置表头第6列的宽度为150，列号从0开始
    this->ui->tableWidgetMessage->verticalHeader()->setDefaultSectionSize(22); //设置行高

    QStringList header;
    header << tr("设备地址") << tr("功能单元") << tr("功能码") << tr("操作码") << tr("用户数据") << tr("原始数据");
    this->ui->tableWidgetMessage->setHorizontalHeaderLabels(header);//初始化表头
    this->ui->tableWidgetMessage->setColumnHidden(ORIGINAL_MSG, true);//隐藏原始消息
    this->ui->tableWidgetMessage->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑触发

    this->ui->checkBoxShowMessage->setChecked(false);//不显示原始消息


    QStringList strlist ;
    strlist << "TCP" << "UDP";
    this->ui->comboBoxProtocolType->addItems(strlist);
    this->ui->comboBoxProtocolType->setCurrentText("TCP");

}

//得到功能单元描述
QString MainWindow::getNodeDescription(quint16 uAddr)
{
    if (MASTER_UNIT_ADDR == uAddr)
    {
        return QString(MASTER_UNIT_NODE_STR);
    }
    else if (DEBUG_UNIT_ADDR == uAddr)
    {
        return QString(DEBUG_UNIT_NODE_STR);
    }
    return "";
}

//得到功能描述
QString MainWindow::getFuncDescription(quint16 funcID)
{
    if (UPDATE_UNIT == funcID)
    {
        return QString(UPDATE_UNIT_STR);
    }
    else if (DEBUG_UNIT == funcID)
    {
        return QString(DEBUG_UNIT_STR);
    }
    else if (ROUTER_UNIT == funcID)
    {
        return QString(ROUTER_UNIT_STR);
    }
    return "";

}

bool MainWindow::checkMsgStr(UCHAR* msg, int len)
{
    for (int i=0; i< len; i++)
    {
        if (msg[i] != 0xff)
            return true;
    }
    return false;
}

#include <QRegExp>
/**
 * @brief 检查IP、掩码、网关、DNS是否合法
 * @param ip IP、掩码、网关、DNS串
 * @retval 合法参数返回0，非法参数返回-1
 */
bool MainWindow::CheckIPAddr(QString ip)
{
    const char pattern[] = "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b";
    QRegExp reg(pattern);
    return reg.exactMatch(ip);
}

/**
 * @brief 检查MAC地址是否合法
 * @param mac MAC地址串
 * @retval 合法参数返回0，非法参数返回-1
 */
bool MainWindow::CheckMacAddr(QString mac)
{
    const char pattern[] = "[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){5}";
    QRegExp reg(pattern);

    return reg.exactMatch(mac);
}

void MainWindow::OnClose(void)
{
    if (this->m_hCommHandle != NULL)
        this->m_hCommHandle->deInit();
    if (this->m_hCommPortInstance != NULL)
        this->m_hCommPortInstance->closeSerialPort();
    //qDebug() << "OnClose";
}

//退出
void MainWindow::OnActionQuit(void)
{
    //OnClose();
    this->close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    OnClose();
    event->accept();
}

void MainWindow::on_checkBoxShowMessage_toggled(bool checked)
{
    this->ui->tableWidgetMessage->setColumnHidden(ORIGINAL_MSG, !checked);//显示（FALSE）或隐藏（TRUE）原始消息
}

//程序升级
void MainWindow::OnActionUpdate(void)
{
    if (NULL == m_pUpdateDialog)
    {
        m_pUpdateDialog = new UpdateDialog(this);
    }

    m_pUpdateDialog->setUpdateNodeAddr(m_uCurAccessNodeAddr);
    m_pUpdateDialog->show();
    m_pUpdateDialog->raise();
    m_pUpdateDialog->activateWindow();

}

//Hex转Bin文件
void MainWindow::OnActionHexToBin(void)
{
    if (NULL == m_pHexToBinDialog)
    {
        m_pHexToBinDialog = new HexToBinDialog(this);
    }

    m_pHexToBinDialog->show();
    m_pHexToBinDialog->raise();
    m_pHexToBinDialog->activateWindow();
}

//关于
void MainWindow::about()
{
    QMessageBox::about(this, tr("About McuMonitor"),
            tr("<h2>McuMonitor 1.0</h2>"
               "<p>Copyright &copy; 2016 wsf."
               "<p>McuMonitor is a small application"));
}

//定时器溢出事件响应
void MainWindow::OnTimerTimeout(void)
{
    //DBG(TRACE("Timeout"));
    static quint8 step = 0;
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    switch(step)
    {
    case 0:                 //Get Bios Version
        pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
        pMsg->blockID = DEBUG_UNIT;
        pMsg->functionID = GET_BIOS_VERSION;
        pMsg->opType = OP_GET;
        pMsg->len = 0;

        step = 1;
        break;
    case 1:                 //Get App Version
        pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
        pMsg->blockID = DEBUG_UNIT;
        pMsg->functionID = GET_APP_VERSION;
        pMsg->opType = OP_GET;
        pMsg->len = 0;

        step = 2;
        break;
    case 2:                 //Get Chip ID
        pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
        pMsg->blockID = DEBUG_UNIT;
        pMsg->functionID = GET_CHIP_ID;
        pMsg->opType = OP_GET;
        pMsg->len = 0;

        step = 3;
        break;
    case 3:                 //Wifi Status
        pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
        pMsg->blockID = DEBUG_UNIT;
        pMsg->functionID = NET_WIFI_CONNET;
        pMsg->opType = OP_GET;
        pMsg->len = 0;

        step = 0;
        break;
    default:
        step = 0;
        break;
    }

    BDComm::getInstance()->transmit(pMsg);

    //DBG(TRACE("Timeout End"));
}

//路由单元消息
void MainWindow::OnCommRouterMsg(BD_MsgTypeDef &msg)
{
    QString text;
    switch (msg.functionID)
    {
    case DEV_ADDR:
    {
        UINT16 uAddr = (msg.data[0] << 8) | msg.data[1];
        uAddr = bd_sock_ntohs(uAddr);
        text.sprintf("%d", uAddr);
        ui->lineEditNodeAddr->setText(text);
        m_uCurAccessNodeAddr = uAddr;
    }
        break;
    default:
        break;
    }
}

//调试单元消息
void MainWindow::OnCommDebugrMsg(BD_MsgTypeDef &msg)
{
    //DBG(TRACE("MainWindow::OnCommDebugrMsg: %#X", msg.functionID));
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
        msg.data[msg.len] = '\0';
        text.sprintf("%02X%02X%02X%02X-%02X%02X%02X%02X-%02X%02X%02X%02X",
                     msg.data[0], msg.data[1], msg.data[2], msg.data[3],
                    msg.data[4], msg.data[5], msg.data[6], msg.data[7],
                    msg.data[8], msg.data[9], msg.data[10], msg.data[11]);
        if (text != this->ui->lineEditChipID->text())
            this->ui->lineEditChipID->setText(text);
        break;
    case NET_LOCAL_IP: /*!< 本地IP、网关、掩码、MAC地址 */
    {
        char buf[32] = {0};
        sprintf(buf, "%d.%d.%d.%d", msg.data[0], msg.data[1], msg.data[2], msg.data[3]);
        ui->lineEditLocalIp->setText(tr(buf));
        sprintf(buf, "%d.%d.%d.%d", msg.data[4], msg.data[5], msg.data[6], msg.data[7]);
        ui->lineEditLocalGateway->setText(tr(buf));
        sprintf(buf, "%d.%d.%d.%d", msg.data[8], msg.data[9], msg.data[10], msg.data[11]);
        ui->lineEditLocalMask->setText(tr(buf));
        sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", msg.data[12], msg.data[13], msg.data[14], msg.data[15], msg.data[16], msg.data[17]);
        ui->lineEditLocalMac->setText(tr(buf));
    }
        break;
    case NET_SERVER_IP: /*!< 服务器IP、端口、连接方式 */
    {
        UINT16 iplen = bd_sock_ntohs((msg.data[0] << 8) | msg.data[1]);
        UINT16 port = bd_sock_ntohs((msg.data[iplen+2] << 8) | msg.data[iplen+3]);
        UINT16 protocol = msg.data[iplen+4];
        port = (0xffff == port) ? 0 : port;
        if (protocol != 2)
            protocol = 1;
        char buf[16] = {0};
        msg.data[iplen+2] = '\0';

        if (checkMsgStr(&msg.data[2], iplen))
            ui->lineEditServerIp->setText(tr((const char*)&msg.data[2]));
        else
            ui->lineEditServerIp->setText(tr(""));
        sprintf(buf, "%d", port);
        ui->lineEditServerPort->setText(tr(buf));
        ui->comboBoxProtocolType->setCurrentIndex(protocol-1);
    }
        break;
    case NET_WIFI_PARAM: /*!< Wifi参数（SSID、PWD） */
    {
        UINT16 ussidlen = bd_sock_ntohs((msg.data[0] << 8) | msg.data[1]);
        UINT16 upwdlen = bd_sock_ntohs((msg.data[2+ussidlen] << 8) | msg.data[2+ussidlen+1]);
        msg.data[2+ussidlen] = '\0';
        msg.data[2+ussidlen+2+upwdlen] = '\0';
        if (checkMsgStr(&msg.data[2], ussidlen))
            ui->lineEditSSID->setText(tr((const char*)&msg.data[2]));
        else
            ui->lineEditSSID->setText(tr(""));
        if (checkMsgStr(&msg.data[ussidlen+4], ussidlen))
            ui->lineEditPassword->setText(tr((const char*)&msg.data[2+ussidlen+2]));
        else
            ui->lineEditPassword->setText(tr(""));
    }
        break;
    case LOCAL_TIME: /*!< 本地时间 */
        break;
    case NET_WIFI_CONNET: /*!< Wifi连接 */
    {
        UINT16 uSt = bd_sock_ntohs((msg.data[0] << 8) | msg.data[1]);
        switch(uSt)
        {
        case ESP8266_CONNETED:		/*!< 建立连接 */
            ui->labelWifiStatus->setText(tr("WiFi状态：建立连接"));
            break;
        case ESP8266_CONNET_LOST:		/*!< 失去连接 */
            ui->labelWifiStatus->setText(tr("WiFi状态：失去连接"));
            break;
        case ESP8266_CONNET_GETIP:		/*!< 获得IP */
            ui->labelWifiStatus->setText(tr("WiFi状态：WIFI在线"));
            break;
        case ESP8266_CONNET_LOST_WIFI:	/*!< 物理掉线 */
            ui->labelWifiStatus->setText(tr("WiFi状态：物理掉线"));
            break;
        default:
            ui->labelWifiStatus->setText(tr("WiFi状态：未知"));
            break;
        }
    }
    default:
        break;
    }
}

void MainWindow::on_pushButtonChangeNodeAddr_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    UINT16 uAddr = ui->lineEditNewNodeAddr->text().toUInt();
    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = ROUTER_UNIT;
    pMsg->functionID = DEV_ADDR;
    pMsg->opType = OP_SETGET;
    uAddr = bd_sock_htons(uAddr);
    pMsg->data[0] = (UCHAR)(uAddr >> 8);
    pMsg->data[1] = (UCHAR)(uAddr);
    pMsg->len = 2;
    BDComm::getInstance()->transmit(pMsg);
}


void MainWindow::on_pushButtonConnetWifi_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = DEBUG_UNIT;
    pMsg->functionID = NET_WIFI_CONNET;
    pMsg->opType = OP_SETGET;
    pMsg->data[0] = 1;
    pMsg->len = 1;
    BDComm::getInstance()->transmit(pMsg);
}

void MainWindow::on_pushButtonSetWiFiInfo_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = DEBUG_UNIT;
    pMsg->functionID = NET_WIFI_PARAM;
    pMsg->opType = OP_SETGET;

    UINT16 uSsidLen = strlen(ui->lineEditSSID->text().toStdString().c_str());
    memcpy(&pMsg->data[2], ui->lineEditSSID->text().toStdString().c_str(), uSsidLen);

    UINT16 uPwdLen = strlen(ui->lineEditPassword->text().toStdString().c_str());
    memcpy(&pMsg->data[uSsidLen+4], ui->lineEditPassword->text().toStdString().c_str(), uPwdLen);

    pMsg->len = uSsidLen + uPwdLen +4;

    uPwdLen = bd_sock_htons(uPwdLen);
    pMsg->data[2+uSsidLen] = (UCHAR)(uPwdLen>>8);
    pMsg->data[3+uSsidLen] = (UCHAR)(uPwdLen);

    uSsidLen = bd_sock_htons(uSsidLen);
    pMsg->data[0] = (UCHAR)(uSsidLen>>8);
    pMsg->data[1] = (UCHAR)(uSsidLen);

    BDComm::getInstance()->transmit(pMsg);
}

void MainWindow::on_pushButtonGetWifiInfo_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = DEBUG_UNIT;
    pMsg->functionID = NET_WIFI_PARAM;
    pMsg->opType = OP_GET;
    pMsg->len = 0;
    BDComm::getInstance()->transmit(pMsg);
}

void MainWindow::on_pushButtonConnetToServer_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = DEBUG_UNIT;
    pMsg->functionID = NET_WIFI_CONNET;
    pMsg->opType = OP_SETGET;
    pMsg->data[0] = 2;
    pMsg->len = 1;
    BDComm::getInstance()->transmit(pMsg);
}

void MainWindow::on_pushButtonSetServerInfo_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = DEBUG_UNIT;
    pMsg->functionID = NET_SERVER_IP;
    pMsg->opType = OP_SETGET;

    UINT16 uIpLen = strlen(ui->lineEditServerIp->text().toStdString().c_str());
    memcpy(&pMsg->data[2], ui->lineEditServerIp->text().toStdString().c_str(), uIpLen);

    UINT16 uServerPort = ui->lineEditServerPort->text().toUInt();
    uServerPort = bd_sock_htons(uServerPort);
    pMsg->data[uIpLen+2] = (UCHAR)(uServerPort >> 8);
    pMsg->data[uIpLen+3] = (UCHAR)(uServerPort);

    UINT16 bTcpConnet = ui->comboBoxProtocolType->currentIndex()+1;
    pMsg->data[uIpLen+4] = (UCHAR)(bTcpConnet);

    pMsg->len = uIpLen+5;

    uIpLen = bd_sock_htons(uIpLen);
    pMsg->data[0] = (UCHAR)(uIpLen >> 8);
    pMsg->data[1] = (UCHAR)(uIpLen);

    BDComm::getInstance()->transmit(pMsg);
}

void MainWindow::on_pushButtonGetServerInfo_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = DEBUG_UNIT;
    pMsg->functionID = NET_SERVER_IP;
    pMsg->opType = OP_GET;
    pMsg->len = 0;
    BDComm::getInstance()->transmit(pMsg);
}

void MainWindow::on_pushButtonSetLocalIp_clicked()
{
    if (!CheckIPAddr(ui->lineEditLocalIp->text()))
    {
        QMessageBox::warning(this, tr("错误"), tr("IP地址不合法!"), QMessageBox::Yes);
        return;
    }
    if (!CheckIPAddr(ui->lineEditLocalGateway->text()))
    {
        QMessageBox::warning(this, tr("错误"), tr("网关地址不合法!"), QMessageBox::Yes);
        return;
    }
    if (!CheckIPAddr(ui->lineEditLocalMask->text()))
    {
        QMessageBox::warning(this, tr("错误"), tr("掩码地址不合法!"), QMessageBox::Yes);
        return;
    }
    if (!CheckMacAddr(ui->lineEditLocalMac->text()))
    {
        QMessageBox::warning(this, tr("错误"), tr("MAC地址不合法!"), QMessageBox::Yes);
        return;
    }

    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = DEBUG_UNIT;
    pMsg->functionID = NET_LOCAL_IP;
    pMsg->opType = OP_SETGET;

    QStringList iplist = ui->lineEditLocalIp->text().split(".");
    QStringList gwlist = ui->lineEditLocalGateway->text().split(".");
    QStringList masklist = ui->lineEditLocalMask->text().split(".");
    QStringList maclist = ui->lineEditLocalMac->text().split(":");
    int i = 0;
    for (; (i < 4) && (iplist.size() == 4); i++)
    {
        pMsg->data[i] = iplist.at(i).toUInt();
    }

    for (; (i < 8) && (gwlist.size() == 4); i++)
    {
        pMsg->data[i] = gwlist.at(i-4).toUInt();
    }

    for (; (i < 12) && (masklist.size() == 4); i++)
    {
        pMsg->data[i] = masklist.at(i-8).toUInt();
    }

    bool ok = true;
    for (; (i < 18) && (maclist.size() == 6); i++)
    {
        pMsg->data[i] = maclist.at(i-12).toUInt(&ok, 16);
    }

    pMsg->len = 18;

    pMsg->opType = OP_SETGET;
    BDComm::getInstance()->transmit(pMsg);
}

void MainWindow::on_pushButtonGetLocalIp_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = DEBUG_UNIT;
    pMsg->functionID = NET_LOCAL_IP;
    pMsg->opType = OP_GET;
    pMsg->len = 0;
    BDComm::getInstance()->transmit(pMsg);
}

void MainWindow::on_pushButtonSendTestMsg_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    UINT16 msglen = strlen(ui->lineEditTestMsg->text().toStdString().c_str());
    memcpy(&pMsg->data[0], ui->lineEditTestMsg->text().toStdString().c_str(), msglen);

    pMsg->uAddr = ui->lineEditNodeAddr->text().toUInt();
    pMsg->blockID = DEBUG_UNIT;
    pMsg->functionID = NET_MSG_TEST;
    pMsg->opType = OP_SETGET;
    pMsg->len = msglen;
    BDComm::getInstance()->transmit(pMsg);
}

void MainWindow::on_pushButtonSerial_clicked()
{
    if (m_hCommPortInstance != NULL)
    {
        //DBG(TRACE("on_comboBoxComPort_currentIndexChanged: "));
        if (!m_hCommPortInstance->isOpen())
        {
            QString sCom = ui->comboBoxComPort->currentText();
            uint BaudRate = ui->comboBoxBaudRate->currentText().toLong();
            DBG(TRACE() << "ComPort: " << sCom << "\tBaudRate: "<< BaudRate);
            UINT uErrorCode = m_hCommPortInstance->openSerialPort(sCom, BaudRate, m_PortSettings.DataBits, m_PortSettings.Parity, m_PortSettings.StopBits, m_PortSettings.Timeout);
            if (ERR_NO_ERROR == uErrorCode)
            {
                DBG(TRACE("Open %s Ok!", sCom.toStdString().c_str()));
                m_strPortName = sCom;
                m_PortSettings.BaudRate = BaudRate;
                m_hCommHandle->init(m_hCommPortInstance);
            }
            else
            {
                DBG(TRACE("Open %s Error: %04X!", sCom.toStdString().c_str(), uErrorCode));
                QMessageBox::warning(this, tr("错误"), tr("串口已被占用!"), QMessageBox::Yes);
            }
        }
        else
        {
            m_hCommPortInstance->closeSerialPort();
        }

        if (m_hCommPortInstance->isOpen())
        {
            ui->pushButtonSerial->setText(tr("关闭串口"));
            //m_pCommHandleThread->start();
            QObject::connect(m_pCommHandleThread, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()));
        }
        else
        {
            ui->pushButtonSerial->setText(tr("打开串口"));
            QObject::disconnect(m_pCommHandleThread, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()));
            //m_pCommHandleThread->stop();
            //m_pCommHandleThread->terminate();
            //m_pCommHandleThread->wait();
        }

    }
}

void MainWindow::on_pushButtonGetDevAddr_clicked()
{
    BD_MsgTypeDef *pMsg = BDComm::getInstance()->getTxMessage();

    pMsg->uAddr = 0xffff;
    pMsg->blockID = ROUTER_UNIT;
    pMsg->functionID = DEV_ADDR;
    pMsg->opType = OP_GET;
    pMsg->len = 0;
    BDComm::getInstance()->transmit(pMsg);
}
