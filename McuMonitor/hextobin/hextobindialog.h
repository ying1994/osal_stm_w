#ifndef HEXTOBINDIALOG_H
#define HEXTOBINDIALOG_H

#include <QDialog>


namespace Ui {
class HexToBinDialog;
}

class QString;

class HexToBinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HexToBinDialog(QWidget *parent = 0);
    ~HexToBinDialog();

    //ASCII字符转Hex
    bool asciiToHex(const QByteArray& arrAscii, quint8& hex);

    /**
     * @brief: 读取Hex文件
     */
    bool readHexFile(void);

protected:
    bool saveFile(void);
    //窗口关闭事件响应函数
    void closeEvent( QCloseEvent *event);
    //窗口显示事件响应函数
    void showEvent( QShowEvent *event);

private slots:
    void on_pushButtonHexFile_clicked();

    void on_pushButtonBinFile_clicked();

    void on_pushButtonStartConvert_clicked();

    void on_pushButtonExit_clicked();

private:
    Ui::HexToBinDialog *ui;

    QString m_strHexFileName;
    QString m_strBinFileName;
};

#endif // HEXTOBINDIALOG_H
