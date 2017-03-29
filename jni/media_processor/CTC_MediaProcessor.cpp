/*
 * author: bo.cao@amlogic.com
 * date: 2012-07-20
 * wrap original source code for CTC usage
 */

#include "CTC_MediaProcessor.h"
#include <android/log.h>    

// need single instance?

ITsPlayer* GetMediaProcessor()
{
    return new CTsPlayer();
}

#ifdef USE_OPTEEOS
ITsPlayer* GetMediaProcessor(bool DRMMode)
{
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
