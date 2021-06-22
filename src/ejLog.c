
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"ejLog.h"


//void * pack__ejlog_protobuf(int *proto_size,const Npci__Pds__Eod__Log *cp_micro_insurance_proto)
void * pack__ejlog_protobuf(int *proto_size,const Npci__Pds__Eod__Protobuffer__Log *cp_micro_insurance_proto)
{

    printf("------------->%s###\n", cp_micro_insurance_proto->protoversion);
    printf("------------->%s###\n", cp_micro_insurance_proto->transactionlog[0]->sorucetransid);
    printf("------------->%s###\n", cp_micro_insurance_proto->transactionlog[0]->date);

//    *proto_size = npci__pds__eod__log__get_packed_size(cp_micro_insurance_proto);
    *proto_size = npci__pds__eod__protobuffer__log__get_packed_size(cp_micro_insurance_proto);

    printf("proto_size = %d\n", *proto_size);

    void *packed_data = malloc(*proto_size);
//    int packed_size = npci__pds__eod__log__pack(cp_micro_insurance_proto, packed_data);
    int packed_size = npci__pds__eod__protobuffer__log__pack(cp_micro_insurance_proto, packed_data);
    printf("proto expected size %d Actual Size %d\n",*proto_size,packed_size);


    if(packed_size != *proto_size)
    {
        free(packed_data);
        return NULL;
    }
#ifdef DEBUG_MODE

    int i=0;
    char *data1=(char *)packed_data;
    for(i=0;i<packed_size;i++)
        printf("%02x",(char )data1[i]);

    printf("\n");
#endif
    return packed_data;

}
/*
int strdupLog(char *buff)
{

    static  Npci__Pds__Eod__Log * nPcieodlog;//1
//    static Npci__Pds__Eod__Log  nPcieodlog;//4
//    static Npci__Pds__Eod__TransactionLog stTxnLog;//4
    static Npci__Pds__Eod__TransactionLog * stTxnLog;//3

    nPcieodlog = (Npci__Pds__Eod__Log*)malloc(sizeof(Npci__Pds__Eod__Log));//1
    printf("step 1\n");

//    nPcieodlog->transactionlog[0] = (Npci__Pds__Eod__TransactionLog*)malloc(sizeof(Npci__Pds__Eod__TransactionLog));//2

    stTxnLog = (Npci__Pds__Eod__TransactionLog*)malloc(sizeof(Npci__Pds__Eod__TransactionLog));//3


//    nPcieodlog->szTxnLog->
//    stTxnLog->sorucetransid = strdup(buff);//3
//    nPcieodlog->stTxnLog->sorucetransid = strdup(buff);

//    nPcieodlog->transactionlog[0]->sorucetransid = strdup(buff);//2
    nPcieodlog->protoversion = strdup(buff);//1

    printf("step 2\n");

    return 0;
}
*/
/*
int strdupLog(char *buff)
{
    static Npci__Pds__Eod__Log  *nPcieodlog;//4
    static Npci__Pds__Eod__TransactionLog *stTxnLog;//4
    stTxnLog = (Npci__Pds__Eod__TransactionLog*)malloc(sizeof(Npci__Pds__Eod__TransactionLog)*100);//3
    nPcieodlog = (Npci__Pds__Eod__Log*)malloc(sizeof(Npci__Pds__Eod__Log)*100);//1
    printf("step 1\n");
    nPcieodlog->transactionlog = (Npci__Pds__Eod__TransactionLog**)malloc(sizeof(Npci__Pds__Eod__TransactionLog)*100);//2

//    stTxnLog = (Npci__Pds__Eod__TransactionLog*)malloc(sizeof(Npci__Pds__Eod__TransactionLog));//3
    stTxnLog->sorucetransid = strdup(buff);//3
//    nPcieodlog->stTxnLog->sorucetransid = strdup(buff);

//    printf("------------>%s@@@@@@\n", stTxnLog->sorucetransid);

//    nPcieodlog->transactionlog[0]->sorucetransid = strdup(buff);//2
//    strcpy(nPcieodlog.transactionlog[0]->sorucetransid, buff);
//    nPcieodlog->protoversion = strdup(buff);//1

    nPcieodlog->transactionlog[0] = stTxnLog;
//    nPcieodlog->transactionlog[0]->sorucetransid = strdup(buff);//strdup(buff);//

    printf("------------->%s###\n", nPcieodlog->transactionlog[0]->sorucetransid);

    printf("step 2\n");

    return 0;
}
*/
int strdupLog(char *buff)
{
    static Npci__Pds__Eod__Protobuffer__Log  *nPcieodlog;//4
    static Npci__Pds__Eod__Protobuffer__TransactionLog *stTxnLog;//4
    stTxnLog = (Npci__Pds__Eod__Protobuffer__TransactionLog*)malloc(sizeof(Npci__Pds__Eod__Protobuffer__TransactionLog)*10);//3
    nPcieodlog = (Npci__Pds__Eod__Protobuffer__Log*)malloc(sizeof(Npci__Pds__Eod__Protobuffer__Log)*10);//1
    nPcieodlog->transactionlog = (Npci__Pds__Eod__Protobuffer__TransactionLog**)malloc(sizeof(Npci__Pds__Eod__Protobuffer__TransactionLog)*10);//2

    nPcieodlog->protoversion = strdup("1.0");

    stTxnLog->sorucetransid = strdup(buff);//3

    nPcieodlog->transactionlog[0] = stTxnLog;
    printf("------------->%s###\n", nPcieodlog->transactionlog[0]->sorucetransid);


    return 0;
}
