#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "widget.h"
static const char cB64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


static const char cD64[] =
        "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";


void encodeBlock( unsigned char i[3], unsigned char o[4], int len)
{
    o[0] = cB64[ i[0] >> 2];
    o[1] = cB64[ ((i[0] & 0x03) << 4) | ((i[1] & 0xf0) >> 4)];
    o[2] = (unsigned char) (len > 1 ? cB64[ ((i[1] & 0x0f) << 2) | ((i[2] & 0xc0) >> 6)] : '=');
    o[3] = (unsigned char) (len > 2 ? cB64[ i[2] & 0x3f ] : '=');
}
//------------------------------------------------------------------------------


int base64Encode(const unsigned char *pInBuff, char *pOutBuff, const int iSize )
{
    unsigned char in[3], out[4];
    int i=0, j=0;

    while (i < iSize) {
        if ((i+3) <= iSize) {
            in[0] = pInBuff[i++];
            in[1] = pInBuff[i++];
            in[2] = pInBuff[i++];
            encodeBlock(in, out, 3);
        } else if ((i+2) == iSize) {
            in[0] = pInBuff[i++];
            in[1] = pInBuff[i++];
            in[2] = '\0';
            encodeBlock(in, out, 2);
        } else {
            in[0] = pInBuff[i++];
            in[1] = '\0';
            in[2] = '\0';
            encodeBlock(in, out, 1);
        }
        pOutBuff[j++] = out[0];
        pOutBuff[j++] = out[1];
        pOutBuff[j++] = out[2];
        pOutBuff[j++] = out[3];
    }
    pOutBuff[j]='\0';
    return(j);
}
//------------------------------------------------------------------------------


void decodeBlock( unsigned char i[4], unsigned char o[3] )
{
    o[0] = (unsigned char) (i[0] << 2 | i[1] >> 4);
    o[1] = (unsigned char) (i[1] << 4 | i[2] >> 2);
    o[2] = (unsigned char) (((i[2] << 6) & 0xc0) | i[3]);
}
//------------------------------------------------------------------------------


int base64Decode(const char *pInBuff,unsigned char *pOutBuff, const int iSize )
{
    unsigned char in[4], out[3], v;
    int i=0, j=0, k=0, len;

    while (i < iSize) {
        for (len=0; (len < 4) && (i < iSize); i++) {
            v = (unsigned char) pInBuff[i];
            v = (unsigned char) ((v < 43 || v > 122) ? 0 : cD64[v-43]);
            if (v)
                v = (unsigned char) ((v == '$') ? 0 : v-61);
            if (v == 0) {
                continue;
            }
            if (v)
                in[len++] = (unsigned char) (v-1);
            else
                in[len++] = 0;
        }
        if (len) {
            decodeBlock(in, out);
            for (k=0; k < (len-1); k++)
                pOutBuff[j++] = out[k];
        }
    }
    return(j);
}
/*static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};

void build_decoding_table() {

        decoding_table = (char *)malloc(256);

        for (int i = 0; i < 64; i++)
                decoding_table[(unsigned char) encoding_table[i]] = i;
}


void base64_cleanup() {
        free(decoding_table);
}
unsigned char *base64_decode(const char *data,
        size_t input_length,
        size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = (unsigned char *)malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
            + (sextet_b << 2 * 6)
            + (sextet_c << 1 * 6)
            + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}
char *base64_encode(const unsigned char *data,size_t input_length, size_t *output_length) {

    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = (char *)malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}
*/
