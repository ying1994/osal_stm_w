#include "hextobindialog.h"
#include "ui_hextobindialog.h"

#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>

#include "updatefileoperator/updatefileoperator.h"


HexToBinDialog::HexToBinDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HexToBinDialog)
{
    ui->setupUi(this);

    m_strHexFileName.clear();
    m_strBinFileName.clear();
    this->ui->lineEditPassword->setEchoMode(QLineEdit::Password);//密码显示模式
}

HexToBinDialog::~HexToBinDialog()
{
    delete ui;
}



//窗口关闭事件响应函数
void HexToBinDialog::closeEvent( QCloseEvent *event)
{
    QDialog::closeEvent(event);
}

//窗口显示事件响应函数
void HexToBinDialog::showEvent( QShowEvent *event)
{
    QFileInfo fileInfo(m_strHexFileName);
    if (fileInfo.exists() && !m_strBinFileName.isEmpty())
    {
        this->ui->pushButtonBinFile->setEnabled(true);
        this->ui->pushButtonStartConvert->setEnabled(true);
    }
    else
    {
        this->ui->pushButtonBinFile->setEnabled(false);
        this->ui->pushButtonStartConvert->setEnabled(false);
    }

    QDialog::showEvent(event);
}


bool HexToBinDialog::saveFile(void)
{
    bool bRetVal = true;
    int status = 0;
    QByteArray baData;
    status = UpdateFileOperator::getInstance()->readFile(m_strHexFileName, baData);

    if (UpdateFileOperator::EnFileOp_FileNotExist == status)
    {
        QMessageBox::warning(this, tr("HexToBin"),
                             tr("文件不存在！"),
                             QMessageBox::Yes);
        return false;
    }
    else if (UpdateFileOperator::EnFileOp_OpenFileError == status)
    {
        QMessageBox::warning(this, tr("HexToBin"),
                             tr("文件打开失败！"),
                             QMessageBox::Yes);
        return false;
    }
    else  if (UpdateFileOperator::EnFileOp_FileDataError == status)
    {
        QMessageBox::warning(this, tr("HexToBin"),
                             tr("文件数据格式错误！"),
                             QMessageBox::Yes);
        return false;
    }
    else if (UpdateFileOperator::EnFileOp_FileFormatError == status)
    {
        QMessageBox::warning(this, tr("HexToBin"),
                             tr("文件类型错误！"),
                             QMessageBox::Yes);
        return false;
    }
    else
    {
        QFile file(m_strBinFileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("HexToBin"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            file.close();
            return false;
        }

        if (this->ui->checkBoxEncryption->isChecked())
        {
            QMessageBox::warning(this, tr("HexToBin"),
                                 tr("加密功能未实现！"),
                                 QMessageBox::Yes);
            bRetVal = false;
        }
        else
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            file.write(baData.constData(), baData.size());
            file.flush();
            QApplication::restoreOverrideCursor();
            bRetVal = true;
        }
        file.close();

        return bRetVal;
    }
}

void HexToBinDialog::on_pushButtonHexFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                               tr("Open Hex File"), "",
                               tr("Hex files (*.hex)"));

    QFileInfo fileInfo(filename);
    if (fileInfo.exists())
    {
        m_strHexFileName = filename;
        this->ui->pushButtonBinFile->setEnabled(true);
    }
    else
    {
        m_strHexFileName.clear();
        this->ui->pushButtonBinFile->setEnabled(false);
    }
    this->ui->lineEditHexFileName->setText(m_strHexFileName);
}

void HexToBinDialog::on_pushButtonBinFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                         tr("Save Bin File"), ".",
                                         tr("Bin files (*.bin)"));
    if (!fileName.isEmpty())
    {
        m_strBinFileName = fileName;
        this->ui->pushButtonStartConvert->setEnabled(true);
    }
    else
    {
        m_strBinFileName.clear();
        this->ui->pushButtonStartConvert->setEnabled(false);
    }
    this->ui->lineEditBinFileName->setText(m_strBinFileName);

}

void HexToBinDialog::on_pushButtonStartConvert_clicked()
{
    if (saveFile())
    {
        QMessageBox::warning(this, tr("HexToBin"),
                             tr("文件转换完成\n"),
                             QMessageBox::Yes);
        this->close();
    }
}

void HexToBinDialog::on_pushButtonExit_clicked()
{
    this->close();
}
