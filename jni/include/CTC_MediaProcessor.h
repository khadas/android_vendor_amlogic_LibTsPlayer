/*
 * author: bo.cao@amlogic.com
 * date: 2012-07-20
 * wrap original source code for CTC usage
 */

#ifndef _CTC_MEDIAPROCESSOR_H_
#define _CTC_MEDIAPROCESSOR_H_
#include "CTsPlayer.h"
#include <CTC_MediaControl.h>
#include <CTsOmxPlayer.h>

#include "vformat.h"
#include "aformat.h"
//#include "AudioBufferProvider.h"
//#include "Common.h"
#include "android_runtime/AndroidRuntime.h"
#include <gui/Surface.h> 
#include <gui/ISurfaceTexture.h>
#include "android_runtime/android_view_Surface.h"

class CTC_MediaProcessor//:public CTsPlayer
{
protected:
		//CTC_MediaControl * ctc_MediaControl;
		sp<ITsPlayer> ctc_MediaControl;
public:
		CTC_MediaProcessor(int use_omx_decoder = 0);
		~CTC_MediaProcessor();
public:
		int  GetMediaControlVersion();//获取版本
		int  GetPlayMode();//取得播放模式
		int  SetVideoWindow(int x,int y,int width,int height);//设置视频显示的位置，以及视频显示的宽高
		int  VideoShow(void);//显示视频图像
		int  VideoHide(void);//隐藏视频图像
		void InitVideo(PVIDEO_PARA_T pVideoPara);//初始化视频参数
		void InitAudio(PAUDIO_PARA_T pAudioPara);//初始化音频参数
		bool StartPlay();//开始播放
		int  WriteData(unsigned char* pBuffer, unsigned int nSize);//将ts流写入缓冲区
		bool Pause();//暂停
		bool Resume();//暂停后的恢复
		bool Fast();//快进或者快退
		bool StopFast();//停止快进或者快退
		bool Stop();//停止
		bool Seek();//定位
		bool SetVolume(int volume);//设定音量
		int  GetVolume();//获取音量
		bool SetRatio(int nRatio);//设定视频显示比例
		int  GetAudioBalance();//获取当前声道
		bool SetAudioBalance(int nAudioBalance);//设置声道
		void GetVideoPixels(int& width, int& height);//获取视频分辩率
		bool IsSoftFit();//判断是否由软件拉伸
		void SetEPGSize(int w, int h);//设置EPG大小
		void SetSurface(Surface* pSurface);//设置显示用的surface

		long  GetCurrentPlayTime();
		void InitSubtitle(PSUBTITLE_PARA_T sParam);
		void SwitchSubtitle(int pid);//设置显示用的surface
		bool SubtitleShowHide(bool bShow);

		void ClearLastFrame() ;

		void BlackOut(int EarseLastFrame) ;
		bool SetErrorRecovery(int mode);

		void GetAvbufStatus(PAVBUF_STATUS pstatus);
		int GetRealTimeFrameRate();
		int GetVideoFrameRate();
		/*end add*/

};

// 获取CTC_MediaProcessor 派生类的实例对象。在CTC_MediaProcessor () 这个接口的实现中，需要创建一个
// CTC_MediaProcessor 派生类的实例，然后返回这个实例的指针
CTC_MediaProcessor* GetMediaProcessor(int use_omx_decoder);  // { return NULL; }


// 获取底层模块实现的接口版本号。将来如果有多个底层接口定义，使得上层与底层之间能够匹配。本版本定义
// 返回为1
int GetMediaProcessorVersion();  //  { return 0; }

#endif  // _CTC_MEDIAPROCESSOR_H_
