/**
 * @file 		iptv_player_jni.cpp
 * @author    	zhouyj
 * @date      	2012/9/5
 * @version   	ver 1.0
 * @brief     	定义CTC_MediaProcessor类中方法的jni接口，供上层调用。
 * @attention
*/

#include "vformat.h"
#include "aformat.h"
#include "android_runtime/AndroidRuntime.h"
#include <gui/Surface.h> 
#include <gui/ISurfaceTexture.h>
#include "android_runtime/android_view_Surface.h"
#include "Proxy_MediaProcessor.h"
using namespace android;


#include "android/log.h"
#include "jni.h"
#include "stdio.h"
#include <android/bitmap.h>
#include <string.h>

#define  LOG_TAG    "CTC_MediaControl"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define BUFF_SIZE (32*1024)

Mutex    gMutexLock;

static FILE * g_TestResultLogHandle = NULL;

static FILE * GetTestResultLogHandle()
{
	  if(g_TestResultLogHandle == NULL)
	  {
	    g_TestResultLogHandle = fopen("/data/data/com.ctc/mediaTestSuite.txt", "wb");
	    if(g_TestResultLogHandle == NULL)
		  {
		    LOGE("create file :error");
		    return NULL;
		  }
		  else
		  {
		  		char  writebuf[2000];
	        unsigned int buflen = 2000;
	        memset(writebuf,0,buflen);
	        snprintf(writebuf,buflen,"%s\r\n%s\r\n","ctc_player test result:","*********************"); 
	        writebuf[buflen -1] = 0;
	        fwrite(writebuf,1,strlen(writebuf),g_TestResultLogHandle);
	        fflush(g_TestResultLogHandle);
		  }
	   }
	

	return g_TestResultLogHandle;
	
}

#ifdef __cplusplus
extern "C" {
#endif

Proxy_MediaProcessor* proxy_mediaProcessor = new Proxy_MediaProcessor();
FILE *fp;
int isPause = 0;

//将test结果输出到文件
void Java_com_ctc_MediaProcessorDemoActivity_nativeWriteFile(JNIEnv* env, jobject thiz, jstring Function, jstring Return, jstring Result)
{
	FILE *result_fp = GetTestResultLogHandle();
	
	if(result_fp == NULL)
	{
		LOGE("create file :error");
		return;
	}
	const char* Function_t = (*env).GetStringUTFChars(Function, NULL);
	const char* Return_t = (*env).GetStringUTFChars(Return, NULL);
	const char* Result_t = (*env).GetStringUTFChars(Result, NULL);
	char *divide_str = "*********************";
	char  writebuf[2000];
	unsigned int buflen = 2000;
	memset(writebuf,0,buflen);
	snprintf(writebuf,buflen,"%s\r\n%s\r\n%s\r\n%s\r\n",Function_t, Return_t, Result_t, divide_str);
	writebuf[buflen -1] = 0;
	fwrite(writebuf,1,strlen(writebuf),result_fp);
	fflush(result_fp);
	return;
}

//从java层获取surface
jint Java_com_ctc_MediaProcessorDemoActivity_nativeCreateSurface(JNIEnv* env, jobject thiz, jobject pSurface, int w, int h)
{
	
	LOGI("get the surface");
	//sp<Surface> surface(Surface_getSurface(env, pSurface));	//This is for ICS
	sp<Surface> surface(android_view_Surface_getSurface(env, pSurface));
	
//mySurface = getNativeSurface(env, pSurface);
//ctc_MediaControl->SetSurface(mySurface);
	LOGI("success: get surface");
	proxy_mediaProcessor->Proxy_SetSurface(surface.get());
	LOGI("success: set surface ");
	proxy_mediaProcessor->Proxy_SetEPGSize(w, h);
	
		
	return 0;
} 

//设置EPG大小
void Java_com_ctc_MediaProcessorDemoActivity_nativeSetEPGSize(JNIEnv* env, jobject thiz, int w, int h)
{
	proxy_mediaProcessor->Proxy_SetEPGSize(w, h);
	return;
}

//根据音视频参数初始化播放器
jint Java_com_ctc_MediaProcessorDemoActivity_nativeInit(JNIEnv* env, jobject thiz)
{
	VIDEO_PARA_T  videoPara;
	videoPara.pid = 101;
	videoPara.nVideoWidth = 544;
	videoPara.nVideoHeight = 576;
	videoPara.nFrameRate = 25;
	videoPara.vFmt = VFORMAT_H264;
	videoPara.cFmt = 0;
	
	AUDIO_PARA_T audioPara;
	audioPara.pid = 144;
	audioPara.nChannels = 1;
	audioPara.nSampleRate = 48000;
	audioPara.aFmt = AFORMAT_MPEG;
	audioPara.nExtraSize = 0;
	audioPara.pExtraData = NULL;
	
	proxy_mediaProcessor->Proxy_InitVideo(&videoPara);
	proxy_mediaProcessor->Proxy_InitAudio(&audioPara);
	return 0;
}

//开始播放
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeStartPlay(JNIEnv* env, jobject thiz)
{
	jboolean result = proxy_mediaProcessor->Proxy_StartPlay();
	return result;
}

//写入数据播放
jint Java_com_ctc_MediaProcessorDemoActivity_nativeWriteData(JNIEnv* env, jobject thiz, jstring url)
{
	const char* URL = (*env).GetStringUTFChars(url, NULL);
	
	int rd_result = 0;
	fp = fopen(URL, "rb+");
	if (fp == NULL)
		{
		LOGE("open file:error!");
		return -1;
	}
	
	while(true)
	{
	 {
	 	Mutex::Autolock l(gMutexLock);
		char* buffer = (char* )malloc(BUFF_SIZE);
		rd_result = fread(buffer, BUFF_SIZE, 1, fp);
		if (rd_result <= 0)	
			{
				LOGE("read the end of file");
			//	proxy_mediaProcessor->~Proxy_MediaProcessor();
				exit(1);
			}
		
		int wd_result = proxy_mediaProcessor->Proxy_WriteData((unsigned char*) buffer, (unsigned int) BUFF_SIZE);
		LOGE("the wd_result[%d]", wd_result);
		
	 }
	 
	 usleep(130*1000);
	
	 
	}
	return 0;
	
}

 
//取得播放模式
jint Java_com_ctc_MediaProcessorDemoActivity_nativeGetPlayMode(JNIEnv* env, jobject thiz)
{
	int result = proxy_mediaProcessor->Proxy_GetPlayMode();
	LOGE("step:1");

  return (jint)result;
}

//设置播放区域的位置和播放区域的宽高
jint Java_com_ctc_MediaProcessorDemoActivity_nativeSetVideoWindow(JNIEnv* env, jobject thiz ,jint x, jint y, jint width, jint height)
{
	int result = proxy_mediaProcessor->Proxy_SetVideoWindow(x, y, width, height);
	LOGE("SetVideoWindow result:[%d]", result);
	return result;
}

//播放器暂停
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativePause(JNIEnv* env, jobject thiz)
{
	LOGE("NEXT:Pause");
	isPause = 1;
	jboolean result = proxy_mediaProcessor->Proxy_Pause();
	return result;
}
//播放器继续播放
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeResume(JNIEnv* env, jobject thiz)
{
	LOGE("NEXT:Resume");
	isPause = 0;
	jboolean result = proxy_mediaProcessor->Proxy_Resume();
	return result;
}

//播放器选时
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeSeek(JNIEnv* env, jobject thiz)
{
	LOGE("nativeSeek---0");
	Mutex::Autolock l(gMutexLock);
	LOGE("nativeSeek---1");	
	fseek(fp, 0, 0);
	LOGE("nativeSeek---2");
	jboolean result = proxy_mediaProcessor->Proxy_Seek();
	LOGE("nativeSeek---3");
	return result;
}

//显示视频
jint Java_com_ctc_MediaProcessorDemoActivity_nativeVideoShow(JNIEnv* env, jobject thiz)
{
	jint result = proxy_mediaProcessor->Proxy_VideoShow();
	return result;
}

//隐藏视频
jint Java_com_ctc_MediaProcessorDemoActivity_nativeVideoHide(JNIEnv* env, jobject thiz)
{
	jint result = proxy_mediaProcessor->Proxy_VideoHide();
	return result;
}

//快进快退
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeFast(JNIEnv* env, jobject thiz)
{
	jboolean result = proxy_mediaProcessor->Proxy_Fast();
	return result;
}

//停止快进快退
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeStopFast(JNIEnv* env, jobject thiz)
{
	jboolean result = proxy_mediaProcessor->Proxy_StopFast();
	return result;
}

//停止播放
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeStop(JNIEnv* env, jobject thiz)
{
	jboolean result = proxy_mediaProcessor->Proxy_Stop();
	
	return result;
}

//获取音量
jint Java_com_ctc_MediaProcessorDemoActivity_nativeGetVolume(JNIEnv* env, jobject thiz)
{
	jint result = proxy_mediaProcessor->Proxy_GetVolume();
	LOGE("the volume is [%d]",result);
	return result;
}

//设定音量
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeSetVolume(JNIEnv* env, jobject thiz,jint volume)
{
	jboolean result = proxy_mediaProcessor->Proxy_SetVolume(volume);
	return result;
}

//设定视频显示比例
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeSetRatio(JNIEnv* env, jobject thiz,jint nRatio)
{
	jboolean result = proxy_mediaProcessor->Proxy_SetRatio(nRatio);
	return result;
}

//获取当前声道
jint Java_com_ctc_MediaProcessorDemoActivity_nativeGetAudioBalance(JNIEnv* env, jobject thiz)
{
	jint result = proxy_mediaProcessor->Proxy_GetAudioBalance();
	return result;
}

//设置声道
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeSetAudioBalance(JNIEnv* env, jobject thiz, jint nAudioBalance)
{
	jboolean result = proxy_mediaProcessor->Proxy_SetAudioBalance(nAudioBalance);
	return result;
}

//获取视频分辨率
void Java_com_ctc_MediaProcessorDemoActivity_nativeGetVideoPixels(JNIEnv* env, jobject thiz)
{
	int width;
	int height;
	LOGE("the video prixels ");
	proxy_mediaProcessor->Proxy_GetVideoPixels(width, height);
	LOGE("the video prixels :[%d]*[%d]",width, height);
	return;
}

//获取是否由软件拉伸，如果由硬件拉伸，返回false
jboolean Java_com_ctc_MediaProcessorDemoActivity_nativeIsSoftFit(JNIEnv* env, jobject thiz)
{
	jboolean result = proxy_mediaProcessor->Proxy_IsSoftFit();
	return result;
}

#ifdef __cplusplus
}
#endif
