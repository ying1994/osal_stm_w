#ifndef TYPES
#define TYPES

#include <minwindef.h>

#define _DEBUG

#define TRACE qDebug
#ifdef _DEBUG
#define DBG(code) code
#else
#define DBG(code)
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
//typedef      void VOID;
typedef      char CHAR;
//typedef      char INT8;
typedef short int INT16;
typedef       int INT;
typedef       int INT32;
typedef      long LONG;

typedef unsigned       char BYTE;
typedef unsigned       char UCHAR;
typedef unsigned       char UINT8;
typedef unsigned short int  UINT16;
typedef unsigned       int  UINT;
typedef unsigned       int  UINT32;
typedef unsigned       long ULONG;

//typedef enum{FALSE = 0, TRUE = !FALSE}BOOL;

typedef VOID* HANDLE;

typedef void (*BD_BASE_FUNC)(void);


#ifndef NULL
#define NULL (VOID*)0
#endif

/**
 * @defgroup Blocks Description
 * @brief 模块结点描述
 * @{
 */
#define MASTER_UNIT_NODE_STR	"01 主控制单元"
#define DEBUG_UNIT_NODE_STR		"02 调试单元"
/**
 * @}
 */

/**
 * @defgroup Blocks Description
 * @brief 模块功能描述
 * @{
 */
#define UPDATE_UNIT_STR		"升级单元"
#define DEBUG_UNIT_STR		"调试单元"
#define ROUTER_UNIT_STR		"路由单元"
/**
 * @}
 */

/**
 * @brief 功能模块结点地址表
 */
typedef enum _RouterAddrTable
{
    LOCAL_NODE_ADDR = 0x00,		/*!< 本地结点地址 */
    MASTER_UNIT_ADDR,			/*!< 主控制单元结点地址 */
    DEBUG_UNIT_ADDR,			/*!< 调试单元结点地址 */
}RouterAddrTable;

/**
 * @brief 注册表信息块定义
 */
struct BD_RouterRegisterDef
{
    enum
    {
        MAX_BLOCK_NAME_LEN = 128,	/*!< 模块名称最大长度，包括“\n” */
        MAX_BLOCK_SIZE = 32 		/*!< 模块最大功能单元个数 */
    };
    UINT8 blockAddr;						/*!< 功能模块结点地址 */
    UCHAR blockName[MAX_BLOCK_NAME_LEN];	/*!< 功能模块名称 */
    UCHAR functionCount;					/*!< 模块功能单元个数 */
    UCHAR functionIDs[MAX_BLOCK_SIZE];		/*!< 模块功能单元ID集合 */

    BD_RouterRegisterDef()
    {
        memset(this, 0, sizeof(BD_RouterRegisterDef));
    }

    BD_RouterRegisterDef(const BD_RouterRegisterDef& r)
    {
        this->blockAddr = r.blockAddr;
        strcpy((char*)this->blockName, (const char*)r.blockName);
        this->functionCount = r.functionCount;
        memcpy(this->functionIDs, r.functionIDs, this->functionCount);
    }

    BD_RouterRegisterDef& operator= (const BD_RouterRegisterDef& r)
    {
        if (this == &r)
            return *this;
        this->blockAddr = r.blockAddr;
        strcpy((char*)this->blockName, (const char*)r.blockName);
        this->functionCount = r.functionCount;
        memcpy(this->functionIDs, r.functionIDs, this->functionCount);
        return *this;
    }

    friend inline bool operator == (const BD_RouterRegisterDef &r1, const BD_RouterRegisterDef &r2){return r1.blockAddr == r2.blockAddr;}
    friend inline bool operator != (const BD_RouterRegisterDef &r1, const BD_RouterRegisterDef &r2){return r1.blockAddr != r2.blockAddr;}
    friend inline bool operator <  (const BD_RouterRegisterDef &r1, const BD_RouterRegisterDef &r2){return r1.blockAddr < r2.blockAddr;}
    friend inline bool operator >  (const BD_RouterRegisterDef &r1, const BD_RouterRegisterDef &r2){return r1.blockAddr > r2.blockAddr;}
    friend inline bool operator <= (const BD_RouterRegisterDef &r1, const BD_RouterRegisterDef &r2){return r1.blockAddr <= r2.blockAddr;}
    friend inline bool operator >= (const BD_RouterRegisterDef &r1, const BD_RouterRegisterDef &r2){return r1.blockAddr >= r2.blockAddr;}
};

#endif // TYPES

