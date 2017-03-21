#ifndef UPDATEFILEOPERATOR_H
#define UPDATEFILEOPERATOR_H

#include <QString>
#include <QByteArray>

class UpdateFileOperator
{
public:
    enum FileOpetatorError
    {
        EnFileOp_NoError = 0,    //无错误
        EnFileOp_FileNotExist,   //文件不存在
        EnFileOp_OpenFileError,  //文件打开错误
        EnFileOp_FileDataError,  //文件数据错误
        EnFileOp_FileFormatError,//文件格式错误
    };

public:
    ~UpdateFileOperator();

    static UpdateFileOperator* getInstance();
    int readFile(const QString &filename, QByteArray &baData);

private:
    UpdateFileOperator();

    //ASCII字符转Hex
    bool asciiToHex(const QByteArray& arrAscii, quint8& hex);
    //读取Hex文件
    int readHexFile(const QString &filename, QByteArray &baData);
    //读取Bin文件
    int readBinFile(const QString &filename, QByteArray &baData);

};

#endif // UPDATEFILEOPERATOR_H
