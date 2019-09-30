/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Network Location Provider Glue

  Copyright  (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __OSNPGLUE_H__
#define __OSNPGLUE_H__

#include <list>
#include <comdef.h>
#include <IzatTypes.h>
#include <IOSLocationProvider.h>

namespace izat_manager {
class IzatRequest;
class IzatLocation;
class IIzatListener;
}

using namespace std;
using namespace izat_manager;

class OSNPGlue
    : public IOSLocationProvider {
public:

    static OSNPGlue * getInstance ();
    static void destroyInstance ();

    void onLocationChanged (IzatLocation * location);
    void onStatusChanged (IzatProviderStatus status);

    // override IOSLocationProvider
    int32 subscribe (IIzatListener * izatListener);
    int32 unsubscribe (IIzatListener * izatListener);
    int32 setRequest (const IzatRequest * request);
    void stopRequest ();

private:
    OSNPGlue ();
    ~OSNPGlue ();

    static OSNPGlue * mInstance;
    list <IIzatListener *> mListeners;
};

#endif // #ifndef __FUSEDLOCATIONPROVIDERGLUE_H__

