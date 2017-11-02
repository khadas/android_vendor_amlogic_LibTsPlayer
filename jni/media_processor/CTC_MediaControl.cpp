/*
 * author: wei.liu@amlogic.com
 * date: 2012-07-12
 * wrap original source code for CTC usage
 */

#include "CTC_MediaControl.h"
#include "CTsHwOmxPlayer.h"
#include "CTsOmxPlayer.h"
#include <cutils/properties.h>

// need single instance?
sp<ITsPlayer> GetMediaControl(int use_omx_decoder)
{
	/*
    char value[PROPERTY_VALUE_MAX] = {0};
    property_get("iptv.decoder.omx", value, "0");
    int prop_use_omxdecoder = atoi(value);

    if (prop_use_omxdecoder || use_omx_decoder)
        return new CTsOmxPlayer();
    else
        return new CTsPlayer();
	*/
	switch(use_omx_decoder) {
		case 0:
		return new CTsPlayer();
		case 1:
		return new CTsOmxPlayer();
		case 2:
		return new CTsHwOmxPlayer();
		default:
		return NULL;
	}
	return NULL;
}

ITsPlayer* GetMediaControl()
{
    char value[PROPERTY_VALUE_MAX] = {0};
    property_get("iptv.decoder.omx", value, "0");
    int prop_use_omxdecoder = atoi(value);

    if (prop_use_omxdecoder)
        return new CTsOmxPlayer();
    else
        return new CTsPlayer();
}

int Get_MediaControlVersion()
{
	return 1;
}
