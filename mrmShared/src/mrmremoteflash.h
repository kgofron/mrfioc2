/*
 * Windows
 * When mrmShared is compiled the class should be set as exported (included in dll).
 * But when for example evgMrm is compiled, class should be set as imported. This means that evgMrm uses the previously exported class from the mrmShared dll.
 */
#ifdef MRMREMOTEFLASH_H_LEVEL2
 #ifdef epicsExportSharedSymbols
  #define MRMREMOTEFLASH_H_epicsExportSharedSymbols
  #undef epicsExportSharedSymbols
  #include "shareLib.h"
 #endif
#endif

#ifndef MRMREMOTEFLASH_H
#define MRMREMOTEFLASH_H

#include <epicsTypes.h>
#include <epicsThread.h>
#include <epicsMutex.h>
#include "mrf/object.h"


class epicsShareClass mrmRemoteFlash : public mrf::ObjectInst<mrmRemoteFlash>
{
public:
    mrmRemoteFlash(const std::string& name, volatile epicsUInt8* pReg);

    /* locking done internally */
    virtual void lock() const{}
    virtual void unlock() const{}

    void setFlashFilename(std::string filename);
    std::string getFlashFilename() const;

    void setFlashFilenameWF(const char* wf,epicsUInt32 l);
    epicsUInt32 getFlashFilenameWF(char* wf,epicsUInt32 l) const;

    void startFlash(bool start);
    bool flashInProgress() const;
    bool flashSuccess() const;

    //Used by flashing thread
    bool m_flash_in_progress;
    volatile epicsUInt8* const m_pReg; //card reg map

private:
    std::string m_filename;
    bool m_file_valid;

    epicsThreadId m_flash_thread_id;

};

#endif // MRMREMOTEFLASH_H

#ifdef MRMREMOTEFLASH_H_epicsExportSharedSymbols
 #undef MRMREMOTEFLASH_H_LEVEL2
 #define epicsExportSharedSymbols
 #include "shareLib.h"
#endif