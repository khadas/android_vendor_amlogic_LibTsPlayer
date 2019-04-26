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
    char value[PROPERTY_VALUE_MAX] = {0};
    int middle_soft_demux = 0;
    int multi_enable = 0;

    amsysfs_write_prop_ctc_multi("iptv.middle.softdemux", "0");
    amsysfs_write_prop_ctc_multi("media.ctcplayer.enable", "0");

    memset(value, 0, PROPERTY_VALUE_MAX);
    property_get("iptv.middle.softdemux", value, "0");
    middle_soft_demux = atoi(value);

    memset(value, 0, PROPERTY_VALUE_MAX);
    property_get("media.ctcplayer.enable", value, "0");
    multi_enable = atoi(value);
    /* +[SE] [BUG][BUG-167372][yanan.wang] added:increase the keep_mode_threshold from 85 to 110 when multi-instances*/
    amsysfs_set_sysfs_int("/sys/class/thermal/thermal_zone0/keep_mode_threshold", 85);

    ALOGI("GetMediaProcessor, middle_soft_demux=%d, multi_enable=%d\n", middle_soft_demux, multi_enable);
    return new CTsPlayer();
}

ITsPlayer* GetMediaProcessor(player_type_t type)
{
    ALOGI("GetMediaProcessor, type=%d\n", type);
    int mOmxDebug = 0;
    char value[PROPERTY_VALUE_MAX] = {0};

    memset(value, 0, PROPERTY_VALUE_MAX);
    property_get("media.ctcplayer.omxdebug", value, "0");
    mOmxDebug = atoi(value);

    /*display_mode 1: vd2; 2: 9-video layer */
    int display_mode = 0;
    memset(value, 0, PROPERTY_VALUE_MAX);
    property_get("media.ctc.display.mode", value, "0");
    display_mode = atoi(value);

    ALOGI("GetMediaProcessor, display_mode=%d\n", display_mode);
    if (type == PLAYER_TYPE_OMX) {
        return new CTsOmxPlayer();
    } else if (type == PLAYER_TYPE_HWOMX || mOmxDebug == 1 || display_mode == 1 || display_mode == 2) {
        return new CTsHwOmxPlayer();
    } else if (type == PLAYER_TYPE_NORMAL_MULTI) {
        struct CTsParameter p;
        p.mMultiSupport = 1;
        return new CTsPlayer(p);
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
