/*
 * author: bo.cao@amlogic.com
 * date: 2012-07-20
 * wrap original source code for CTC usage
 */

#include "CTC_MediaProcessor.h"
#include <android/log.h>    

// need single instance?

CTC_MediaProcessor* const mediaProcessor = new CTC_MediaProcessor();

CTC_MediaProcessor* GetMediaProcessor()
{
	return mediaProcessor;
}

void DeleteMediaProcessor()
{
	__android_log_print(ANDROID_LOG_INFO, "CTC_MediaProcessor", "DeleteMediaProcessor\n");
	delete mediaProcessor;
}

int GetMediaProcessorVersion()
{
	return 2;
}
