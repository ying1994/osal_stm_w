#include "iniprofile.h"
#include <QString>

IniProfile::IniProfile():m_strFileName("")
{

}

IniProfile::IniProfile(const QString &filename):m_strFileName(filename)
{

}

IniProfile::~IniProfile()
{

}


/**
 * @brief 向INI文件写入字符串
 * @param strSectionName 节名
 * @param strKeyName 键名
 * @param strValue 写入的字符串
 * @return 写入成功返回TRUE，失败返回FALSE
 */
BOOL IniProfile::setProfileString(const QString &strSectionName, const QString &strKeyName, const QString &strValue)
{
    //BOOL WritePrivateProfileStringW (LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString, LPCWSTR lpFileName);
    return WritePrivateProfileString(strSectionName.toStdWString().c_str(), strKeyName.toStdWString().c_str(), strValue.toStdWString().c_str(), m_strFileName.toStdWString().c_str());
}
/**
 * @brief 读取INI文件中的字符串
 * @param strSectionName 节名
 * @param strKeyName 键名
 * @return 返回读取的字符串
 */
QString IniProfile::getProfileString(const QString &strSectionName, const QString &strKeyName)
{
    //DWORD GetPrivateProfileStringW (LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName);

    QString strResult("");
    DWORD size = 0;
    WCHAR wbuffer[64];
    char buffer[64];
    size = GetPrivateProfileString(strSectionName.toStdWString().c_str(), strKeyName.toStdWString().c_str(), L"", wbuffer, sizeof(buffer), m_strFileName.toStdWString().c_str());

    if (size > 0)
    {
        wcstombs(buffer, wbuffer, sizeof(wbuffer));
        strResult = buffer;
    }
    return strResult;
}
/**
 * @brief 向INI文件写入字符串
 * @param strSectionName 节名
 * @param strKeyName 键名
 * @param uValue 写入的数字
 * @return 写入成功返回TRUE，失败返回FALSE
 */
BOOL IniProfile::setProfileInt(const QString &strSectionName, const QString &strKeyName, UINT uValue)
{
    //BOOL WritePrivateProfileStringW (LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString, LPCWSTR lpFileName);
    char buffer[32];
    itoa(uValue, buffer, 10);
    QString strValue = buffer;
    return WritePrivateProfileString(strSectionName.toStdWString().c_str(), strKeyName.toStdWString().c_str(), strValue.toStdWString().c_str(), m_strFileName.toStdWString().c_str());
}
/**
 * @brief 读取INI文件中的数字
 * @param strSectionName 节名
 * @param strKeyName 键名
 * @return 返回读取到的数字
 */
UINT IniProfile::getProfileInt(const QString &strSectionName, const QString &strKeyName)
{
    //UINT GetPrivateProfileIntW (LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName);
    UINT uValue = 0;
    uValue = GetPrivateProfileInt(strSectionName.toStdWString().c_str(), strKeyName.toStdWString().c_str(), 0, m_strFileName.toStdWString().c_str());
    return uValue;
}
