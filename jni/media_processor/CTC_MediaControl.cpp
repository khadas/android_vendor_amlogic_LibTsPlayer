/*
 * author: wei.liu@amlogic.com
 * date: 2012-07-12
 * wrap original source code for CTC usage
 */

#include "CTC_MediaControl.h"
#include "CTsOmxPlayer.h"
#include <cutils/properties.h>

// need single instance?
sp<ITsPlayer> GetMediaControl(int use_omx_decoder)
{
	//return new CTC_MediaControl();
    char value[PROPERTY_VALUE_MAX] = {0};
    property_get("iptv.decoder.omx", value, "0");
    int prop_use_omxdecoder = atoi(value);

    if (prop_use_omxdecoder || use_omx_decoder)
        return new CTsOmxPlayer();
    else
        return new CTsPlayer();
}
CTC_MediaControl* GetMediaControlImpl()
{
    //if (NULL == mCTC_MediaControl)
        //mCTC_MediaControl = new CTC_MediaControl();
    //return mCTC_MediaControl;
	return NULL;
}

int Get_MediaControlVersion()
{
	return 1;
}
