#ifndef __LOWI_RESPONSE_H__
#define __LOWI_RESPONSE_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Response Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWIResponse

Copyright (c) 2012 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/
#include <inc/lowi_const.h>
#include <inc/lowi_scan_measurement.h>

namespace qc_loc_fw
{

// Forward declaration
class LOWIRequest;

/**
 * Base class for Response
 */
class LOWIResponse
{
private:
  uint32 requestId;
public:
  /**
   * Log Tag
   */
  static const char * const TAG;

  /** Type of Response
   * When adding additional responses, update the following utility function:
   *  -- LOWIUtils::to_string(LOWIResponse::eResponseType a)
   *
   *  ADD NEW RESPONSES ONLY AT THE END OF THIS ENUM
   */
  enum eResponseType
  {
    /** Unknown response type */
    RESPONSE_TYPE_UNKNOWN = 0,
    /** Response carrying the results produced by a DISCOVERY_SCAN request */
    DISCOVERY_SCAN,
    /** Response carrying the results produced by a RANGING_SCAN request */
    RANGING_SCAN,
    /** List of capabilities supported by LOWI */
    CAPABILITY,
    /** Response to a RESET_CACHE request */
    RESET_CACHE,
    /** Response carrying asynchronous results produced by an
     *  ASYNC_DISCOVERY_SCAN_RESULTS request */
    ASYNC_DISCOVERY_SCAN_RESULTS,
    /** Status response */
    LOWI_STATUS,
    /** FW background scan capabilities */
    BGSCAN_CAPABILITIES,
    /** List of frequency channels supported by the WiFi driver */
    BGSCAN_CHANNELS_SUPPORTED,
    /** Asynchronous response event resulting from a
     *  SIGNIFINCANT_CHANGE_LIST_SET request */
    BGSCAN_EVENT,
    /** Response carrying cached results produced a BGSCAN_START request */
    BGSCAN_CACHED_RESULTS,
    /** Asynchronous response event resulting from a HOTLIST_SET request */
    HOTLIST_AP_EVENT,
    /** Asynchronous response event resulting from a
     *  SIGNIFINCANT_CHANGE_LIST_SET request */
    SIGNIFICANT_CHANGE_EVENT,
    /** Response to the capability subscription request, carries dynamic capabilities*/
    CAPABILITY_SUBSCRIPTION,
    /** Response carrying cached results produced a BGSCAN_START request per
     *  new M-release
     */
    BGSCAN_CACHED_RESULTS_VECTOR,
    /** This is the channel response of the responder for the LOWI_ENABLE_RESPONDER
     * request or LOWI_RTT_RM_CHANNEL_REQUEST, this response is used for STA-STA
     * RTT feature
     */
    LOWI_RTT_RM_CHANNEL_RESPONSE,
    /** Response to LOWI_WLAN_STATE_QUERY_REQUEST */
    LOWI_WLAN_STATE_QUERY_RESPONSE
  };

  /**
   * Defines status of the Scan Request.
   */
  enum eScanStatus
  {
    SCAN_STATUS_UNKNOWN = 0,
    /** Measurements were obtained successfully from the WLAN driver.
     * Note that SUCCESS does not guarantee that there is at least one
     * measurement in this packet. It is possible to have zero measurement
     * and a SUCCESS; if there are no APs in the vicinity.*/
    SCAN_STATUS_SUCCESS = 1,
    /** Indicates that the number of pending clients have reached the maximum*/
    SCAN_STATUS_BUSY = 2,
    /** Unable to initiate request to driver*/
    SCAN_STATUS_DRIVER_ERROR = 3,
    /** Unable to get response from driver*/
    SCAN_STATUS_DRIVER_TIMEOUT = 4,
    /** There is an internal error condition that causes
     * LOWI unable to provide any measurements*/
    SCAN_STATUS_INTERNAL_ERROR = 5,
    /** Invalid request*/
    SCAN_STATUS_INVALID_REQ = 6,
    /** Request not supported */
    SCAN_STATUS_NOT_SUPPORTED = 7,
    /** Wifi not enabled */
    SCAN_STATUS_NO_WIFI = 8,
    /** Too many instances of this request type */
    SCAN_STATUS_TOO_MANY_REQUESTS = 9,
    /** Out of memory condition */
    SCAN_STATUS_OUT_OF_MEMORY = 10,
    /** Wigig not enabled */
    SCAN_STATUS_NO_WIGIG = 11
  };

  /**
   * Constructor
   * @param uint32 requestId generated by the client for request
   */
  LOWIResponse (uint32 requestId);
  /**
   * Destructor
   */
  virtual ~LOWIResponse () = 0;

  /**
   * Request id generated and provided in request by the client.
   * Echoed back in the response.
   * @return Corresponding Request id
   */
  uint32 getRequestId ();

  /**
   * Returns the response type
   * @return eResponseType Type of Response
   */
  virtual eResponseType getResponseType () = 0;
};

/**
 * This class defines the Capabilities of the WifiDriver
 */
class LOWICapabilities
{
public:

  /** Log Tag */
  static const char * const TAG;

  /** NOTE **
   *  Except for supportedCapsWigig below, all of the following variables
   *  do NOT apply to wigig driver
   */
  /** true if Discovery scan is supported */
  bool discoveryScanSupported;

  /** true if Ranging scan is supported */
  bool rangingScanSupported;

  /** true if Active scan is supported */
  bool activeScanSupported;

 /** true if single-sided ranging is supported */
  bool oneSidedRangingSupported;

  /** true if dual-sided ranging per 11v std is supported */
  bool dualSidedRangingSupported11v;

  /** true if dual-sided ranging per 11mc std is supported */
  bool dualSidedRangingSupported11mc;

  /** true if bgscan-compatible scan off-loading is supported */
  bool bgscanSupported;

  /** Highest bandwidth support for rtt requests */
  uint8 bwSupport;

  /** Bit mask representing preambles supported for rtt requests */
  uint8 preambleSupport;

  /** Bit mask representing the mc version supported */
  uint8 mcVersion;

  /** Defines for potential capabilities supported by a driver */
  #define LOWI_NO_SCAN_SUPPORTED        0x0000
  #define LOWI_LP_SCAN_SUPPORTED        0x0001 // static capability. driver supports LP scans
  #define LOWI_DISCOVERY_SCAN_SUPPORTED 0x0002 // static capability. driver supports discovery scans
  #define LOWI_RANGING_SCAN_SUPPORTED   0x0004 // static capability. driver supports ranging scans
  #define LOWI_BG_SCAN_SUPPORTED        0x0008 // static capability. LOWI and driver support BG scans
  #define LOWI_FW_MULTIBURST_SUPPORTED  0x0010 // FW supports RTT multi burst measurements
  #define LOWI_RTT_RESPONDER_SUPPORTED  0x0020 // LOWI supports RTT responder mode or not
  #define LOWI_RTT_INITIATOR_SUPPORTED  0x0040 // Driver supports initiator mode or not
  #define LOWI_RTT_ASAP_SUPPORTED       0x0080 // Driver supports ASAP mode or not
  #define LOWI_AOA_MEAS_SUPPORTED       0x0100 // Driver supports AoA stand alone measurements or not
  #define LOWI_AOA_IN_FTM_REQ_SUPPORTED 0x0200 // Driver supports AoA measurements in FTM requests or not
  #define LOWI_AOA_TOP_CIR_PH_SUPPORTED 0x0400 // Driver supports top CIR phase only AoA measurements
  #define LOWI_AOA_TOP_CIR_PH_AMP_SUPPORTED 0x0800 // Driver supports top CIR phase/amplitude AoA measurements

  /** Bitmask representing capabilities supported by the loaded wifi driver.
   *  Defines above are used. */
  uint32 supportedCapablities;

  /** Bitmask representing capabilities supported by the loaded wigig driver.
   *  Defines above are used. */
  uint32 supportedCapsWigig;

public:
  /** Constructor */
  LOWICapabilities ();

  /** Print all capability fields */
  void PrintCapabilities();

  /**
   * Checks whether the capability passed in the argument is supported or not.
   * NOTE:
   * The argument to this function must come from the set of "potential
   * capabilities supported by a driver" as provided by the #define's above.
   *
   * @param uint32: capability to be checked.
   * @return bool: true if capability is supported, false otherwise
   */
  bool isSupportedWigigCap(uint32 cap)
  {
    return (LOWI_NO_SCAN_SUPPORTED == supportedCapsWigig) ? true:(cap & supportedCapsWigig);
  }

  /**
   * Sets the capability provided in the argument into the bitmask
   * "supportedCapsWigig"
   *
   * @param uint32: capability to be set.
   * NOTE:
   * Must come  the set of "potential capabilities supported by a driver" as
   * provided by the #define's above.
   */
  void setSupportedWigigCap(uint32 cap)
  {
    supportedCapsWigig &= ~cap; // clear the corresponding bit
    supportedCapsWigig |=  cap; // set the corresponding bit
  }
};

/**
 * Response to the Capability Request
 */
class LOWICapabilityResponse : public LOWIResponse
{
private:
  LOWICapabilities mCapabilities;
  bool             mStatus;
public:
  /**
   * Constructor
   * @param uint32 requestId generated by the client for request
   * @param LOWICapabilities Capabilities object
   * @param bool true for success, false to indicate failure
   */
  LOWICapabilityResponse (uint32 requestId, LOWICapabilities capabilities, bool status);
  /** Destructor*/
  virtual ~LOWICapabilityResponse ();

  /**
   * Returns the response type
   * @return eResponseType Type of Response
   */
  virtual eResponseType getResponseType ();

  /**
   * Return the capabilities
   * @return LOWICapabilities
   */
  LOWICapabilities getCapabilities ();

  /**
   * Returns the status of the Request
   * @return bool true for success, false otherwise
   */
  bool getStatus ();

};

/**
 * Response to the Cache reset Request
 */
class LOWICacheResetResponse : public LOWIResponse
{
private:
  bool mCacheResetStatus;
public:
  /**
   * Constructor
   * @param uint32 requestId generated by the client for request
   * @param bool true for success, false to indicate failure
   */
  LOWICacheResetResponse (uint32 requestId, bool status);
  /** Destructor*/
  virtual ~LOWICacheResetResponse ();

  /**
   * Returns the response type
   * @return eResponseType Type of Response
   */
  virtual eResponseType getResponseType ();

  /**
   * Returns the status of the Request
   * @return bool true for success, false otherwise
   */
  bool getStatus ();
};

/**
 * Response to the Discovery Scan Request
 */
class LOWIDiscoveryScanResponse : public LOWIResponse
{
public:
  /** Response type*/
  enum eScanTypeResponse
  {
    WLAN_SCAN_TYPE_UNKNOWN = 0,
    WLAN_SCAN_TYPE_PASSIVE,
    WLAN_SCAN_TYPE_ACTIVE
  };

  /** This may be different than the requested ScanType,
   * since user may request active scan, but if there�s an ongoing
   * passive scan results, LOWI may give back those results.*/
  eScanTypeResponse       scanTypeResponse;
  /** Status of the scan */
  eScanStatus             scanStatus;
  /** Time at the end of scan � should be equal to the timestamp of the
   * latest measurement in this packet. If there are zero measurements,
   * timestamp is the time when the response from the driver is received.
   * In msec, since January 1, 1970 UTC
   */
  uint64                   timestamp;
  /** Dynamic array containing received ScanMeasurement*/
  vector <LOWIScanMeasurement*> scanMeasurements;

  /**
   * Constructor
   * @param uint32 Corresponding Request Id generated by the client for request
   */
  LOWIDiscoveryScanResponse (uint32 requestId);
  /** Destructor*/
  virtual ~LOWIDiscoveryScanResponse ();

  /**
   * Returns the response type
   * @return eResponseType type of Response
   */
  virtual eResponseType getResponseType ();
};

/**
 * Response to the Ranging scan Request
 */
class LOWIRangingScanResponse: public LOWIResponse
{
public:
  /** Status of the scan */
  eScanStatus             scanStatus;
  /** Dynamic array containing received ScanMeasurement*/
  vector <LOWIScanMeasurement*> scanMeasurements;

  /**
   * Constructor
   * @param uint32 Request Id generated by the client for Request
   */
  LOWIRangingScanResponse (uint32 requestId);
  /** Destructor*/
  virtual ~LOWIRangingScanResponse ();

  /**
   * Returns the response type
   * @return eResponseType type of Response
   */
  virtual eResponseType getResponseType ();
};

class LOWIAsyncDiscoveryScanResultResponse :
  public LOWIDiscoveryScanResponse
{
public:
    /**
     * Constructor
     * @param uint32 Corresponding Request Id generated
     *               by the client for request
     */
    LOWIAsyncDiscoveryScanResultResponse (uint32 requestId);
    /** Destructor*/
    virtual ~LOWIAsyncDiscoveryScanResultResponse ();

    /**
     * Returns the response type
     * @return eResponseType type of Response
     */
    virtual eResponseType getResponseType ();
};

/** Status response from the wifi driver to a given
 *  background scan request (bgscan, hotlist, etc).
 */
class LOWIStatusResponse : public LOWIResponse
{
public:
  /** Status returned by lower layer */
  eScanStatus scanStatus;

  /** Request type for which this is a status response */
  LOWIRequest::eRequestType mRequestType;

  /**
   * Constructor
   * @param uint32 Corresponding Request Id generated
   *               by the client for request
   */
  LOWIStatusResponse(uint32 requestId);
  /** Destructor*/
  ~LOWIStatusResponse();

  /**
   * Returns the response type
   * @return eResponseType: type of response
   */
  virtual eResponseType getResponseType();
};

/**
 * This class provides the response to LOWIWlanStateQueryRequest
 * Currently supported WLAN state includes the connection state
 */
class LOWIWlanStateQueryResponse : public LOWIResponse
{
private:
public:
  /* Indicates the status of the response
   *
   * NOTE:
   * if status == SUCCESS, client can use the fields in the response
   * if status != SUCCESS, clients should ignore the fields in the response
   *    and consider request failed and see the reason of failure
   *    from eScanStatus
   */
  eScanStatus status;
  /**
   * Returns the WLAN connection status CONNECTED / DISCONNECTED
   * true for Connected, false for DISCONNECTED
   * NOTE: if this field is set to false, client should ignore all
   * other fields below.
   */
  bool connected;

  /**
   * Returns the BSSID of the connected Node.
   * LOWIMacAddress BSSID of the connected Node
   */
  LOWIMacAddress connectedNodeBssid;

  /**
   * Returns the freq of the connected Node.
   * Frequency of the connected Node
   */
  uint32 connectedNodeFreq;

  /**
   * Connected node RSSI in dBm
   */
  int16 connectedNodeRssi;

  /**
   * Returns the SSID of the connected Node.
   * LOWISsid SSID of the connected Node
   */
  LOWISsid connectedNodeSsid;

  /**
   * Constructor
   * @param uint32 requestId generated by the client of the request.
   * @param eScanScatus Indicates the status of the response
   *
   * NOTE:
   * if stat == SUCCESS, client can use the fields in the response
   * if stat != SUCCESS, clients should ignore the fields in the response
   *    and consider request failed and see the reason of failure
   *    from eScanStatus
   */
  LOWIWlanStateQueryResponse (uint32 requestId, eScanStatus stat)
  : LOWIResponse(requestId), status (stat), connected (false), connectedNodeFreq (0),
    connectedNodeRssi (0)
  {}

  /** Destructor*/
  virtual ~LOWIWlanStateQueryResponse () {}

  /**
   * Returns the response type
   * @return eResponseType type of Response
   */
  virtual eResponseType getResponseType ()
  {
    return LOWI_WLAN_STATE_QUERY_RESPONSE;
  }
};


} // namespace qc_loc_fw

#endif //#ifndef __LOWI_RESPONSE_H__
