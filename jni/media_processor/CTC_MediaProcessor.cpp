/*
 * author: bo.cao@amlogic.com
 * date: 2012-07-20
 * wrap original source code for CTC usage
 */

#include "CTC_MediaProcessor.h"
#include "CTsOmxPlayer.h"
#include "CTsHwOmxPlayer.h"
#include <android/log.h>
#include <cutils/properties.h>
#include "Amsysfsutils.h"


#define LOG_TAG "CTC_MediaProcessor"



// need single instance?
ITsPlayer* GetMediaProcessor()
{
    property_set("iptv.middle.softdemux", "0");
    property_set("media.ctcplayer.enable", "0");
    return new CTsPlayer();
}

ITsPlayer* GetMediaProcessor(player_type_t type)
{
    int mOmxDebug = 0;
    char value[PROPERTY_VALUE_MAX] = {0};

    memset(value, 0, PROPERTY_VALUE_MAX);
    property_get("media.ctcplayer.omxdebug", value, "0");
    mOmxDebug = atoi(value);

    if (type == PLAYER_TYPE_OMX) {
        return new CTsOmxPlayer();
    } else if (type == PLAYER_TYPE_HWOMX || mOmxDebug == 1) {
        return new CTsHwOmxPlayer();
    } else {
        return new CTsPlayer();
    }
}
#ifdef USE_OPTEEOS
ITsPlayer* GetMediaProcessor(bool DRMMode)
{
    return new CTsPlayer(DRMMode);
}

ITsPlayer* GetMediaProcessor(player_type_t type, bool DRMMode)
{
    if (type == PLAYER_TYPE_OMX)
        return new CTsOmxPlayer();
    else
        return new CTsPlayer(DRMMode);
}
#endif

int GetMediaProcessorVersion()
{
#ifdef TELECOM_QOS_SUPPORT
    return 3;
#else
    return 2;
#endif
}
