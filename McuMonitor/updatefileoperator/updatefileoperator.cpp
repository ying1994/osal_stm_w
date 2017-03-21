#include "updatefileoperator.h"

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

UpdateFileOperator::UpdateFileOperator()
{

}

UpdateFileOperator::~UpdateFileOperator()
{

}


UpdateFileOperator* UpdateFileOperator::getInstance()
{
    static UpdateFileOperator m_Instance;
    return &m_Instance;
}

//ASCII字符转Hex
bool UpdateFileOperator::asciiToHex(const QByteArray& arrAscii, quint8& hex)
{
    uchar high, low;
    high = arrAscii[0];//高4位
    low = arrAscii[1];//低4位

    if(low >= '0' && low <= '9')
    {
      low = low - '0';
    }
    else if(low >= 'a' && low<='f')
    {
      low = low-'a'+ 0xa;
    }
    else if(low >= 'A' && low <= 'F')
    {
      low = low - 'A'+ 0xa;
    }
    else
    {
      return false;
    }

    if(high >= '0' && high <= '9')
    {
      high = high - '0';
    }
    else if(high >= 'a' && high <= 'f')
    {
      high = high - 'a'+ 0xa;
    }
    else if(high >= 'A' && high <= 'F')
    {
      high = high - 'A'+ 0xa;
    }
    else
    {
      return false;
    }

    hex=(high << 4) | low;
    //DBG(TRACE("hex: %02x\n",hex));

    return true;
}


/**
 * @brief: 读取Hex文件
 *         hex文件格式:
 *         冒号(1B)	本行数据长度(1B)	本行数据起始地址(2B)	数据类型(1B)	数据(nB)	校验码(1B)
 *
 *         数据类型有：0x00、0x01、0x02、0x03、0x04、0x05。
 *         '00' Data Rrecord：用来记录数据，HEX文件的大部分记录都是数据记录
 *         '01' End of File Record:用来标识文件结束，放在文件的最后，标识HEX文件的结尾
 *         '02' Extended Segment Address Record:用来标识扩展段地址的记录
 *         '03' Start Segment Address Record:开始段地址记录
 *         '04' Extended Linear Address Record:用来标识扩展线性地址的记录
 *         '05' Start Linear Address Record:开始线性地址记录
 *
 *         校验和的算法为：计算校验和前所有16进制码的累加和(不计进位)，检验和 = 0x100 - 累加和
 */
int UpdateFileOperator::readHexFile(const QString &filename, QByteArray &baData)
{
    int retval = EnFileOp_NoError;
    QFileInfo fileInfo(filename);
    if (!fileInfo.exists())
        return EnFileOp_FileNotExist;

    QFile file(fileInfo.filePath());

    quint8 hexdata = 0;
    quint16 index = 0;

    quint8 lineLen = 0;//本行数据长度
    quint16 uAddr = 0;//数据地址
    quint8 datatype = 0; //数据类型
    QByteArray arrayReadData;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        file.close();
        return EnFileOp_OpenFileError;
    }
    arrayReadData.clear();
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();

        index = 0;
        if (line.at(index) != ':')
        {
            retval = EnFileOp_FileDataError;
            break;
        }

        index += 1;                             //本行数据长度
        asciiToHex(line.mid(index, 2), hexdata);
        lineLen = hexdata;

        index += 2;                             //本行数据地址高8位
        asciiToHex(line.mid(index, 2), hexdata);
        uAddr = hexdata;

        index += 2;                             //本行数据地址低8位
        asciiToHex(line.mid(index, 2), hexdata);
        uAddr = (uAddr << 8) | hexdata;

        index += 2;                             //数据类型
        asciiToHex(line.mid(index, 2), hexdata);
        datatype = hexdata;

        if (0x00 == datatype)//数据记录
        {
            for (quint16 i = 0; i < lineLen; i++)
            {
                index += 2;
                asciiToHex(line.mid(index, 2), hexdata);
                arrayReadData.push_back((char)hexdata);
            }
        }
        else if (0x01 == datatype)//文件结束
        {
            baData = arrayReadData;
            retval = EnFileOp_NoError;
            break;
        }
        else if (0x02 == datatype)//标识扩展段地址的记录
        {
        }
        else if (0x03 == datatype)//开始段地址记录
        {
        }
        else if (0x04 == datatype)//标识扩展线性地址的记录
        {
            quint32 uBaseAddr = 0;
            index += 2;                             //数据基地址高8位
            asciiToHex(line.mid(index, 2), hexdata);
            uBaseAddr = hexdata;
            //qDebug("%02x", hexdata);

            index += 2;                             //数据基地址低8位
            asciiToHex(line.mid(index, 2), hexdata);
            uBaseAddr = (uBaseAddr << 8) | hexdata;
            uBaseAddr <<= 16;
            //m_hUpdateThread->m_vBaseAddrs.push_back(uBaseAddr);
        }
        else if (0x05 == datatype)//开始线性地址记录
        {
        }
    }
    file.close();


    return retval;
}

//读取Bin文件
int UpdateFileOperator::readBinFile(const QString &filename, QByteArray &baData)
{
    QFileInfo fileInfo(filename);
    if (!fileInfo.exists())
        return EnFileOp_FileNotExist;

    QFile file(fileInfo.filePath());

    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        return EnFileOp_OpenFileError;
    }

    baData  = file.readAll();

    file.close();

    return EnFileOp_NoError;
}

int UpdateFileOperator::readFile(const QString &filename, QByteArray &baData)
{
    QFileInfo fileinfo(filename);
    QString suffix = fileinfo.suffix();//获取文件类型，即文件最后一个点之后的字符串
    if (suffix == "hex")
    {
        return readHexFile(filename, baData);
    }
    else if (suffix == "bin")
    {
        return readBinFile(filename, baData);
    }

    return EnFileOp_FileFormatError;
}

