/*************************************************************************\
* Copyright (c) 2010 Brookhaven Science Associates, as Operator of
*     Brookhaven National Laboratory.
* mrfioc2 is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/
/*
 * Author: Michael Davidsaver <mdavidsaver@bnl.gov>
 */

#ifndef EVR_HPP_INC
#define EVR_HPP_INC

#include "evr/util.h"
#include "evr/output.h"
#include "mrf/object.h"

#include <epicsTypes.h>
#include <epicsTime.h>
#include <callback.h>

#include "mrmShared.h"

class epicsShareClass Pulser;
class epicsShareClass Output;
class epicsShareClass PreScaler;
class epicsShareClass Input;
class epicsShareClass CML;
class epicsShareClass DelayModuleEvr;

enum TSSource {
  TSSourceInternal=0,
  TSSourceEvent=1,
  TSSourceDBus4=2
};

/**@brief Base interface for EVRs.
 *
 * This is the interface which the generic EVR device support
 * will use to interact with an EVR.
 *
 * Device support can use one of the functions returning IOSCANPVT
 * to impliment get_ioint_info().
 */
class epicsShareClass EVR : public mrf::ObjectInst<EVR>
{
public:
  EVR(const std::string& n, bus_configuration& busConfig) : mrf::ObjectInst<EVR>(n), busConfiguration(busConfig) {}

  virtual ~EVR()=0;

  //! Hardware model
  virtual epicsUInt32 model() const=0;

  //! Firmware Version
  virtual epicsUInt32 version() const=0;
  //! Software Version
  virtual std::string versionSw() const;

  //! Position of EVR device in enclosure.
  virtual std::string position() const;
  bus_configuration* getBusConfiguration();

  /**\defgroup ena Enable/disable pulser output.
   */
  /*@{*/
  virtual bool enabled() const=0;
  virtual void enable(bool)=0;
  /*@}*/

  //! Pulser id number is device specific
  virtual Pulser* pulser(epicsUInt32)=0;
  virtual const Pulser* pulser(epicsUInt32) const=0;

  //! Output id number is device specific
  virtual Output* output(OutputType otype,epicsUInt32 idx)=0;
  virtual const Output* output(OutputType,epicsUInt32) const=0;

  //! Input id number is device specific
  virtual Input* input(epicsUInt32 idx)=0;
  virtual const Input* input(epicsUInt32) const=0;

  //! Prescaler id number is device specific
  virtual PreScaler* prescaler(epicsUInt32)=0;
  virtual const PreScaler* prescaler(epicsUInt32) const=0;

  //! CML Output id number is device specific
  virtual CML* cml(epicsUInt32 idx)=0;
  virtual const CML* cml(epicsUInt32) const=0;

  /** Hook to handle general event mapping table manipulation.
   *  Allows 'special' events only (ie heartbeat, log, led, etc)
   *  Normal mappings (pulsers, outputs) must be made through the
   *  appropriate class (Pulser, Output).
   *
   * Note: this is one place where Device Support will have some depth.
   */
  virtual bool specialMapped(epicsUInt32 code, epicsUInt32 func) const=0;
  virtual void specialSetMap(epicsUInt32 code, epicsUInt32 func,bool set)=0;

  /**\defgroup dc Delay compensation.
    */
   /*@{*/
   virtual void setDelayCompensationEnabled(bool enabled) = 0;
   virtual bool isDelayCompensationEnabled() const = 0;
   virtual epicsUInt32 delayCompensationTarget() const = 0;
   virtual void setDelayCompensationTarget(epicsUInt32 target) = 0;
   virtual epicsUInt32 delayCompensationRxValue() const = 0;
   virtual epicsUInt32 delayCompensationIntValue() const = 0;
   virtual epicsUInt32 delayCompensationStatus() const = 0;
   /*@}*/

  /**\defgroup pll Module reference clock
   *
   *  Controls the local oscillator for the event
   *  clock.
   */
  /*@{*/

  /**Find current LO settings
   *@returns Clock rate in Hz
   */
  virtual double clock() const=0;
  /**Set LO frequency
   *@param clk Clock rate in Hz
   */
  virtual void clockSet(double clk)=0;

  //! Internal PLL Status and bandwidth
  virtual bool pllLocked() const=0;
  virtual void setPllBandwidth(PLLBandwidth pllBandwidth) = 0;
  virtual PLLBandwidth pllBandwidth() const = 0;

  //! Approximate divider from event clock period to 1us
  virtual epicsUInt32 uSecDiv() const=0;
  /*@}*/

  //! Using external hardware input for inhibit?
  virtual bool extInhib() const=0;
  virtual void setExtInhib(bool)=0;

  /**\defgroup ts Time Stamp
   *
   * Configuration and access to the hardware timestamp
   */
  /*@{*/
  //!Select source which increments TS counter
  virtual void setSourceTS(TSSource)=0;
  virtual TSSource SourceTS() const=0;

  /**Find current TS settings
   *@returns Clock rate in Hz
   */
  virtual double clockTS() const=0;
  /**Set TS frequency
   *@param clk Clock rate in Hz
   */
  virtual void clockTSSet(double)=0;

  //!When using internal TS source gives the divider from event clock period to TS period
  virtual epicsUInt32 tsDiv() const=0;

  /** Indicate (lack of) interest in a particular event code.
   *  This allows an EVR to ignore event codes which are not needed.
   */
  virtual bool interestedInEvent(epicsUInt32 event,bool set)=0;


  virtual bool TimeStampValid() const=0;
  virtual IOSCANPVT TimeStampValidEvent() const=0;

  /** Gives the current time stamp as sec+nsec
   *@param ts This pointer will be filled in with the current time
   *@param event N<=0 Return the current wall clock time
   *@param event N>0  Return the time the most recent event # N was received.
   *@return true When ts was updated
   *@return false When ts could not be updated
   */
  virtual bool getTimeStamp(epicsTimeStamp *ts,epicsUInt32 event)=0;

  /** Returns the current value of the Timestamp Event Counter
   *@param tks Pointer to be filled with the counter value
   *@return false if the counter value is not valid
   */
  virtual bool getTicks(epicsUInt32 *tks)=0;

  virtual IOSCANPVT eventOccurred(epicsUInt32 event) const=0;

  typedef void (*eventCallback)(void* userarg, epicsUInt32 event);
  virtual void eventNotifyAdd(epicsUInt32 event, eventCallback, void*)=0;
  virtual void eventNotifyDel(epicsUInt32 event, eventCallback, void*)=0;
  /*@}*/

  virtual epicsUInt32 irqCount() const=0;

  /**\defgroup linksts Event Link Status
   */
  /*@{*/
  virtual bool linkStatus() const=0;
  virtual IOSCANPVT linkChanged() const=0;
  virtual epicsUInt32 recvErrorCount() const=0;
  /*@}*/

  virtual epicsUInt16 dbus() const=0;
  virtual epicsUInt32 dbusToPulserMapping(epicsUInt8 dbus) const = 0;
  virtual void setDbusToPulserMapping(epicsUInt8 dbus, epicsUInt32 pulsers) = 0;

  virtual epicsUInt32 heartbeatTIMOCount() const=0;
  virtual IOSCANPVT heartbeatTIMOOccured() const=0;

  virtual epicsUInt32 FIFOFullCount() const=0;
  virtual epicsUInt32 FIFOOverRate() const=0;
  virtual epicsUInt32 FIFOEvtCount() const=0;
  virtual epicsUInt32 FIFOLoopCount() const=0;


  /**\defgroup devhelp Device Support Helpers
   *
   * These functions exists to make life easier for device support
   */
  /*@{*/
  void setSourceTSraw(epicsUInt32 r){setSourceTS((TSSource)r);}
  epicsUInt32 SourceTSraw() const{return (epicsUInt32)SourceTS();}

  void setPllBandwidthRaw(epicsUInt16 r){setPllBandwidth((PLLBandwidth)r);}
  epicsUInt16 pllBandwidthRaw() const{return (epicsUInt16)pllBandwidth();}

  void setDbusToPulserMapping0(epicsUInt32 pulsers){setDbusToPulserMapping(0, pulsers);}
  epicsUInt32 dbusToPulserMapping0() const{return dbusToPulserMapping(0);}
  void setDbusToPulserMapping1(epicsUInt32 pulsers){setDbusToPulserMapping(1, pulsers);}
  epicsUInt32 dbusToPulserMapping1() const{return dbusToPulserMapping(1);}
  void setDbusToPulserMapping2(epicsUInt32 pulsers){setDbusToPulserMapping(2, pulsers);}
  epicsUInt32 dbusToPulserMapping2() const{return dbusToPulserMapping(2);}
  void setDbusToPulserMapping3(epicsUInt32 pulsers){setDbusToPulserMapping(3, pulsers);}
  epicsUInt32 dbusToPulserMapping3() const{return dbusToPulserMapping(3);}
  void setDbusToPulserMapping4(epicsUInt32 pulsers){setDbusToPulserMapping(4, pulsers);}
  epicsUInt32 dbusToPulserMapping4() const{return dbusToPulserMapping(4);}
  void setDbusToPulserMapping5(epicsUInt32 pulsers){setDbusToPulserMapping(5, pulsers);}
  epicsUInt32 dbusToPulserMapping5() const{return dbusToPulserMapping(5);}
  void setDbusToPulserMapping6(epicsUInt32 pulsers){setDbusToPulserMapping(6, pulsers);}
  epicsUInt32 dbusToPulserMapping6() const{return dbusToPulserMapping(6);}
  void setDbusToPulserMapping7(epicsUInt32 pulsers){setDbusToPulserMapping(7, pulsers);}
  epicsUInt32 dbusToPulserMapping7() const{return dbusToPulserMapping(7);}

  /*@}*/

private:
  bus_configuration busConfiguration;
}; // class EVR

#endif // EVR_HPP_INC
