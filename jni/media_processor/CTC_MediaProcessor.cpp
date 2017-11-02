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
static int player_count = 0;
ITsPlayer* GetMediaProcessor()
{
#if 1
    int isExit = 1;
    isExit = amsysfs_get_sysfs_int("/sys/module/amvideo/parameters/ctsplayer_exist");
    player_count++;

    if (isExit == 0) {
        ALOGI("---GetMediaProcessor, CTsPlayer,player_count =%d\n", player_count);
        return new CTsPlayer();
    } else {
        ALOGI("---GetMediaProcessor, CTsHwOmxPlayer,player_count =%d\n", player_count);
        return new CTsHwOmxPlayer();
    }

#if 0

    ALOGI("---GetMediaProcessor, player_count =%d\n", player_count);
    if (player_count == 1) {
        return new CTsPlayer();
    } else if (player_count > 1) {
        return new CTsHwOmxPlayer();
    } else {
        return new CTsHwOmxPlayer();
    }
#endif
#else
    int mOmxHw = 1;
    char value[PROPERTY_VALUE_MAX] = {0};
    memset(value, 0, PROPERTY_VALUE_MAX);
    property_get("iptv.hwomxplayer", value, "1");
    mOmxHw = atoi(value);
    if (mOmxHw == 0) {
        return new CTsPlayer();
    } else if (mOmxHw == 2) {
        return new CTsOmxPlayer();
    } else {
        return new CTsHwOmxPlayer();
    }
#endif
}

ITsPlayer* GetMediaProcessor(player_type_t type)
{
    if (type == PLAYER_TYPE_OMX) {
        return new CTsOmxPlayer();
    } else if (type == PLAYER_TYPE_HWOMX) {
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
