/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C_NO_DEPRECATED
#define PROTOBUF_C_NO_DEPRECATED
#endif

#include "bil_pds_eod.pb-c.h"
void   npci__pds__eod__protobuffer__log__init
                     (Npci__Pds__Eod__Protobuffer__Log         *message)
{
  static Npci__Pds__Eod__Protobuffer__Log init_value = NPCI__PDS__EOD__PROTOBUFFER__LOG__INIT;
  *message = init_value;
}
size_t npci__pds__eod__protobuffer__log__get_packed_size
                     (const Npci__Pds__Eod__Protobuffer__Log *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &npci__pds__eod__protobuffer__log__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t npci__pds__eod__protobuffer__log__pack
                     (const Npci__Pds__Eod__Protobuffer__Log *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &npci__pds__eod__protobuffer__log__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t npci__pds__eod__protobuffer__log__pack_to_buffer
                     (const Npci__Pds__Eod__Protobuffer__Log *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &npci__pds__eod__protobuffer__log__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Npci__Pds__Eod__Protobuffer__Log *
       npci__pds__eod__protobuffer__log__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Npci__Pds__Eod__Protobuffer__Log *)
     protobuf_c_message_unpack (&npci__pds__eod__protobuffer__log__descriptor,
                                allocator, len, data);
}
void   npci__pds__eod__protobuffer__log__free_unpacked
                     (Npci__Pds__Eod__Protobuffer__Log *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &npci__pds__eod__protobuffer__log__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   npci__pds__eod__protobuffer__transaction_log__init
                     (Npci__Pds__Eod__Protobuffer__TransactionLog         *message)
{
  static Npci__Pds__Eod__Protobuffer__TransactionLog init_value = NPCI__PDS__EOD__PROTOBUFFER__TRANSACTION_LOG__INIT;
  *message = init_value;
}
size_t npci__pds__eod__protobuffer__transaction_log__get_packed_size
                     (const Npci__Pds__Eod__Protobuffer__TransactionLog *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &npci__pds__eod__protobuffer__transaction_log__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t npci__pds__eod__protobuffer__transaction_log__pack
                     (const Npci__Pds__Eod__Protobuffer__TransactionLog *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &npci__pds__eod__protobuffer__transaction_log__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t npci__pds__eod__protobuffer__transaction_log__pack_to_buffer
                     (const Npci__Pds__Eod__Protobuffer__TransactionLog *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &npci__pds__eod__protobuffer__transaction_log__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Npci__Pds__Eod__Protobuffer__TransactionLog *
       npci__pds__eod__protobuffer__transaction_log__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Npci__Pds__Eod__Protobuffer__TransactionLog *)
     protobuf_c_message_unpack (&npci__pds__eod__protobuffer__transaction_log__descriptor,
                                allocator, len, data);
}
void   npci__pds__eod__protobuffer__transaction_log__free_unpacked
                     (Npci__Pds__Eod__Protobuffer__TransactionLog *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &npci__pds__eod__protobuffer__transaction_log__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor npci__pds__eod__protobuffer__log__field_descriptors[10] =
{
  {
    "protoVersion",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, protoversion),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "date",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, date),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "time",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, time),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Vendor_ID",
    4,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, vendor_id),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "TerminalID",
    5,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, terminalid),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Agent_ID",
    6,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, agent_id),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "AgentCity",
    7,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, agentcity),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "AgentState",
    8,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, agentstate),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "AgentPincode",
    9,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, agentpincode),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "transactionLog",
    10,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, n_transactionlog),
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__Log, transactionlog),
    &npci__pds__eod__protobuffer__transaction_log__descriptor,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned npci__pds__eod__protobuffer__log__field_indices_by_name[] = {
  6,   /* field[6] = AgentCity */
  8,   /* field[8] = AgentPincode */
  7,   /* field[7] = AgentState */
  5,   /* field[5] = Agent_ID */
  4,   /* field[4] = TerminalID */
  3,   /* field[3] = Vendor_ID */
  1,   /* field[1] = date */
  0,   /* field[0] = protoVersion */
  2,   /* field[2] = time */
  9,   /* field[9] = transactionLog */
};
static const ProtobufCIntRange npci__pds__eod__protobuffer__log__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 10 }
};
const ProtobufCMessageDescriptor npci__pds__eod__protobuffer__log__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "npci.pds.eod.protobuffer.Log",
  "Log",
  "Npci__Pds__Eod__Protobuffer__Log",
  "npci.pds.eod.protobuffer",
  sizeof(Npci__Pds__Eod__Protobuffer__Log),
  10,
  npci__pds__eod__protobuffer__log__field_descriptors,
  npci__pds__eod__protobuffer__log__field_indices_by_name,
  1,  npci__pds__eod__protobuffer__log__number_ranges,
  (ProtobufCMessageInit) npci__pds__eod__protobuffer__log__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor npci__pds__eod__protobuffer__transaction_log__field_descriptors[25] =
{
  {
    "SoruceTransID",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, sorucetransid),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Date",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, date),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Time",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, time),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "STAN",
    4,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, stan),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "RRN",
    5,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, rrn),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "transactionMode",
    6,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, transactionmode),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "CustomerAadhaar_No",
    7,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, customeraadhaar_no),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "CutomerIIN",
    8,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, cutomeriin),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "CustomerName",
    9,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, customername),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "CustomerAccount_No",
    10,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, customeraccount_no),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "CustomerMobile_No",
    11,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, customermobile_no),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "transactionType",
    12,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, transactiontype),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "TransactionAmount",
    13,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, has_transactionamount),
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, transactionamount),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "AvailableBalance",
    14,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, has_availablebalance),
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, availablebalance),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Beneficiary_Aadhaar_No",
    15,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, beneficiary_aadhaar_no),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Benefeiciary_IIN",
    16,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, benefeiciary_iin),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Beneficiary_Mobile_No",
    17,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, beneficiary_mobile_no),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "IFSC_Code",
    18,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, ifsc_code),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "status",
    19,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, status),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "ResponseCode",
    20,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, responsecode),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "UIDAIAuthCode",
    21,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, uidaiauthcode),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "AuthType",
    22,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, authtype),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "NICRRN",
    23,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, nicrrn),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "reservedForFuture1",
    24,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, reservedforfuture1),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "reservedForFuture2",
    25,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Npci__Pds__Eod__Protobuffer__TransactionLog, reservedforfuture2),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned npci__pds__eod__protobuffer__transaction_log__field_indices_by_name[] = {
  21,   /* field[21] = AuthType */
  13,   /* field[13] = AvailableBalance */
  15,   /* field[15] = Benefeiciary_IIN */
  14,   /* field[14] = Beneficiary_Aadhaar_No */
  16,   /* field[16] = Beneficiary_Mobile_No */
  6,   /* field[6] = CustomerAadhaar_No */
  9,   /* field[9] = CustomerAccount_No */
  10,   /* field[10] = CustomerMobile_No */
  8,   /* field[8] = CustomerName */
  7,   /* field[7] = CutomerIIN */
  1,   /* field[1] = Date */
  17,   /* field[17] = IFSC_Code */
  22,   /* field[22] = NICRRN */
  4,   /* field[4] = RRN */
  19,   /* field[19] = ResponseCode */
  3,   /* field[3] = STAN */
  0,   /* field[0] = SoruceTransID */
  2,   /* field[2] = Time */
  12,   /* field[12] = TransactionAmount */
  20,   /* field[20] = UIDAIAuthCode */
  23,   /* field[23] = reservedForFuture1 */
  24,   /* field[24] = reservedForFuture2 */
  18,   /* field[18] = status */
  5,   /* field[5] = transactionMode */
  11,   /* field[11] = transactionType */
};
static const ProtobufCIntRange npci__pds__eod__protobuffer__transaction_log__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 25 }
};
const ProtobufCMessageDescriptor npci__pds__eod__protobuffer__transaction_log__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "npci.pds.eod.protobuffer.TransactionLog",
  "TransactionLog",
  "Npci__Pds__Eod__Protobuffer__TransactionLog",
  "npci.pds.eod.protobuffer",
  sizeof(Npci__Pds__Eod__Protobuffer__TransactionLog),
  25,
  npci__pds__eod__protobuffer__transaction_log__field_descriptors,
  npci__pds__eod__protobuffer__transaction_log__field_indices_by_name,
  1,  npci__pds__eod__protobuffer__transaction_log__number_ranges,
  (ProtobufCMessageInit) npci__pds__eod__protobuffer__transaction_log__init,
  NULL,NULL,NULL    /* reserved[123] */
};
