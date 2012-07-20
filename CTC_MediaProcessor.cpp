/*
 * author: bo.cao@amlogic.com
 * date: 2012-07-20
 * wrap original source code for CTC usage
 */

#include "CTC_MediaProcessor.h"

// need single instance?
CTC_MediaProcessor* GetMediaProcessor()
{
	return new CTC_MediaProcessor();
}

int GetMediaProcessorVersion()
{
	return 1;
}
