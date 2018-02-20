#ifndef _AES_H_
#define _AES_H_

#define RoundKeyLEN  300
void AES_Encrypt(char* pExpressText , char* pCipherText , char* pAeskey);
void AES_Decrypt(char* pExpressText , char* pCipherText , char* pAeskey);
#endif
