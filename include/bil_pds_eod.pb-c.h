/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#ifndef PROTOBUF_C_bil_5fpds_5feod_2eproto__INCLUDED
#define PROTOBUF_C_bil_5fpds_5feod_2eproto__INCLUDED

#include <protobuf-c.h>

PROTOBUF_C_BEGIN_DECLS


typedef struct _Npci__Pds__Eod__Protobuffer__Log Npci__Pds__Eod__Protobuffer__Log;
typedef struct _Npci__Pds__Eod__Protobuffer__TransactionLog Npci__Pds__Eod__Protobuffer__TransactionLog;


/* --- enums --- */


/* --- messages --- */

struct  _Npci__Pds__Eod__Protobuffer__Log
{
  ProtobufCMessage base;
  char *protoversion;
  char *date;
  char *time;
  char *vendor_id;
  char *terminalid;
  char *agent_id;
  char *agentcity;
  char *agentstate;
  char *agentpincode;
  size_t n_transactionlog;
  Npci__Pds__Eod__Protobuffer__TransactionLog **transactionlog;
};
#define NPCI__PDS__EOD__PROTOBUFFER__LOG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&npci__pds__eod__protobuffer__log__descriptor) \
    , NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,NULL }


struct  _Npci__Pds__Eod__Protobuffer__TransactionLog
{
  ProtobufCMessage base;
  char *sorucetransid;
  char *date;
  char *time;
  char *stan;
  char *rrn;
  char *transactionmode;
  char *customeraadhaar_no;
  char *cutomeriin;
  char *customername;
  char *customeraccount_no;
  char *customermobile_no;
  char *transactiontype;
  protobuf_c_boolean has_transactionamount;
  double transactionamount;
  protobuf_c_boolean has_availablebalance;
  double availablebalance;
  char *beneficiary_aadhaar_no;
  char *benefeiciary_iin;
  char *beneficiary_mobile_no;
  char *ifsc_code;
  char *status;
  char *responsecode;
  char *uidaiauthcode;
  char *authtype;
  char *nicrrn;
  char *reservedforfuture1;
  char *reservedforfuture2;
};
#define NPCI__PDS__EOD__PROTOBUFFER__TRANSACTION_LOG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&npci__pds__eod__protobuffer__transaction_log__descriptor) \
    , NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0, 0,0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }


/* Npci__Pds__Eod__Protobuffer__Log methods */
void   npci__pds__eod__protobuffer__log__init
                     (Npci__Pds__Eod__Protobuffer__Log         *message);
size_t npci__pds__eod__protobuffer__log__get_packed_size
                     (const Npci__Pds__Eod__Protobuffer__Log   *message);
size_t npci__pds__eod__protobuffer__log__pack
                     (const Npci__Pds__Eod__Protobuffer__Log   *message,
                      uint8_t             *out);
size_t npci__pds__eod__protobuffer__log__pack_to_buffer
                     (const Npci__Pds__Eod__Protobuffer__Log   *message,
                      ProtobufCBuffer     *buffer);
Npci__Pds__Eod__Protobuffer__Log *
       npci__pds__eod__protobuffer__log__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   npci__pds__eod__protobuffer__log__free_unpacked
                     (Npci__Pds__Eod__Protobuffer__Log *message,
                      ProtobufCAllocator *allocator);
/* Npci__Pds__Eod__Protobuffer__TransactionLog methods */
void   npci__pds__eod__protobuffer__transaction_log__init
                     (Npci__Pds__Eod__Protobuffer__TransactionLog         *message);
size_t npci__pds__eod__protobuffer__transaction_log__get_packed_size
                     (const Npci__Pds__Eod__Protobuffer__TransactionLog   *message);
size_t npci__pds__eod__protobuffer__transaction_log__pack
                     (const Npci__Pds__Eod__Protobuffer__TransactionLog   *message,
                      uint8_t             *out);
size_t npci__pds__eod__protobuffer__transaction_log__pack_to_buffer
                     (const Npci__Pds__Eod__Protobuffer__TransactionLog   *message,
                      ProtobufCBuffer     *buffer);
Npci__Pds__Eod__Protobuffer__TransactionLog *
       npci__pds__eod__protobuffer__transaction_log__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   npci__pds__eod__protobuffer__transaction_log__free_unpacked
                     (Npci__Pds__Eod__Protobuffer__TransactionLog *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Npci__Pds__Eod__Protobuffer__Log_Closure)
                 (const Npci__Pds__Eod__Protobuffer__Log *message,
                  void *closure_data);
typedef void (*Npci__Pds__Eod__Protobuffer__TransactionLog_Closure)
                 (const Npci__Pds__Eod__Protobuffer__TransactionLog *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor npci__pds__eod__protobuffer__log__descriptor;
extern const ProtobufCMessageDescriptor npci__pds__eod__protobuffer__transaction_log__descriptor;

PROTOBUF_C_END_DECLS


#endif  /* PROTOBUF_bil_5fpds_5feod_2eproto__INCLUDED */
