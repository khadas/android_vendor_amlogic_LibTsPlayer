/*
 * author: bo.cao@amlogic.com
 * date: 2012-07-20
 * wrap original source code for CTC usage
 */

#include "CTC_MediaProcessor.h"
#include <android/log.h>    

// need single instance?
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , "CTC_MediaProcessor", __VA_ARGS__)

CTC_MediaProcessor* GetMediaProcessor(int use_omx_decoder)
{
	LOGD("GetMediaProcessor decoder ID %d \n",use_omx_decoder);
    return new CTC_MediaProcessor(use_omx_decoder);
}

CTC_MediaProcessor::CTC_MediaProcessor(int use_omx_decoder)
{
		LOGD("CTC_MediaProcessor decoder ID %d \n",use_omx_decoder);

	ctc_MediaControl = GetMediaControl(use_omx_decoder);
}

CTC_MediaProcessor::~CTC_MediaProcessor()
{
	//ctc_MediaControl->~CTC_MediaControl();
}

int CTC_MediaProcessor::GetMediaControlVersion()
{
	int result = 1;
	return result;
}

int CTC_MediaProcessor::GetPlayMode()
{
	int result = ctc_MediaControl->GetPlayMode();
	return result;
}

bool CTC_MediaProcessor::StartPlay()
{
	bool result = ctc_MediaControl->StartPlay();
	return result;
}

int CTC_MediaProcessor::WriteData(unsigned char* pBuffer, unsigned int nSize)
{
	int result = ctc_MediaControl->WriteData(pBuffer, nSize);
	return result;
}

int CTC_MediaProcessor::SetVideoWindow(int x ,int y, int width, int height)
{
	int result = ctc_MediaControl->SetVideoWindow(x, y, width, height);
	return result;
}

int CTC_MediaProcessor::VideoShow()
{
	int result = ctc_MediaControl->VideoShow();
	return result;
}

int CTC_MediaProcessor::VideoHide()
{
	int result = ctc_MediaControl->VideoHide();
	return result;
}

void CTC_MediaProcessor::InitVideo(PVIDEO_PARA_T pVideoPara)
{
	ctc_MediaControl->InitVideo(pVideoPara);
	return;
}

void CTC_MediaProcessor::InitAudio(PAUDIO_PARA_T pAudioPara)
{
	ctc_MediaControl->InitAudio(pAudioPara);
	return;
}

bool CTC_MediaProcessor::Pause()
{
	bool result = ctc_MediaControl->Pause();
	return result;
}

bool CTC_MediaProcessor::Resume()
{
	bool result = ctc_MediaControl->Resume();
	return result;
}

bool CTC_MediaProcessor::Fast()
{
	bool result = ctc_MediaControl->Fast();
	return result;
}

bool CTC_MediaProcessor::StopFast()
{
	bool result = ctc_MediaControl->StopFast();
	return result;
}

bool CTC_MediaProcessor::Stop()
{
	bool result = ctc_MediaControl->Stop();
	return result;
}

bool CTC_MediaProcessor::Seek()
{
	bool result = ctc_MediaControl->Seek();
	return result;
}

bool CTC_MediaProcessor::SetVolume(int volume)
{
	bool result = ctc_MediaControl->SetVolume(volume);
	return result;
}

int CTC_MediaProcessor::GetVolume()
{
	int result = ctc_MediaControl->GetVolume();
	return result;
}

bool CTC_MediaProcessor::SetRatio(int nRatio)
{
	bool result = ctc_MediaControl->SetRatio(nRatio);
	return result;
}

int CTC_MediaProcessor::GetAudioBalance()
{
	int result = ctc_MediaControl->GetAudioBalance();
	return result;
}

bool CTC_MediaProcessor::SetAudioBalance(int nAudioBalance)
{
	bool result = ctc_MediaControl->SetAudioBalance(nAudioBalance);
	return result;
}

void CTC_MediaProcessor::GetVideoPixels(int& width, int& height)
{
	ctc_MediaControl->GetVideoPixels(width, height);
	return;
}

bool CTC_MediaProcessor::IsSoftFit()
{
	bool result = ctc_MediaControl->IsSoftFit();
	return result;
}

void CTC_MediaProcessor::SetEPGSize(int w, int h)
{
	ctc_MediaControl->SetEPGSize(w, h);
	return;
}

void CTC_MediaProcessor::SetSurface(Surface* pSurface)
{
	//ctc_MediaControl->SetSurface(pSurface);
	return;
}

long CTC_MediaProcessor::GetCurrentPlayTime()
{
	long currentPlayTime=0;
	currentPlayTime = ctc_MediaControl->GetCurrentPlayTime();//ms
	return currentPlayTime;
}

void CTC_MediaProcessor::InitSubtitle(PSUBTITLE_PARA_T sParam)
{
	ctc_MediaControl->InitSubtitle(sParam);
	return;
}

void CTC_MediaProcessor::SwitchSubtitle(int pid)
{
	ctc_MediaControl->SwitchSubtitle(pid);
	return;
}
bool CTC_MediaProcessor::SubtitleShowHide(bool bShow)
{
	bool result = ctc_MediaControl->SubtitleShowHide(bShow);
	return result;
}

void CTC_MediaProcessor::ClearLastFrame(void)
{
	ctc_MediaControl->ClearLastFrame();
	return;
}

void CTC_MediaProcessor::BlackOut(int EarseLastFrame)
{
	ctc_MediaControl->BlackOut(EarseLastFrame);
	return;
}

bool CTC_MediaProcessor::SetErrorRecovery(int mode)
{
	bool result = ctc_MediaControl->SetErrorRecovery(mode);
	return result;
}

void CTC_MediaProcessor::GetAvbufStatus(PAVBUF_STATUS pstatus)
{
	ctc_MediaControl->GetAvbufStatus(pstatus);
	return;
}

int CTC_MediaProcessor::GetRealTimeFrameRate()
{
	int result = ctc_MediaControl->GetRealTimeFrameRate();
	return result;
}
int CTC_MediaProcessor::GetVideoFrameRate()
{
	int result = ctc_MediaControl->GetVideoFrameRate();
	return result;
}

int GetMediaProcessorVersion()
{
#ifdef TELECOM_QOS_SUPPORT
    return 3;
#else
    return 2;
#endif
}
