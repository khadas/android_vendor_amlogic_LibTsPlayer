/**
 * @file 				Proxy_MediaProcessor.cpp
 * @author    	zhouyj
 * @date      	2012/9/12
 * @version   	ver 1.0
 * @brief     	定义CTC_MediaControl类中方法的代理接口。
 * @attention
*/

#include "Proxy_MediaProcessor.h"
 
Proxy_MediaProcessor::Proxy_MediaProcessor()
{
 	ctc_MediaControl = GetMediaControl(); 		
}

Proxy_MediaProcessor::~Proxy_MediaProcessor()
{
		ctc_MediaControl->~CTC_MediaControl();
}

int Proxy_MediaProcessor::Proxy_GetMediaControlVersion()
{
	int result = 1;
	return result;
}
		
int Proxy_MediaProcessor::Proxy_GetPlayMode()
{
	int result = ctc_MediaControl->GetPlayMode();
	return result;
}

bool Proxy_MediaProcessor::Proxy_StartPlay()
{
	bool result = ctc_MediaControl->StartPlay();
	return result;
}

int Proxy_MediaProcessor::Proxy_WriteData(unsigned char* pBuffer, unsigned int nSize)
{
	int result = ctc_MediaControl->WriteData(pBuffer, nSize);
	return result;
}

int Proxy_MediaProcessor::Proxy_SetVideoWindow(int x ,int y, int width, int height)
{
	int result = ctc_MediaControl->SetVideoWindow(x, y, width, height);
	return result;
}

int Proxy_MediaProcessor::Proxy_VideoShow()
{
	int result = ctc_MediaControl->VideoShow();
	return result;
}

int Proxy_MediaProcessor::Proxy_VideoHide()
{
	int result = ctc_MediaControl->VideoHide();
	return result;
}

void Proxy_MediaProcessor::Proxy_InitVideo(PVIDEO_PARA_T pVideoPara)
{
	ctc_MediaControl->InitVideo(pVideoPara);
	return;
}

void Proxy_MediaProcessor::Proxy_InitAudio(PAUDIO_PARA_T pAudioPara)
{
	ctc_MediaControl->InitAudio(pAudioPara);
	return;
}

bool Proxy_MediaProcessor::Proxy_Pause()
{
	bool result = ctc_MediaControl->Pause();
	return result;
}

bool Proxy_MediaProcessor::Proxy_Resume()
{
	bool result = ctc_MediaControl->Resume();
	return result;
}

bool Proxy_MediaProcessor::Proxy_Fast()
{
	bool result = ctc_MediaControl->Fast();
	return result;
}

bool Proxy_MediaProcessor::Proxy_StopFast()
{
	bool result = ctc_MediaControl->StopFast();
	return result;
}

bool Proxy_MediaProcessor::Proxy_Stop()
{
	bool result = ctc_MediaControl->Stop();
	return result;
}
		
bool Proxy_MediaProcessor::Proxy_Seek()
{
	bool result = ctc_MediaControl->Seek();
	return result;
}

bool Proxy_MediaProcessor::Proxy_SetVolume(int volume)
{
	bool result = ctc_MediaControl->SetVolume(volume);
	return result;
}

int Proxy_MediaProcessor::Proxy_GetVolume()
{
	int result = ctc_MediaControl->GetVolume();
	return result;
}

bool Proxy_MediaProcessor::Proxy_SetRatio(int nRatio)
{
	bool result = ctc_MediaControl->SetRatio(nRatio);
	return result;
}

int Proxy_MediaProcessor::Proxy_GetAudioBalance()
{
	int result = ctc_MediaControl->GetAudioBalance();
	return result;
}

bool Proxy_MediaProcessor::Proxy_SetAudioBalance(int nAudioBalance)
{
	bool result = ctc_MediaControl->SetAudioBalance(nAudioBalance);
	return result;
}

void Proxy_MediaProcessor::Proxy_GetVideoPixels(int& width, int& height)
{
	ctc_MediaControl->GetVideoPixels(width, height);
	return;
}

bool Proxy_MediaProcessor::Proxy_IsSoftFit()
{
	bool result = ctc_MediaControl->IsSoftFit();
	return result;
}

void Proxy_MediaProcessor::Proxy_SetEPGSize(int w, int h)
{
	ctc_MediaControl->SetEPGSize(w, h);
	return;
}

void Proxy_MediaProcessor::Proxy_SetSurface(Surface* pSurface)
{
	//ctc_MediaControl->SetSurface(pSurface);
	return;
}