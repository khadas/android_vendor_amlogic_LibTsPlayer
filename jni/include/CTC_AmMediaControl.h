/*
 * author: wei.liu@amlogic.com
 * date: 2012-07-12
 * wrap original source code for CTC usage
 */

#ifndef _CTC_AMMEDIACONTROL_H_
#define _CTC_AMMEDIACONTROL_H_
//#include "CTsPlayer.h"
#include "CTC_AmMediaProcessor.h"
#include <media/CTC_MediaControl.h>


class CTC_AmMediaControl :public ICTC_MediaControl {
public:
    CTC_AmMediaControl();
    virtual ~CTC_AmMediaControl();
public:
    virtual void  PlayFromStart(char* url);
    virtual void  PlayFromStart(int fd);
    virtual void  Pause();
    virtual void  Resume();
    virtual void  PlayByTime(int time);
    virtual void  Fast(int speed);
    virtual void Stop();
    virtual void SetSurface(Surface* pSurface);
    virtual int GetCurrentPlayTime();
    virtual int GetDuration();
    virtual void SetListenNotify(ICTC_MCNotify* notify);
    virtual void SetVolume(float leftVolume, float rightVolume);
private:
    CTC_AmMediaProcessor *pAmMediaProce;
    ICTC_MCNotify* mNotify;
};
//typedef CTC_MediaControl* (*FGetMediaControl)();
//typedef void (*FFreeMediaControl)(CTC_MediaControl* pMediaContrl);


#endif  // _CTC_AMMEDIACONTROL_H_
