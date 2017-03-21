#ifndef ADVANCEDENCRYPTIONSTANDARD_H
#define ADVANCEDENCRYPTIONSTANDARD_H


class AdvancedEncryptionStandard
{
public:
    AdvancedEncryptionStandard();
    ~AdvancedEncryptionStandard();
    //设置密码，密码长度为固定16位
    void setKey(unsigned char *key);
    //加密过程
    void* cipher(void* input, int length);
    //解密过程
    void* invCipher(void* input, int length);

private:
    //ByteSubstitution（字节替代）
    void subBytes(unsigned char state[][4]);
    //InvByteSubstitution（逆字节替代）
    void invSubBytes(unsigned char state[][4]);
    //ShiftRows（行移位变换）
    void shiftRows(unsigned char state[][4]);
    //InvShiftRows（逆行移位变换）
    void invShiftRows(unsigned char state[][4]);
    //有限域GF(2^8)上的乘法
    unsigned char fFmul(unsigned char a, unsigned char b);
    //MixColumns（列混淆变换）
    void mixColumns(unsigned char state[][4]);
    //InvMixColumns（逆列混淆变换）
    void invMixColumns(unsigned char state[][4]);
    //AddRoundKey（轮密钥加变换）
    void addRoundKey(unsigned char state[][4], unsigned char k[][4]);
    // KeyExpansion（密钥扩展）
    void keyExpansion(unsigned char* key, unsigned char w[][4][4]);


    //加密过程
    unsigned char* cipher(unsigned char* input);
    //解密过程
    unsigned char* invCipher(unsigned char* input);

private:
    //置换表
    unsigned char m_sBox[256];
    //下面是逆置换表，解密时使用
    unsigned char m_invSBox[256];
    //扩展密钥
    unsigned char m_w[11][4][4];
};

#endif // ADVANCEDENCRYPTIONSTANDARD_H
