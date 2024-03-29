#include"iso8583.h"
isodef d[] = {
		/*000*/ {ISO_NUMERIC, 0, 2, "Message Type Indicator"},
		/*001*/ {ISO_BITMAP, 0, 32, "Bitmap"},
		/*002*/ {ISO_NUMERIC, 2, 19, "Primary Account number"},
		/*003*/ {ISO_NUMERIC, 0, 6, "Processing Code"},
		/*004*/ {ISO_NUMERIC, 0, 12, "Amount, Transaction"},
		/*005*/ {ISO_NUMERIC, 0, 12, "Amount, Reconciliation"},
		/*006*/ {ISO_NUMERIC, 0, 12, "Amount, Cardholder billing"},
		/*007*/ {ISO_NUMERIC, 0, 10, "Date and time, transmission"},
		/*008*/ {ISO_NUMERIC, 0, 8, "Amount, Cardholder billing fee"},
		/*009*/ {ISO_NUMERIC, 0, 8, "Conversion rate, Reconciliation"},
		/*010*/ {ISO_NUMERIC, 0, 8, "Conversion rate, Cardholder billing"},
		/*011*/ {ISO_NUMERIC, 0, 6, "Systems trace audit number"},
        /*012*/ {ISO_NUMERIC, 0, 6, "Date and time, Local transaction"},//Changed frm 12 for GenPact
		/*013*/ {ISO_NUMERIC, 0, 4, "Date, Effective"},
		/*014*/ {ISO_NUMERIC, 0, 4, "Date, Expiration"},
		/*015*/ {ISO_NUMERIC, 0, 4, "Date, Settlement"},
		/*016*/ {ISO_NUMERIC, 0, 4, "Date, Conversion"},
		/*017*/ {ISO_NUMERIC, 0, 4, "Date, Capture"},
		/*018*/ {ISO_NUMERIC, 0, 4, "Merchant type"},
		/*019*/ {ISO_NUMERIC, 0, 3, "Country code, Acquiring institution"},
		/*020*/ {ISO_NUMERIC, 0, 3, "Country code, Primary account number"},
		/*021*/ {ISO_NUMERIC, 0, 3, "Country code, Forwarding institution"},
        /*022*/ {ISO_NUMERIC, 0, 3, "Point of service data code"}, //Changed frm 15 for GenPact
		/*023*/ {ISO_NUMERIC, 0, 3, "Card sequence number"},
		/*024*/ {ISO_NUMERIC, 0, 3, "NIID"},
		/*025*/ {ISO_NUMERIC, 0, 2, "Message reason code"},
		/*026*/ {ISO_NUMERIC, 0, 2, "Card acceptor business code"},
		/*027*/ {ISO_NUMERIC, 0, 1, "Approval code length"},
		/*028*/ {ISO_NUMERIC, 0,6, "Date, Reconciliation"},
		/*029*/ {ISO_NUMERIC, 0,3, "Reconciliation indicator"},
		/*030*/ {ISO_NUMERIC, 0,1+8, "Amounts, original"},
		/*031*/ {ISO_NUMERIC, 0,1+8, "Acquirer reference data"},
        /*032*/ {ISO_NUMERIC, 2, 11, "Acquirer institution identification code"},
		/*033*/ {ISO_NUMERIC, 2, 11, "Forwarding institution identification code"},
		/*034*/ {ISO_ALPHANUMERIC, 2, 28, "Primary account number, extended"},
		/*035*/ {ISO_ALPHANUMERIC, 2, 37, "Track 2 data"},
		/*036*/ {ISO_ALPHANUMERIC, 3, 104, "Track 3 data"},
		/*037*/ {ISO_ALPHANUMERIC, 0, 12, "Retrieval reference number"},
		/*038*/ {ISO_ALPHANUMERIC, 0, 6, "Approval code"},
		/*039*/ {ISO_ALPHANUMERIC, 0, 2, "Action code"},
		/*040*/ {ISO_NUMERIC, 0, 3, "Service code"},
        /*041*/ {ISO_ALPHANUMERIC, 0, 16, "Card acceptor terminal identification"}, //Changed frm 8 for GenPAct
		/*042*/ {ISO_ALPHANUMERIC, 0, 15, "Card acceptor identification code"},
		/*043*/ {ISO_ALPHANUMERIC, 0, 40, "Card acceptor name/location"},
		/*044*/ {ISO_ALPHANUMERIC, 2, 99, "Additional response data"},
		/*045*/ {ISO_ALPHANUMERIC, 2, 76, "Track 1 data"},
        /*046*/ {ISO_ALPHANUMERIC, 3, 999, "Amounts, Fees"},//Changed from 204 for GenPact
		/*047*/ {ISO_ALPHANUMERIC, 3, 999, "Additional data - national"},
		/*048*/ {ISO_ALPHANUMERIC, 4, 999, "Additional data - private"},
		/*049*/ {ISO_NUMERIC, 0, 3, "Currency code, Transaction"},
		/*050*/ {ISO_NUMERIC, 0, 3, "Currency code, Reconciliation"},
		/*051*/ {ISO_ALPHANUMERIC, 0, 3, "Currency code, Cardholder billing"},
		///*052*/ {ISO_BINARY, 0, 8, "Personal identification number, PIN) data"},
		/*052*/ {ISO_ALPHANUMERIC, 0, 16, "Personal identification number, PIN) data"},
		/*053*/ {ISO_BINARY, 2, 48, "Security related control information"},
		/*054*/ {ISO_ALPHANUMERIC, 3, 120, "Amounts, additional"},
		/*055*/ {ISO_BINARY, 2, 999, "IC card system related data"},
		/*056*/ {ISO_NUMERIC, 2, 35, "Original data elements"},
		/*057*/ {ISO_NUMERIC, 3, 3, "Authorization life cycle code"},
		/*058*/ {ISO_NUMERIC, 3, 11, "Authorizing agent institution Id Code"},
		/*059*/ {ISO_ALPHANUMERIC, 3, 999, "Transport data"},
		/*060*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*061*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*062*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for private use"},
		/*063*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for private use"},
		/*064*/ {ISO_BINARY, 0, 8, "Message authentication code field"},
		/*065*/ {ISO_BINARY, 0, 8, "Reserved for ISO use"},
		/*066*/ {ISO_NUMERIC, 0, 1, "Amounts, original fees"},
		/*067*/ {ISO_NUMERIC, 0, 2, "Extended payment data"},
		/*068*/ {ISO_NUMERIC, 0, 3, "Country code, receiving institution"},
		/*069*/ {ISO_NUMERIC, 0, 3, "Country code, settlement institution"},
		/*070*/ {ISO_NUMERIC, 0, 3, "Country code, authorizing agent Inst."},
		/*071*/ {ISO_NUMERIC, 0, 8, "Message number"},
		/*072*/ {ISO_ALPHANUMERIC, 3, 999, "Data record"},
		/*073*/ {ISO_NUMERIC, 0, 6, "Date, action"},
		/*074*/ {ISO_NUMERIC, 0, 10, "Credits, number"},
		/*075*/ {ISO_NUMERIC, 0, 10, "Credits, reversal number"},
		/*076*/ {ISO_NUMERIC, 0, 10, "Debits, number"},
		/*077*/ {ISO_NUMERIC, 0, 10, "Debits, reversal number"},
		/*078*/ {ISO_NUMERIC, 0, 10, "Transfer, number"},
		/*079*/ {ISO_NUMERIC, 0, 10, "Transfer, reversal number"},
		/*080*/ {ISO_NUMERIC, 0, 10, "Inquiries, number"},
		/*081*/ {ISO_NUMERIC, 0, 10, "Authorizations, number"},
		/*082*/ {ISO_NUMERIC, 0, 12, "Inquiries, reversal number"},
		/*083*/ {ISO_NUMERIC, 0, 12, "Payments, number"},
		/*084*/ {ISO_NUMERIC, 0, 12, "Payments, reversal number"},
		/*085*/ {ISO_NUMERIC, 0, 12, "Fee collections, number"},
		/*086*/ {ISO_NUMERIC, 0, 16, "Credits, amount"},
		/*087*/ {ISO_NUMERIC, 0, 16, "Credits, reversal amount"},
		/*088*/ {ISO_NUMERIC, 0, 16, "Debits, amount"},
		/*089*/ {ISO_NUMERIC, 0, 16, "Debits, reversal amount"},
		/*090*/ {ISO_NUMERIC, 0, 10, "Authorizations, reversal number"},
		/*091*/ {ISO_ALPHANUMERIC, 0, 1, "Country code, transaction Dest. Inst."},
		/*092*/ {ISO_ALPHANUMERIC, 0, 3, "Country code, transaction Orig. Inst."},
		/*093*/ {ISO_NUMERIC, 2, 11, "Transaction Dest. Inst. Id code"},
		/*094*/ {ISO_NUMERIC, 2, 11, "Transaction Orig. Inst. Id code"},
		/*095*/ {ISO_ALPHANUMERIC,0 ,42, "Card issuer reference data"},
		/*096*/ {ISO_BINARY, 3, 999, "Key management data"},
		/*097*/ {ISO_NUMERIC, 0, 1+16, "Amount, Net reconciliation"}, /* was ISO_AMOUNT */
		/*098*/ {ISO_ALPHANUMERIC, 0, 25, "Payee"},
		/*099*/ {ISO_ALPHANUMERIC, 2, 11, "Settlement institution Id code"},
		/*100*/ {ISO_NUMERIC, 2, 11, "Receiving institution Id code"},
		/*101*/ {ISO_ALPHANUMERIC, 2, 17, "File name"},
		/*102*/ {ISO_ALPHANUMERIC, 2, 28, "Account identification 1"},
		/*103*/ {ISO_ALPHANUMERIC, 2, 28, "Account identification 2"},
		/*104*/ {ISO_ALPHANUMERIC, 3, 100, "Transaction description"},
		/*105*/ {ISO_NUMERIC, 0, 16, "Credits, Chargeback amount"},
		/*106*/ {ISO_NUMERIC, 0, 16, "Debits, Chargeback amount"},
		/*107*/ {ISO_NUMERIC, 0, 10, "Credits, Chargeback number"},
		/*108*/ {ISO_NUMERIC, 0, 10, "Debits, Chargeback number"},
		/*109*/ {ISO_ALPHANUMERIC, 2, 84, "Credits, Fee amounts"},
		/*110*/ {ISO_ALPHANUMERIC, 2, 84, "Debits, Fee amounts"},
		/*111*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for ISO use"},
		/*112*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for ISO use"},
		/*113*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for ISO use"},
		/*114*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for ISO use"},
		/*115*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for ISO use"},
		/*116*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*117*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*118*/ {ISO_NUMERIC, 3, 999, "Reserved for national use"},
		/*119*/ {ISO_NUMERIC, 3, 999, "Reserved for national use"},
		/*120*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*121*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*122*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*123*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for private use"},
		/*124*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for private use"},
		/*125*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for private use"},
		/*126*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for private use"},
		/*127*/ {ISO_ALPHANUMERIC, 6, 999, "Reserved for private use"},
		/*128*/ {ISO_BINARY, 0, 8, "Message authentication code field"},
		/*129-48.00*/{ISO_BITMAP,0,2,"DE48 Field Bitmap"},
		/*130-48.01*/ {ISO_ALPHANUMERIC, 0, 19, "POS Data"},
		/*131-48.02*/ {ISO_ALPHANUMERIC, 0, 2, "Authorization profile"},
		/*132-48.03*/ {ISO_NUMERIC, 0, 4, "Routing Information"},
		/*133-48.04*/ {ISO_NUMERIC, 0, 4, "POS data"},
		/*134-48.05*/ {ISO_ALPHANUMERIC, 3, 255, "Service Station Data"},
		/*135-48.06*/ {ISO_ALPHANUMERIC, 3, 999, "Authorization Profile"},
		/*136-48.07*/ {ISO_ALPHANUMERIC, 0, 48, "Check Data"},
		/*137-48.08*/ {ISO_ALPHANUMERIC, 2, 50, "Retention Data"},
		/*138-48.09*/ {ISO_ALPHANUMERIC, 0, 29, "Additional Node data "},
		/*139-48.10*/ {ISO_ALPHANUMERIC, 0, 1, "CVV2"},
		/*140-48.11*/ {ISO_ALPHANUMERIC, 0, 1, "Reserver for private use"},
		/*141-48.12*/ {ISO_ALPHANUMERIC,3,999,"Terminal Owner"},
		/*142-48.13*/ {ISO_ALPHANUMERIC, 0,31, "POS Geographic Data"},
		/*143-48.14*/ {ISO_ALPHANUMERIC, 0, 253, "Sponsor Bank"},
		/*144-48.15*/ {ISO_ALPHANUMERIC, 2, 28, "Address Verification Data"},
		/*145-48.16*/ {ISO_ALPHANUMERIC, 4, 9999, "Address Verification Result"},
		/*146-127.00*/ {ISO_BITMAP, 0, 8, "Bitmap"},
		/*147-127.01*/ {ISO_ALPHANUMERIC, 2, 99, "Reserved for private use"},
		/*148-127.02*/ {ISO_ALPHANUMERIC, 2, 99, "Reserved for private use"},
		/*149-127.03*/ {ISO_ALPHANUMERIC, 0, 48, "Routing Information"},
		/*150-127.04*/ {ISO_ALPHANUMERIC, 0, 22, "POS data"},
		/*151-127.05*/ {ISO_ALPHANUMERIC, 0, 73, "Service Station Data"},
		/*152-127.06*/ {ISO_NUMERIC, 0, 2, "Authorization Profile"},
		/*153-127.07*/ {ISO_ALPHANUMERIC, 2, 50, "Check Data"},
		/*154-127.08*/ {ISO_ALPHANUMERIC, 3, 999, "Retention Data"},
		/*155-127.09*/ {ISO_ALPHANUMERIC, 3, 999, "Additional Node data "},
		/*156-127.10*/ {ISO_NUMERIC, 0, 3, "CVV2"},
		/*157-127.11*/ {ISO_NUMERIC, 0, 6, "Reserver for private use"},
		/*158-127.12*/ {ISO_ALPHANUMERIC,2,25,"Terminal Owner"},
		/*159-127.13*/ {ISO_ALPHANUMERIC, 0,17, "POS Geographic Data"},
		/*160-127.14*/ {ISO_ALPHANUMERIC, 0, 8, "Sponsor Bank"},
		/*161-127.15*/ {ISO_ALPHANUMERIC, 2, 29, "Address Verification Data"},
		/*162-127.16*/ {ISO_ALPHANUMERIC, 0, 1, "Address Verification Result"},
		/*163-127.17*/ {ISO_ALPHANUMERIC, 2, 50, "Card holder Information"},
		/*164-127.18*/ {ISO_ALPHANUMERIC, 2, 50, "Validation Data"},
		/*165-127.19*/ {ISO_ALPHANUMERIC, 0, 31, "Country code, Acquiring institution"},
		/*166-127.20*/ {ISO_NUMERIC, 0, 8, "Original/authorizer data settlement(CCYYMMDD)"},
		/*167-127.21*/ {ISO_ALPHANUMERIC, 2, 12, "Record Identification"},
		/*168-127.22*/ {ISO_ALPHANUMERIC, 6, 9999, "Structured Data"},
		/*169-127.23*/ {ISO_ALPHANUMERIC, 0, 253, "Payee Name and Address"},
		/*170-127.24*/ {ISO_ALPHANUMERIC, 2, 28, "Payer Account"},
		/*171-127.25*/ {ISO_ALPHANUMERIC, 4, 8000, "ICC Data"},
		/*172-127.26*/ {ISO_ALPHANUMERIC, 2, 20, "Original Node"},
		/*173-127.27*/ {ISO_ALPHANUMERIC, 0, 1, "Card Verification Result"},
		/*174-127.28*/ {ISO_NUMERIC, 0, 4, "American Express Card Id(CID)"},
		/*175-127.29*/ {ISO_BINARY, 0, 40, "3D Secure Data"},
		/*176-127.30*/ {ISO_ALPHANUMERIC, 0, 1, "3D Secure Result"},
		/*177-127.31*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*178-127.32*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*179-127.33*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*180-127.34*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*181-127.35*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*182-127.36*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*183-127.37*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*184-127.38*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*185-127.39*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*186-127.40*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*187-127.41*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*188-127.42*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*189-127.43*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*190-127.44*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*191-127.45*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*192-127.46*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*193-127.47*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*194-127.48*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*195-127.49*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*196-127.50*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*197-127.51*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*198-127.52*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*199-127.53*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*200-127.54*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*201-127.55*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*202-127.56*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*203-127.57*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*204-127.58*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*205-127.59*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*206-127.60*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*207-127.61*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*208-127.62*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*209-127.63*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"},
		/*210-127.64*/ {ISO_ALPHANUMERIC, 3, 999, "Reserved for national use"}
};
