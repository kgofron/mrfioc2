#ifndef MRMDATABUFFERUSER_H
#define MRMDATABUFFERUSER_H

#include <vector>
#include <epicsTypes.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsThread.h>

#include "mrmDataBufferType.h"

#ifdef _WIN32
/**
 * Removes warning on windows for m_rx_callbacks: needs to have dll-interface to be used by clients of class 'mrmDataBuffer'
 * This is safe, because m_rx_callbacks is not used outside the boundary of the DLL file.
 */
#pragma warning( disable: 4251 )
#endif



class mrmDataBuffer;    // forward decleration in order to avoid dependancy on mrmDataBuffer.h file when using this class


typedef void(*dataBufferRxCallback_t)(size_t updated_offset, size_t length, void* pvt);


/**
 * @brief The mrmDataBufferUser class
 *
 * Interface towards MRF EVM and EVR data buffer
 *
 * Each user of the data buffer creates 1 instance of this class. On every segment update the data is
 * copied into all registered mrmDataBufferUser instances. Each instance is than responsible for invoking
 * its own callbacks, which users can subscribe to.
 *
 * Note that if users callbacks (registerd via registerInterest function) are slow (slower than updates), data will be lost!
 *
 */
class epicsShareClass mrmDataBufferUser {
public:
    mrmDataBufferUser();
    ~mrmDataBufferUser();

    /**
     * @brief init will connect to the data buffer based on device name. Throws exception if it does not succeed.
     * @param deviceName is the name of the device which holds the data buffer (eg. EVR0, EVG0, ...)
     * @param userOffset sets m_user_offset. When not provided it defaults to 0.
     * @param strictMode sets m_strict_mode. When not provided it defaults to false.
     * @param userUpdateThreadPriority sets the priority at which the user update thread will run. Defaults to epicsThreadPriorityLow.
     * @return true on success, false otherwise
     */
    bool init(const char* deviceName, mrmDataBufferType::type_t type, size_t userOffset = 0, bool strictMode = false, unsigned int userUpdateThreadPriority = epicsThreadPriorityLow);

    /**
     * @brief registerInterest Register callback for part (or whole) data buffer.
     * @param offset is the starting offset to register to
     * @param length is the length from the offset to register to
     * @param fptr is the callback function to invoke when data buffer on registered addresses is updated
     * @param pvt is pointer to the callback function private structure
     * @param the ID of the iterest we want to update, or 0 for new registation
     * @return the ID of the registered interest or 0 on error
     */
    size_t registerInterest(size_t offset, size_t length, dataBufferRxCallback_t fptr, void* pvt, size_t interestId=0);

    /**
     * @brief removeInterest Remove previously registred interest
     * @param id is the ID of the previously registered interest
     * @return true on success, false otherwise
     */
    bool removeInterest(size_t id);

    /**
     * @brief put data to buffer, but do not send it yet
     * @param offset location in data buffer where the new data should be placed
     * @param length length of new data
     * @param buffer pointer to new data
     */
    void put(size_t offset, size_t length, void* buffer);

    /**
     * @brief get retrive data by copying it into a destination buffer
     * User has to ensure that size of dest >= length
     * @param offset Start location
     * @param length to get from the starting location
     * @param buffer pointer to a destination buffer to hold the data
     */
    void get(size_t offset, size_t length, void *buffer);

    /**
     * @brief send the updated segments of data buffer that were set via put();
     * If the send operation is already in progress the function will block untill data can be sent.
     * If wait is true the function will block until the segment was actually sent by MRF HW.
     * @return true on success, false otherwise
     */
    bool send(bool wait);

    /**
     * @brief updateSegment is called from the underlying data buffer class when data is received.
     * @param segment is the first segment that was updated
     * @param data is the updated data
     * @param length is the length of the updated data from the segment offset
     */
    void updateSegment(epicsUInt16 segment, epicsUInt8* data, epicsUInt16 length);

    /**
     * @brief supportsRx Checks if the underlying data buffer supports reception.
     * @return True if the data buffer supports reception, false otherwise. It also returns false if receive mechanisms (eg. update thread) could not be initialized.
     */
    bool supportsRx();

    /**
     * @brief supportsTx Checks if the underlying data buffer supports transmission
     * @return True if the data buffer supports transmission, false otherwise
     */
    bool supportsTx();

    /**
     * @brief getMaxLength Returns the maximum length of the buffer based on user offset.
     * @return the capacity of the data buffer
     */
    size_t getMaxLength();

    /**
     * @brief requestTxBuffer opens direct access to the user transmit buffer. Great care must be taken when using this function, since it locks the buffer transmission until releaseTxBuffer() is called. User must also make sure that the offset and length to be written to are inside the allowed buffer length (can be checked using getMaxLength()).
     * @return a pointer to the start + user offset of the underlying transmit buffer.
     */
    epicsUInt8* requestTxBuffer();

    /**
     * @brief releaseTxBuffer closes the access to the user transmit buffer, thus releasing it to other users. It also marks the data to be send out based on the provided offset and length.
     * @param offset is the starting offset where the data was written.
     * @param length is the length of the written data.
     */
    void releaseTxBuffer(size_t offset, size_t length);

    /**
     * @brief requestRxBuffer opens direct access to the user receive buffer. Great care must be taken when using this function, since it locks the buffer reception until releaseRxBuffer() is called. User must also make sure that the offset and length to be read from are inside the allowed buffer length (can be checked using getMaxLength()).
     * @return a pointer to the start + user offset of the underlying receive buffer.
     */
    epicsUInt8* requestRxBuffer();

    /**
     * @brief releaseRxBuffer closes the access to the user receive buffer, thus releasing it to other users and to data buffer reception.
     */
    void releaseRxBuffer();
private:
    epicsUInt8 m_rx_buff[2048];    // A copy of the received data
    epicsUInt8 m_tx_buff[2048];    // A copy of the data to be send out

    epicsUInt32 m_tx_segments[4];  // Segment mask for updated segments,  e.g. segments that are updated in a local buffer but were not sent out yet
    epicsUInt32 m_rx_segments[4];  // Segment mask for updates that were received but not yet dispatched
    epicsMutex m_tx_lock;          // Protects race condition betwen put and send
    epicsMutex m_rx_lock;          // Protects race condition between level 1 callback and consumer + protects access to rx buffer, rx segments and rx callbacks.

    mrmDataBuffer *m_data_buffer;  // Reference to the underlying data buffer

    epicsThreadId m_thread_id;      // the ID of the user update thread
    bool m_thread_stop;             // used to signal the user update thread that it should exit
    epicsEventId m_thread_stopped;  // used to signal when the user update thread exited
    epicsEventId m_thread_sync;     // used for synchronisation between updateSegment and updateUserThread


    //Registered callbacks
    struct RxCallback{
        size_t id;                              // the id of the registered callback. Used for deletion.
        epicsUInt32 segments[4];                // segment mask in which the user is interested
        dataBufferRxCallback_t fptr;            // callback function pointer
        void* pvt;                              // callback private
    };
    std::vector<RxCallback*> m_rx_callbacks;    // a list of registered users and their segments of interest (offset + length)
    epicsUInt32 m_segments_interested[4];       // global segment mask in which users are interested in

    size_t m_user_offset;                       // user offset + offset of the calling function determine the actuall data buffer offset.
    bool m_strict_mode;                         // When in strict mode, updateSegment function uses 'lock' instead of 'tryLock'. When using strict mode, sser must ensure that the buffer operations are fast and non-blocking, sice they affect all users.


    /**
     * @brief userUpdateThread is the thread responsible for informing users (m_rx_callbacks) of new data received based on their registered interest
     * @param args are the arguments passed to the thread
     */
    static void userUpdateThread(void *args);
};

#endif // MRMDATABUFFERUSER_H
