/*
 * author: chengshun.wang@amlogic.com
 * date: 2017-08-04
 * interface api for ctc middleware to ctsplayer
 */

#include "CTC_AmlPlayer.h"
#include <android/log.h>
#include "CTC_MediaProcessor.h"
#include "Amsysfsutils.h"

using namespace android;

#define LOG_TAG "CTC_AmlPlayer"
static int aml_instance = 0;

CTC_AmlPlayer::CTC_AmlPlayer()
{
    ALOGI("CTC_AmlPlayer construct \n");
    m_pTsPlayer = GetMediaProcessor();
}

CTC_AmlPlayer::CTC_AmlPlayer(int count)
{
    ALOGI("CTC_AmlPlayer construct, test count=%d,aml_instance:%d\n", count,aml_instance);
    if (count == 0) {
        m_pTsPlayer = GetMediaProcessor(PLAYER_TYPE_NORMAL);
    } else {
        /* +[SE] [BUG][BUG-167372][yanan.wang] added:increase the keep_mode_threshold from 85 to 110 when multi-instances*/
        amsysfs_set_sysfs_int("/sys/class/thermal/thermal_zone0/keep_mode_threshold", 110);
        ALOGI("CTC_AmlPlayer keep_mode_threshold is 110\n");
        m_pTsPlayer = GetMediaProcessor(PLAYER_TYPE_HWOMX);
    }
    /* +[SE] [BUG][BUG-170572[chuanqi.wang] added:keep 900M freq in 90% load when more than three-instances*/
    aml_instance++;
    if (aml_instance>=3) {
        amsysfs_set_sysfs_str("/sys/devices/system/cpu/cpufreq/interactive/target_loads", "90 900000:100");
        ALOGI("CTC_AmlPlayer target_loads is 90  900000:100\n");
    }
}
CTC_AmlPlayer::~CTC_AmlPlayer()
{
    ALOGI("CTC_AmlPlayer destroy\n");
    if (m_pTsPlayer != NULL) {
        delete m_pTsPlayer;
        m_pTsPlayer = NULL;
    }
    aml_instance--;
    /* +[SE] [BUG][BUG-167372][yanan.wang] added:increase the keep_mode_threshold from 85 to 110 when multi-instances*/
    amsysfs_set_sysfs_int("/sys/class/thermal/thermal_zone0/keep_mode_threshold", 85);
   if (aml_instance <= 0){
        amsysfs_set_sysfs_str("/sys/devices/system/cpu/cpufreq/interactive/target_loads", "50 900000:70");
         ALOGI("CTC_AmlPlayer target_loads is 50 900000:70\n");
    }

}

int CTC_AmlPlayer::CTC_GetAmlPlayerVersion()
{
    return 1.0;
}


int CTC_AmlPlayer::CTC_GetPlayMode()
{
    int mode = 0;

    mode = m_pTsPlayer->GetPlayMode();

    return mode;
}

int CTC_AmlPlayer::CTC_SetVideoWindow(int x,int y,int width,int height)
{
    int ret = OK;

    ret = m_pTsPlayer->SetVideoWindow(x, y, width, height);

    return ret;
}

void CTC_AmlPlayer::CTC_GetVideoPixels(int& width, int& height)
{
    m_pTsPlayer->GetVideoPixels(width, height);
}

bool CTC_AmlPlayer::CTC_IsSoftFit()
{
    bool b_ret = false;

    b_ret = m_pTsPlayer->IsSoftFit();

    return b_ret;
}

void CTC_AmlPlayer::CTC_SetSurface(Surface* pSurface)
{
    m_pTsPlayer->SetSurface(pSurface);
}

int CTC_AmlPlayer::CTC_SetEPGSize(int w, int h)
{
    int ret = OK;

    m_pTsPlayer->SetEPGSize(w, h);

    return ret;
}

int CTC_AmlPlayer::CTC_VideoShow()
{
    int ret = OK;

    ret = m_pTsPlayer->VideoShow();

    return ret;
}

int CTC_AmlPlayer::CTC_VideoHide()
{
    int ret = OK;

    ret = m_pTsPlayer->VideoHide();

    return ret;
}

int CTC_AmlPlayer::CTC_StartPlay()
{
    int ret = OK;

    ret = m_pTsPlayer->StartPlay();

    if (ret == 0) {
        ret = -1;
    }

    return ret;
}

int CTC_AmlPlayer::CTC_Pause()
{
    int ret = OK;

    ret = m_pTsPlayer->Pause();

    return ret;
}

int CTC_AmlPlayer::CTC_Resume()
{
    int ret = OK;

    ret = m_pTsPlayer->Resume();

    return ret;
}

int CTC_AmlPlayer::CTC_Fast()
{
    int ret = OK;

    ret = m_pTsPlayer->Fast();

    return ret;
}

int CTC_AmlPlayer::CTC_StopFast()
{
    int ret = OK;

    ret = m_pTsPlayer->StopFast();

    return ret;
}

int CTC_AmlPlayer::CTC_Stop()
{
    int ret = OK;

    ret = m_pTsPlayer->Stop();

    return ret;
}

int CTC_AmlPlayer::CTC_Seek()
{
    int ret = OK;

    ret = m_pTsPlayer->Seek();

    return ret;

}

int CTC_AmlPlayer::CTC_WriteData(unsigned char* pBuffer, unsigned int nSize)
{
    int ret = OK;

    ret = m_pTsPlayer->WriteData(pBuffer, nSize);

    return ret;

}

int CTC_AmlPlayer::SoftWriteData(int type, uint8_t *pBuffer, uint32_t nSize, uint64_t timestamp)
{
    int ret = OK;

    PLAYER_STREAMTYPE_E mtype = (PLAYER_STREAMTYPE_E)type;

    ret = m_pTsPlayer->SoftWriteData(mtype, pBuffer, nSize, timestamp);

    return ret;

}


int CTC_AmlPlayer::CTC_SetVolume(int volume)
{
    int ret = OK;

    ret = m_pTsPlayer->SetVolume(volume);

    return ret;
}

int CTC_AmlPlayer::CTC_GetVolume()
{
    int volume = 0;

    volume = m_pTsPlayer->GetVolume();

    return volume;
}

int CTC_AmlPlayer::CTC_SetRatio(int nRatio)
{
    int ret = OK;

    ret = m_pTsPlayer->SetRatio(nRatio);

    return ret;
}

int CTC_AmlPlayer::CTC_GetAudioBalance()
{
    int aBalance = 0;

    aBalance = m_pTsPlayer->GetAudioBalance();

    return aBalance;
}

int CTC_AmlPlayer::CTC_SetAudioBalance(int nAudioBalance)
{
    int ret = OK;

    ret = m_pTsPlayer->SetAudioBalance(nAudioBalance);

    return ret;
}

int CTC_AmlPlayer::CTC_GetCurrentPlayTime()
{
    int time = 0;

    time = m_pTsPlayer->GetCurrentPlayTime();

    return time;
}

int CTC_AmlPlayer::CTC_SwitchSubtitle(int pid)
{
    int ret = OK;

    m_pTsPlayer->SwitchSubtitle(pid);

    return ret;
}

int CTC_AmlPlayer::CTC_SwitchAudio(int pid)
{
    int ret = OK;

    m_pTsPlayer->SwitchAudioTrack(pid);

    return ret;
}

int CTC_AmlPlayer::CTC_InitVideo(void *para)
{
    int ret = OK;

    PVIDEO_PARA_T pVideoPara = (VIDEO_PARA_T*)para;
    //ALOGI("CTC_InitVideo: pVideoPara=%p,para=%p, pVideoPara->pid=%d,vfmt=%d, height=%d\n",
    //    pVideoPara, para, pVideoPara->pid, pVideoPara->vFmt, pVideoPara->nVideoHeight);

    m_pTsPlayer->InitVideo(pVideoPara);

    return ret;
}

int CTC_AmlPlayer::CTC_InitAudio(void *para)
{
    int ret = OK;

    PAUDIO_PARA_T pAudioPara = (AUDIO_PARA_T*)para;

    //ALOGI("CTC_InitAudio: pAudioPara=%p,para=%p, pAudioPara->pid=%d, channels=%d\n", pAudioPara, para, pAudioPara->pid, pAudioPara->nChannels);

    m_pTsPlayer->InitAudio(pAudioPara);

    return ret;
}

int CTC_AmlPlayer::CTC_InitSubtitle(void *para)
{
    int ret = OK;

    PSUBTITLE_PARA_T pSubPara = (SUBTITLE_PARA_T*)para;
    m_pTsPlayer->InitSubtitle(pSubPara);

    return ret;
}

int CTC_AmlPlayer::CTC_GetAVStatus(float *abuf, float *vbuf)
{
    int ret = OK;

    PAVBUF_STATUS pstatus;

    pstatus = (PAVBUF_STATUS)malloc(sizeof(AVBUF_STATUS));

    m_pTsPlayer->GetAvbufStatus(pstatus);
    //ALOGI("CTC_GetAVStatus : pstatus->abuf_size=%d, pstatus->abuf_data_len=%d\n", pstatus->abuf_size, pstatus->abuf_data_len);
    //ALOGI("CTC_GetAVStatus : pstatus->vbuf_size=%d, pstatus->vbuf_data_len=%d\n", pstatus->vbuf_size, pstatus->vbuf_data_len);
    if (pstatus->abuf_size != 0) {
        *abuf = (float)pstatus->abuf_data_len/pstatus->abuf_size;
    }
    if (pstatus->vbuf_size != 0) {
        *vbuf = (float)pstatus->vbuf_data_len/pstatus->vbuf_size;
    }

    free(pstatus);

    return ret;
}


int CTC_AmlPlayer::CTC_ClearLastFrame()
{
    int ret = OK;

    if (m_pTsPlayer) {
        m_pTsPlayer->ClearLastFrame();
    }
    return ret;
}

