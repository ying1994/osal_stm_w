#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QString>
#include <QMutex>
#include <QTimer>
#include <QList>
#include "comm/bdcomm.h"
#include "iniprofile.h"

namespace Ui {
class MainWindow;
}

class CommHandleThread;
class UpdateDialog;
class HexToBinDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //初始化状态栏
    void initialStatusBar(void);

    //读取INI配置文件初始化串口参数
    void readComPortSetting(void);
    //将串口参数保存到INI文件中
    void saveComPortSetting(void);

    //获取系统可用串口列表，初始化串口信息
    void initialComm(void);
    //初始化消息显示框
    void initialMessageTable(void);


    void closeEvent( QCloseEvent *event);

private:
    void showMessage(BD_MsgTypeDef &msg);//显示消息
    //路由单元消息
    void OnCommRouterMsg(BD_MsgTypeDef &msg);
    //调试单元消息
    void OnCommDebugrMsg(BD_MsgTypeDef &msg);
    //得到结点功能描述
    QString getNodeDescription(quint16 uAddr);
    //得到功能描述
    QString getFuncDescription(quint16 funcID);

    bool checkMsgStr(UCHAR* msg, int len);
    /**
     * @brief 检查IP、掩码、网关、DNS是否合法
     * @param ip IP、掩码、网关、DNS串
     * @retval 合法参数返回0，非法参数返回-1
     */
    bool CheckIPAddr(QString ip);

    /**
     * @brief 检查MAC地址是否合法
     * @param mac MAC地址串
     * @retval 合法参数返回0，非法参数返回-1
     */
    bool CheckMacAddr(QString mac);

public slots:
    //协议消息事件回调函数
    void OnCommMsgEvent(BD_MsgTypeDef &msg);

private slots:
    void OnClose(void);
    //退出
    void OnActionQuit(void);
    //程序升级
    void OnActionUpdate(void);
    //Hex转Bin文件
    void OnActionHexToBin(void);
    //关于
    void about(void);
    //定时器溢出事件响应
    void OnTimerTimeout(void);

    //Comm端口改变事件
    void on_checkBoxShowMessage_toggled(bool checked);

    void on_pushButtonConnetWifi_clicked();

    void on_pushButtonChangeNodeAddr_clicked();

    void on_pushButtonSetWiFiInfo_clicked();

    void on_pushButtonGetWifiInfo_clicked();

    void on_pushButtonConnetToServer_clicked();

    void on_pushButtonSetServerInfo_clicked();

    void on_pushButtonGetServerInfo_clicked();

    void on_pushButtonSetLocalIp_clicked();

    void on_pushButtonGetLocalIp_clicked();

    void on_pushButtonSendTestMsg_clicked();

    void on_pushButtonSerial_clicked();

    void on_pushButtonGetDevAddr_clicked();

private:
    static QMutex m_Mutex;

    Ui::MainWindow *ui;
    QString m_strPortName;              //串口号
    QStringList m_strListcomboName;        //当前系统可用串口列表
    BD_MsgTypeDef m_CommMessage;        //协议消息实例
    BDComm *m_hCommHandle;              //协议栈实例句柄
    BDSerialPort *m_hCommPortInstance;  //串口实例句柄
    PortSettingsDef m_PortSettings;     // 串口控制结构
    IniProfile m_iniProFileOperator;    //INI文件操作实例
    quint16 m_uCurAccessNodeAddr;           //当前访问的结点地址
    quint16 m_u16CommHandleCount;

    QList<BD_RouterRegisterDef> m_listRegistTable; //当前系统注册表

    QTimer* m_pTimer;                   //定时器
    CommHandleThread *m_pCommHandleThread;

    UpdateDialog *m_pUpdateDialog;
    HexToBinDialog *m_pHexToBinDialog;
};

#endif // MAINWINDOW_H
