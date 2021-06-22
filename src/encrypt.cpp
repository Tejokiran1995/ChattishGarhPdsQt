#include "include/widget.h"
#include <openssl/evp.h>
#include <openssl/conf.h>
#include "openssl/aes.h"
#include "openssl/rand.h"
#include "openssl/bio.h"
#include "openssl/buffer.h"

//========================
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>

#include <openssl/bio.h>
#include <openssl/err.h>

#include <string.h>

#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bs64.h>

int base64Encode(const unsigned char *,char *, const int );
int padding = RSA_PKCS1_PADDING;

RSA * createRSA(unsigned char * key,int publicc)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    if(publicc)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }

    return rsa;
}

int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}

int Widget::rsaEncryption(QString input, QString rsaPubKey)
{
    qDebug()<<"================= RSA START=======================";
    //rsa pub key encryption
    qDebug()<<"rsaPubKey: "<<rsaPubKey;
    //    QFile file(rsaPubKey);
    //    if(!file.exists()){
    //        qDebug() << "File Not Found";
    //        return -1;
    //    }

    std::string strpk = rsaPubKey.toStdString();
    std::string strinpt = input.toStdString();

    //    char *buffer = NULL;
    //    size_t size = 0;

    //    /* Open your_file in read-only mode */
    //    FILE *fp = fopen(strpk.c_str(), "r");

    //    /* Get the buffer size */
    //    fseek(fp, 0, SEEK_END); /* Go to end of file */
    //    size = ftell(fp); /* How many bytes did we pass ? */

    //    /* Set position of stream to the beginning */
    //    rewind(fp);

    //    /* Allocate the buffer (no need to initialize it with calloc) */
    //    buffer = (char *)malloc((size + 1) * sizeof(*buffer)); /* size + 1 byte for the \0 */

    //    /* Read the file into the buffer */
    //    fread(buffer, size, 1, fp); /* Read 1 chunk of size bytes from fp into buffer */

    //    fclose(fp); //close file

    //    /* NULL-terminate the buffer */
    //    buffer[size] = '\0';

    //    /* Print it ! */
    //    printf("%s\n", buffer);

    //===============================================

    char plainText[512];
    memset(plainText, 0, sizeof(plainText));
    strcpy(plainText, strinpt.c_str());
    char * Base64Ret;

    char publicKey[1024];
    memset(publicKey, 0, sizeof(publicKey));
    strcpy(publicKey, "-----BEGIN PUBLIC KEY-----\n");
    strcat(publicKey, strpk.c_str());
    strcat(publicKey, "\n-----END PUBLIC KEY-----\n");

    printf("%s\n", publicKey);

    unsigned char  encrypted[1024]={};
    memset(encrypted, 0, sizeof(encrypted));

    qDebug()<<"I AM HERE";
    int encrypted_length = public_encrypt((unsigned char *)plainText,strlen(plainText),(unsigned char *)publicKey,encrypted);
    qDebug()<<"I AM HERE2";
    if(encrypted_length == -1)
    {
        // printLastError("Public Encrypt failed ");
        qDebug()<<"Public Encrypt failed ";
        exit(0);
    }

    //==============base 64====================
    int asciiBase64EncLen ;
//    asciiBase64Enc = base64( encrypted, encrypted_length, &asciiBase64EncLen ) ;
    Base64Ret = base64( encrypted, encrypted_length, &asciiBase64EncLen );

    memset(asciiBase64Enc,0x00,sizeof(asciiBase64Enc));
    strcpy(asciiBase64Enc,Base64Ret);

    printf("Base64: %s\n", asciiBase64Enc);
    printf("length bas64: %d\n",asciiBase64EncLen);

    qDebug()<<"=================RSA END=======================";

}
