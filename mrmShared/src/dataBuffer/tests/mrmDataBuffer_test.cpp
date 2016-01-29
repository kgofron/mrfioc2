#include "iocsh.h"
#include "epicsExport.h"

#include "evgMrm.h"
#include "evrMrm.h"

#include "time.h"

#include "mrmDataBufferUser.h"


mrmDataBuffer* getDataBufferFromDevice(char *device) {
    evgMrm* evg = dynamic_cast<evgMrm*>(mrf::Object::getObject(device));
    if(evg){
        return evg->getDataBuffer();
    } else {
        EVRMRM* evr = dynamic_cast<EVRMRM*>(mrf::Object::getObject(device));
        if(evr){
            return evr->getDataBuffer();
        }
    }

    return NULL;
}

/********** Put to data buffer  *******/
// Puts the data to internal buffer, but does not send it.
// Use mrmDataBufferSend to flush the buffer.

static const iocshArg mrmDataBufferPutArg0 = { "Device", iocshArgString };
static const iocshArg mrmDataBufferPutArg1 = { "Offset", iocshArgInt };
static const iocshArg mrmDataBufferPutArg2 = { "Length", iocshArgInt };
static const iocshArg mrmDataBufferPutArg3 = { "Value", iocshArgInt };

static const iocshArg * const mrmDataBufferPutArgs[4] = { &mrmDataBufferPutArg0, &mrmDataBufferPutArg1, &mrmDataBufferPutArg2, &mrmDataBufferPutArg3};
static const iocshFuncDef mrmDataBufferDef_put = { "mrmDataBufferPut", 4, mrmDataBufferPutArgs };

epicsUInt8 data[0x0007ff] = { 0 };
static void mrmDataBufferFunc_put(const iocshArgBuf *args) {
   epicsInt32 offset = args[1].ival;
   epicsInt32 length = args[2].ival;
   epicsInt32 value = args[3].ival;
   epicsInt32 i;

   mrmDataBuffer *dataBuffer = getDataBufferFromDevice(args[0].sval);
   if(dataBuffer == NULL) {
       printf("Data buffer for %s not found.\n", args[0].sval);
       return;
   }

   if(offset < 0) { // reset entire buffer
       for(i=0; i<0x0007ff; i++) {
           data[i] = 0;
       }
   } else {
       for(i=offset; i<offset + length; i++) {
           data[i] = value;
       }
   }
}

/******************/

/********** Send data buffer  *******/
// Sends out the buffer that was previously filled with mrmDataBufferPut

static const iocshArg mrmDataBufferSendArg0 = { "Device", iocshArgString };
static const iocshArg mrmDataBufferSendArg1 = { "Offset", iocshArgInt };
static const iocshArg mrmDataBufferSendArg2 = { "Length", iocshArgInt };

static const iocshArg * const mrmDataBufferSendArgs[3] = { &mrmDataBufferSendArg0, &mrmDataBufferSendArg1, &mrmDataBufferSendArg2};
static const iocshFuncDef mrmDataBufferDef_send = { "mrmDataBufferSend", 3, mrmDataBufferSendArgs };

static void mrmDataBufferFunc_send(const iocshArgBuf *args) {
   epicsInt32 offset = args[1].ival;
   epicsInt32 length = args[2].ival;

   mrmDataBuffer *dataBuffer = getDataBufferFromDevice(args[0].sval);
   if(dataBuffer == NULL) {
       printf("Data buffer for %s not found.\n", args[0].sval);
       return;
   }

   if(offset<0){ // send all zeroes
       epicsUInt8 zeroData[0x0007ff] = { 0 };
       dataBuffer->send(0, 0x0007fc, zeroData);
   } else {
       length += offset - ((offset / 16) * 16);
       printf("Sending using segment %d and length %d\n", offset / 16, length);
       dataBuffer->send((epicsUInt8)offset / 16, (epicsUInt16)length, &data[(offset / 16) * 16]);
   }
}

/******************/

/********** Read data buffer  *******/
// Reads the latest received data buffer

static const iocshArg mrmDataBufferArg0_read = { "Device", iocshArgString };
static const iocshArg mrmDataBufferArg1_read = { "offset", iocshArgInt };
static const iocshArg mrmDataBufferArg2_read = { "length", iocshArgInt };

static const iocshArg * const mrmDataBufferArgs_read[3] = { &mrmDataBufferArg0_read, &mrmDataBufferArg1_read, &mrmDataBufferArg2_read};
static const iocshFuncDef mrmDataBufferDef_read = { "mrmDataBufferRead", 3, mrmDataBufferArgs_read };


static void mrmDataBufferFunc_read(const iocshArgBuf *args) {

    mrmDataBuffer *dataBuffer = getDataBufferFromDevice(args[0].sval);
    if(dataBuffer == NULL) {
       printf("Data buffer for %s not found.\n", args[0].sval);
       return;
    }

    dataBuffer->read(args[1].ival, args[2].ival);
}

/******************/

/********** Enable / disable Rx IRQ  *******/
// Sets first, second, third or forth 32bits of the segment IRQ register

static const iocshArg mrmDataBufferArg0_IRQ = { "Device", iocshArgString };
static const iocshArg mrmDataBufferArg1_IRQ = { "segments", iocshArgInt }; // 0 - 4
static const iocshArg mrmDataBufferArg2_IRQ = { "mask", iocshArgInt }; // mask to apply

static const iocshArg * const mrmDataBufferArgs_IRQ[3] = { &mrmDataBufferArg0_IRQ, &mrmDataBufferArg1_IRQ, &mrmDataBufferArg2_IRQ};
static const iocshFuncDef mrmDataBufferDef_IRQ = { "mrmDataBufferIRQ", 3, mrmDataBufferArgs_IRQ };


static void mrmDataBufferFunc_IRQ(const iocshArgBuf *args) {
    epicsUInt32 i = args[1].ival;
    epicsUInt32 mask = args[2].ival;

    mrmDataBuffer *dataBuffer = getDataBufferFromDevice(args[0].sval);
    if(dataBuffer == NULL) {
       printf("Data buffer for %s not found.\n", args[0].sval);
       return;
    }

    dataBuffer->setSegmentIRQ(i, mask);
}

/******************/

/********** Enable / disable Rx flags  *******/
// Sets first, second, third or forth 32bits of the segment Rx register

static const iocshArg mrmDataBufferArg0_Rx = { "Device", iocshArgString };
static const iocshArg mrmDataBufferArg1_Rx = { "segmens", iocshArgInt }; // 0 - 4
static const iocshArg mrmDataBufferArg2_Rx = { "mask", iocshArgInt }; // mask to apply

static const iocshArg * const mrmDataBufferArgs_Rx[3] = { &mrmDataBufferArg0_Rx, &mrmDataBufferArg1_Rx, &mrmDataBufferArg2_Rx};
static const iocshFuncDef mrmDataBufferDef_Rx = { "mrmDataBufferRx", 3, mrmDataBufferArgs_Rx };


static void mrmDataBufferFunc_Rx(const iocshArgBuf *args) {
    epicsUInt32 i = args[1].ival;
    epicsUInt32 mask = args[2].ival;

    mrmDataBuffer *dataBuffer = getDataBufferFromDevice(args[0].sval);
    if(dataBuffer == NULL) {
       printf("Data buffer for %s not found.\n", args[0].sval);
       return;
    }

    dataBuffer->setRx(i, mask);
}

/******************/

/********** Rx stop  *******/
// Writes the DBDIS bit to 1

static const iocshArg mrmDataBufferArg0_stop = { "Device", iocshArgString };

static const iocshArg * const mrmDataBufferArgs_stop[1] = { &mrmDataBufferArg0_stop};
static const iocshFuncDef mrmDataBufferDef_stop = { "mrmDataBufferStop", 1, mrmDataBufferArgs_stop };


static void mrmDataBufferFunc_stop(const iocshArgBuf *args) {

    mrmDataBuffer *dataBuffer = getDataBufferFromDevice(args[0].sval);
    if(dataBuffer == NULL) {
       printf("Data buffer for %s not found.\n", args[0].sval);
       return;
    }

    dataBuffer->stop();
}

/******************/

/********** Print IRQ, checksum, overflow and Rx registers  *******/
static const iocshArg mrmDataBufferArg0_print = { "Device", iocshArgString };

static const iocshArg * const mrmDataBufferArgs_print[1] = { &mrmDataBufferArg0_print};
static const iocshFuncDef mrmDataBufferDef_print = { "mrmDataBufferPrint", 1, mrmDataBufferArgs_print };


static void mrmDataBufferFunc_print(const iocshArgBuf *args) {

    mrmDataBuffer *dataBuffer = getDataBufferFromDevice(args[0].sval);
    if(dataBuffer == NULL) {
       printf("Data buffer for %s not found.\n", args[0].sval);
       return;
    }

    dataBuffer->printRegs();
}

/******************/

/********** Send and receive on an offset  *******/
static const iocshArg mrmDataBufferArg0_user = { "Offset", iocshArgInt };

static const iocshArg * const mrmDataBufferArgs_user[1] = { &mrmDataBufferArg0_user};
static const iocshFuncDef mrmDataBufferDef_user = { "mrmDataBufferUser", 1, mrmDataBufferArgs_user };

//bool done;
struct cbpvt{
    mrmDataBufferUser *rx;
    size_t offset;
} cb_pvt;

void callback(size_t updated_offset, size_t length, void* pvt){
    cbpvt *str = (cbpvt *)pvt;
    mrmDataBufferUser *rx = (*str).rx;
    size_t offset = (*str).offset;
    epicsUInt8 buffer[3000];

    printf("Got update on offset %" FORMAT_SIZET_U "+%" FORMAT_SIZET_U "\n", updated_offset, length);
    rx->get(offset, length, (void *)&buffer);
    for(unsigned int i=0; i<length; i++){
        printf("%d, ", buffer[i]);
    }
    printf("\n");
    //done=true;
}

static void mrmDataBufferFunc_user(const iocshArgBuf *args) {
    mrmDataBufferUser *rx = new mrmDataBufferUser();
    mrmDataBufferUser *tx = new mrmDataBufferUser();
    struct timespec t;
    t.tv_nsec = 0000;
    t.tv_sec = 2;
    size_t offset = args[0].ival;
    int idx;

    if (!rx->init("EVR0")) {
        printf("Failed to initialize EVR0 data buffer\n");
        delete rx;
        return;
    }

    if (!tx->init("EVG0")) {
        printf("Failed to initialize EVG0 data buffer\n");
        delete tx;
        return;
    }


    epicsUInt8 data[20] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    //done = false;

    cb_pvt.offset = offset;
    cb_pvt.rx = rx;
    idx = rx->registerInterest(offset, 20, callback, &cb_pvt);

    tx->put(offset, 20, (void *)data);
    tx->send(true);
    printf("sleeping...");
    nanosleep(&t,&t);
    printf("DONE!\n");

    rx->removeInterest(idx);


    printf("interest removed\n");

    delete tx;
    printf("Deleted tx\n");
    delete rx;

    printf("Deleted rx tx\n");
}

/******************/

/********** Handle interest  *******/
static const iocshArg mrmDataBufferArg0_interest = { "User", iocshArgInt };
static const iocshArg mrmDataBufferArg1_interest = { "Offset", iocshArgInt };

static const iocshArg * const mrmDataBufferArgs_interest[2] = { &mrmDataBufferArg0_interest, &mrmDataBufferArg1_interest};
static const iocshFuncDef mrmDataBufferDef_initInterest = { "mrmDataBufferInitInterest", 2, mrmDataBufferArgs_interest };
static const iocshFuncDef mrmDataBufferDef_addInterest = { "mrmDataBufferAddInterest", 2, mrmDataBufferArgs_interest };
static const iocshFuncDef mrmDataBufferDef_removeInterest = { "mrmDataBufferRemoveInterest", 2, mrmDataBufferArgs_interest };

struct usr{
    mrmDataBufferUser *user;
    int id;
} users[5];

static void mrmDataBufferFunc_initInterest(const iocshArgBuf *args) {
    int i;

    for (i=0; i<5; i++) {
        users[i].user = new mrmDataBufferUser();
        if (users[i].user->init("EVR0")) {
            printf("Failed to initialize data buffer\n");
            return;
        }
    }
}

static void mrmDataBufferFunc_addInterest(const iocshArgBuf *args) {
    int uid = args[0].ival;
    size_t offset = args[1].ival;

    if(uid<0 || uid>4) return;

    users[uid].id = users[uid].user->registerInterest(offset, 20, callback, &users[uid].user);
}

static void mrmDataBufferFunc_removeInterest(const iocshArgBuf *args) {
    int uid = args[0].ival;

    if(uid<0 || uid>4) return;
    users[uid].user->removeInterest(users[uid].id);
}

/******************/


extern "C" {
    static void mrmDataBufferRegistrar() {
        iocshRegister(&mrmDataBufferDef_send, mrmDataBufferFunc_send);
        iocshRegister(&mrmDataBufferDef_put, mrmDataBufferFunc_put);
        iocshRegister(&mrmDataBufferDef_read, mrmDataBufferFunc_read);
        iocshRegister(&mrmDataBufferDef_IRQ, mrmDataBufferFunc_IRQ);
        iocshRegister(&mrmDataBufferDef_Rx, mrmDataBufferFunc_Rx);
        iocshRegister(&mrmDataBufferDef_user, mrmDataBufferFunc_user);
        iocshRegister(&mrmDataBufferDef_stop, mrmDataBufferFunc_stop);
        iocshRegister(&mrmDataBufferDef_print, mrmDataBufferFunc_print);
        iocshRegister(&mrmDataBufferDef_initInterest, mrmDataBufferFunc_initInterest);
        iocshRegister(&mrmDataBufferDef_addInterest, mrmDataBufferFunc_addInterest);
        iocshRegister(&mrmDataBufferDef_removeInterest, mrmDataBufferFunc_removeInterest);
    }

    epicsExportRegistrar(mrmDataBufferRegistrar);
}
