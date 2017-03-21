#ifndef INIPROFILE_H
#define INIPROFILE_H
#include <windows.h>
#include <winbase.h>
#include <QString>

class QString;

class IniProfile
{
public:
    IniProfile();
    IniProfile(const QString &filename);
    ~IniProfile();
    /**
     * @brief 向INI文件写入字符串
     * @param strSectionName 节名
     * @param strKeyName 键名
     * @param strValue 写入的字符串
     * @return 写入成功返回TRUE，失败返回FALSE
     */
    BOOL setProfileString(const QString &strSectionName, const QString &strKeyName, const QString &strValue);
    /**
     * @brief 读取INI文件中的字符串
     * @param strSectionName 节名
     * @param strKeyName 键名
     * @return 返回读取的字符串
     */
    QString getProfileString(const QString &strSectionName, const QString &strKeyName);
    /**
     * @brief 向INI文件写入字符串
     * @param strSectionName 节名
     * @param strKeyName 键名
     * @param uValue 写入的数字
     * @return 写入成功返回TRUE，失败返回FALSE
     */
    BOOL setProfileInt(const QString &strSectionName, const QString &strKeyName, UINT uValue);
    /**
     * @brief 读取INI文件中的数字
     * @param strSectionName 节名
     * @param strKeyName 键名
     * @return 返回读取到的数字
     */
    UINT getProfileInt(const QString &strSectionName, const QString &strKeyName);

private:
    QString m_strFileName;
};

#endif // INIPROFILE_H
