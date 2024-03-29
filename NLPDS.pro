######################################################################
# Automatically generated by qmake (2.01a) Tue Dec 19 13:24:58 2017
######################################################################

TEMPLATE = app

TARGET = bin/commonpds

QT       += core gui
QT       += sql
QT       += xml

OBJECTS_DIR = obj
MOC_DIR = moc

PRECOMPILED_HEADER =include/compheader.h
CONFIG +=PRECOMPILED_HEADER

#LIBS += -L /usr/local/IMX6-Qt-4.7.4/Qt-X11-ARM-Target/lib -lX11 -L./lib -lgl11pos -lpcsclite -lkf311 -lcurl -lssl -lcrypto -ltfm -lusb-0.1 -lxml2 -lbluetooth -lBiomatiquesApi -lBiomatiquesSDK -L./usr/lib/ -lm
LIBS += -L /usr/local/IMX6-Qt-4.7.4/Qt-X11-ARM-Target/lib -lX11 -L./lib -lgl11pos -lpcsclite -lkf311 -lcurl -lssl -lcrypto -ltfm -lusb-0.1 -lxml2 -lbluetooth

LIBS +=-L./lib -lauth_app
LIBS +=-L./lib -lIritech

DEPENDPATH += . \
              forms \
              include \
              intermediate \
              src
INCLUDEPATH += . \
               include \


# Input
HEADERS += include/2dimager.h \
           include/bclib.h \
           include/bil_pds_eod.pb-c.h \
           include/bio_Iris_api.h \
           include/Biomatiques.h \
           include/bluetooth.h \
           include/bnep.h \
           include/CHeader.h \
           include/cmtp.h \
           include/create_bmp.h \
           include/data.h \
           include/database.h \
           include/defines.h \
           include/ejLog.h \
           include/EPI1000.h \
           include/ethernet.h \
           include/gl11.h \
           include/gprs.h \
           include/gsm_modem.h \
           include/hci.h \
           include/hci_lib.h \
           include/hidp.h \
           include/http_client.h \
           include/Iddk2000_features.h \
           include/Iddk2000_main.h \
           include/Iddk2000_utils.h \
           include/Iddk2000Apis.h \
           include/Iddk2000Defs.h \
           include/IddkBase.h \
           include/iris.h \
           include/iris_biometric_api.h \
           include/IrisCapture.h \
           include/iso8583.h \
           include/isodef.h \
           include/l2cap.h \
           include/logo_print.h \
           include/misc.h \
           include/modem.h \
           include/parse.h \
           include/parsing.h \
           include/popup_msg.h \
           include/popup_nothing.h \
           include/printer.h \
           include/protobuf-c-private.h \
           include/protobuf-c.h \
           include/rfcomm.h \
           include/sco.h \
           include/sdp.h \
           include/sdp_lib.h \
           include/tfmapi.h \
           include/tfmerror.h \
           include/tfmtypes.h \
           include/ui_BFDwidget.h \
           include/ui_mobileupdatedialog.h \
           include/ui_processingdlg.h \
           include/uid_auth.h \
           include/usb.h \
           include/widget.h \
           include/compheader.h \
           include/database_queries.h \
    include/BiomatiquesRDSClient.h \
    libxml/xpointer.h \
    libxml/xpathInternals.h \
    libxml/xpath.h \
    libxml/xmlwriter.h \
    libxml/xmlversion.h.in \
    libxml/xmlversion.h \
    libxml/xmlunicode.h \
    libxml/xmlstring.h \
    libxml/xmlschemastypes.h \
    libxml/xmlschemas.h \
    libxml/xmlsave.h \
    libxml/xmlregexp.h \
    libxml/xmlreader.h \
    libxml/xmlmodule.h \
    libxml/xmlmemory.h \
    libxml/xmlIO.h \
    libxml/xmlexports.h \
    libxml/xmlerror.h \
    libxml/xmlautomata.h \
    libxml/xlink.h \
    libxml/xinclude.h \
    libxml/valid.h \
    libxml/uri.h \
    libxml/tree.h \
    libxml/threads.h \
    libxml/schematron.h \
    libxml/schemasInternals.h \
    libxml/SAX2.h \
    libxml/SAX.h \
    libxml/relaxng.h \
    libxml/pattern.h \
    libxml/parserInternals.h \
    libxml/parser.h \
    libxml/nanohttp.h \
    libxml/nanoftp.h \
    libxml/list.h \
    libxml/HTMLtree.h \
    libxml/HTMLparser.h \
    libxml/hash.h \
    libxml/globals.h \
    libxml/entities.h \
    libxml/encoding.h \
    libxml/DOCBparser.h \
    libxml/dict.h \
    libxml/debugXML.h \
    libxml/chvalid.h \
    libxml/catalog.h \
    libxml/c14n.h \
    include/iritech.h \
    include/json.h \
    include/json_response.h \
    include/json_request.h \
    openssl/x509v3.h \
    openssl/x509.h \
    openssl/x509_vfy.h \
    openssl/whrlpool.h \
    openssl/ui.h \
    openssl/ui_compat.h \
    openssl/txt_db.h \
    openssl/ts.h \
    openssl/tls1.h \
    openssl/symhacks.h \
    openssl/stack.h \
    openssl/ssl23.h \
    openssl/ssl3.h \
    openssl/ssl2.h \
    openssl/ssl.h \
    openssl/srtp.h \
    openssl/srp.h \
    openssl/sha.h \
    openssl/seed.h \
    openssl/safestack.h \
    openssl/rsa.h \
    openssl/ripemd.h \
    openssl/rc4.h \
    openssl/rc2.h \
    openssl/rand.h \
    openssl/pqueue.h \
    openssl/pkcs12.h \
    openssl/pkcs7.h \
    openssl/pem2.h \
    openssl/pem.h \
    openssl/ossl_typ.h \
    openssl/opensslv.h \
    openssl/opensslconf.h \
    openssl/ocsp.h \
    openssl/objects.h \
    openssl/obj_mac.h \
    openssl/modes.h \
    openssl/mdc2.h \
    openssl/md5.h \
    openssl/md4.h \
    openssl/lhash.h \
    openssl/kssl.h \
    openssl/krb5_asn.h \
    openssl/idea.h \
    openssl/hmac.h \
    openssl/evp.h \
    openssl/err.h \
    openssl/engine.h \
    openssl/ecdsa.h \
    openssl/ecdh.h \
    openssl/ec.h \
    openssl/ebcdic.h \
    openssl/e_os2.h \
    openssl/dtls1.h \
    openssl/dso.h \
    openssl/dsa.h \
    openssl/dh.h \
    openssl/des.h \
    openssl/des_old.h \
    openssl/crypto.h \
    openssl/conf.h \
    openssl/conf_api.h \
    openssl/comp.h \
    openssl/cms.h \
    openssl/cmac.h \
    openssl/cast.h \
    openssl/camellia.h \
    openssl/buffer.h \
    openssl/bn.h \
    openssl/blowfish.h \
    openssl/bio.h \
    openssl/asn1t.h \
    openssl/asn1.h \
    openssl/asn1_mac.h \
    openssl/aes.h

FORMS += forms/popup_msg.ui forms/popup_nothing.ui forms/widget.ui
SOURCES += \
           src/bluetooth_api.c \
           src/create_bmp.c \
           src/database.cpp \
           src/ethernet.cpp \
           src/gprs.cpp \
           src/iris.cpp \
           src/logo_print.cpp \
           src/main.cpp \
           src/misc.cpp \
           src/modem.cpp \
           src/parse.cpp \
           src/popup_msg.cpp \
           src/popup_nothing.cpp \
           src/usb.cpp \
           src/widget.cpp \
           src/database_queries.cpp \
    src/Receivegoods.cpp \
    src/jsoncpp.cpp \
    src/json_response.cpp \
    src/json_request.cpp \
    src/BeneficiaryVerify.cpp\
    src/PartialOnline.cpp \
    src/Reports.cpp \
    src/eKYC.cpp \
    src/Inspection.cpp \
    src/logout.cpp \
    src/ReceiveGoodsOffline.cpp \
    src/Mdm.cpp \
    src/encrypt.cpp \
    src/minQty.cpp \
    src/base64decode.cpp

RESOURCES += Resources.qrc
