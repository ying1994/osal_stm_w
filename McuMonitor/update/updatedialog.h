#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QString>
#include <QVector>
#include "comm/bdcomm.h"

namespace Ui {
class UpdateDialog;
}

class UpdateThread;

class UpdateDialog : public QDialog
{
    Q_OBJECT
public:
    //friend class UpdateThread;
public:
    enum {UPDATE_DATA_MAX = 256};

public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();

    //重置对话框
    virtual void reset(void);

    //读取文件
    bool readFile(void);
    //检查文件是否存在
    bool checkFile(QString &filename);

    //设置升级结点地址
    void setUpdateNodeAddr(quint16 addr);


protected:
    //窗口关闭事件响应函数
    void closeEvent( QCloseEvent *event);
    //窗口显示事件响应函数
    void showEvent( QShowEvent *event);

private:
    //调试单元消息处理
    void OnDebugUnitMsgEvent(BD_MsgTypeDef &msg);
    //升级单元消息处理
    void OnUpDateUnitMsgEvent(BD_MsgTypeDef &msg);

public slots:
    //协议消息事件回调函数
    void OnCommMsgEvent(BD_MsgTypeDef &msg);

    void OnUpdate_start();
    void OnUpdate_ready();
    void OnUpdate_updating();
    void OnUpdate_result();
    void OnUpdate_lastresult();
    void OnUpdate_end();

private slots:
    void on_pushButtonOpenFile_clicked();

    void on_pushButtonUpdate_clicked();


private:
    Ui::UpdateDialog *ui;

    QString m_strFileName;      //文件名

    BD_MsgTypeDef m_txMessage;

    QFileDialog* m_pFileDialog; //打开文件对话框
    QFileInfo* m_pFileInfo;     //文件信息操作实例

    quint16 m_uUpdateResult;
    quint16 m_uUpdateLastResult;

    UpdateThread* m_hUpdateThread; //程序升级线程
    qint32 m_nSendDataSize;

   quint16 m_uCRC16;
};



#endif // UPDATEDIALOG_H
