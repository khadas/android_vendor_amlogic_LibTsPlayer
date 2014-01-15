/*
 * author: wei.liu@amlogic.com
 * date: 2012-07-12
 * wrap original source code for CTC usage
 */

#include "CTC_MediaControl.h"

// need single instance?
CTC_MediaControl* GetMediaControl()
{
	return new CTC_MediaControl();
}

int Get_MediaControlVersion()
{
	return 1;
}
