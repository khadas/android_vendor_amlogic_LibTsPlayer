#ifndef _CTC_HWOMXPLAYER_H_
#define _CTC_HWOMXPLAYER_H_

#include <gui/Surface.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/foundation/AMessage.h>
#include <gui/SurfaceComposerClient.h>
#include <media/stagefright/foundation/ADebug.h>
#include <gui/Surface.h>
#include <system/window.h>
#include <gui/IGraphicBufferProducer.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/ICrypto.h>
#include <media/stagefright/foundation/AString.h>
#include <utils/Log.h>
#include <utils/Mutex.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/MediaBuffer.h>
#include "CTsPlayer.h"

class LivePlayer;
using namespace android;

typedef enum {
    VIDEO_FORMAT_UNKNOWN 	= -1,
    VIDEO_FORMAT_MPEG1,
    VIDEO_FORMAT_MPEG2,
    VIDEO_FORMAT_MPEG4,
    VIDEO_FORMAT_H264,
    VIDEO_FORMAT_H265,
    VIDEO_FORMAT_MAX,
} VIDEO_FORMAT_E;
class CTsHwOmxPlayer : public ITsPlayer {
public:
    CTsHwOmxPlayer();
    virtual ~CTsHwOmxPlayer();
    virtual void InitVideo(PVIDEO_PARA_T pVideoPara);
	virtual void InitAudio(PAUDIO_PARA_T pAudioPara);
    virtual void InitSubtitle(PSUBTITLE_PARA_T pSubtitlePara);
    virtual bool StartPlay();
    virtual bool Stop();
    virtual int WriteData(unsigned char* pBuffer, unsigned int nSize);
    virtual void SetEPGSize(int w, int h);
    virtual int  SetVideoWindow(int x,int y,int width,int height);
    virtual bool createWindowSurface();
    virtual void toggleAudio();
    virtual void pauseAudio();
    virtual void resumeAudio();
    virtual void ExecuteCmd(const char* cmd_str);
    virtual int SoftWriteData(PLAYER_STREAMTYPE_E type, uint8_t *pBuffer, uint32_t nSize, uint64_t timestamp);
#ifdef IPTV_ZTE_SUPPORT
    virtual int GetAVPTSDiff(int *avdiff);
#endif
    int WriteData2();

    /*need to add an empty definition for these virtual functions*/
    virtual int  GetPlayMode();
    //x闁哄嫬澧介妵姘辨喆閸℃侗鏆�
    virtual int  VideoShow(void);
    //闂傚懏鍔樺Λ宀�鎲撮崱娑辨殽
    virtual int  VideoHide(void);
    //闁哄棗鍊告禒锟�
    virtual bool Pause();
    //缂備綀鍛暰闁圭虎鍘介弬锟�
    virtual bool Resume();
    //闊浂鍋夌换妯跨疀椤愶讣鎷烽敓锟�
    virtual bool Fast();
    //闁稿绮嶉娑滅疀椤愩儳绠婚煫鍥跺亰閿熸枻鎷�
    virtual bool StopFast();
    //閻庤鐭紞锟�
    virtual bool Seek();
    //閻犱焦鍎抽悾楣冩閹惰棄娅�
    virtual bool SetVolume(int volume);
    //闁兼儳鍢茶ぐ鍥閹惰棄娅�
    virtual int GetVolume();
    //閻犱焦鍎抽悾鍓ф喆閸℃侗鏆ラ柡鍕⒔閵囨艾袙閺傚墽浼�
    virtual bool SetRatio(int nRatio);
    //闁兼儳鍢茶ぐ鍥亹閹惧啿顤呭閫涘嵆娴滐拷
    virtual int GetAudioBalance();
    //閻犱礁澧介悿鍡樼珶娴肩繝澹�
    virtual bool SetAudioBalance(int nAudioBalance);
    //闁兼儳鍢茶ぐ鍥╂喆閸℃侗鏆ラ柛鎺戞娴滎剟鎮抽敓锟�
    virtual void GetVideoPixels(int& width, int& height);
    virtual bool IsSoftFit();
    virtual void SetSurface(Surface* pSurface);
    //16濞达絽绉锋竟濠偳庨柆宥嗕粯閻熸洑娴囬鏇犵磾閻㈩槙lorkey闁哄鍎甸敓钘夌箰閸ゎ厾鎲撮崱娑辨殽闁挎冻鎷�
    virtual void SwitchAudioTrack(int pid);
    virtual void SwitchSubtitle(int pid);
    virtual void SetProperty(int nType, int nSub, int nValue);
    virtual int64_t GetCurrentPlayTime();
    virtual void leaveChannel();
    virtual void playerback_register_evt_cb(IPTV_PLAYER_EVT_CB pfunc, void *hander);
#if defined(TELECOM_QOS_SUPPORT) || defined(IPTV_ZTE_SUPPORT)
    virtual void RegisterParamEvtCb(void *hander, IPTV_PLAYER_PARAM_Evt_e enEvt, IPTV_PLAYER_PARAM_EVENT_CB  pfunc);
#endif
    //ZTEDSP 20140905 闁告艾鐗嗛崣锟�760D 濞寸媴绲块悥婊堝储閻斿娼楅柟鎭掑劚瑜版盯宕氶崶銊ュ簥闂傚﹤鐤囧娲嫉婢舵劖锛栧Λ甯嫹
    virtual void SwitchAudioTrack_ZTE(PAUDIO_PARA_T pAudioPara);
    virtual void BlackOut(int EarseLastFrame);
    virtual void GetAvbufStatus(PAVBUF_STATUS pstatus);
    //Add PIP function
    //set the play mode
    //virtual int  SetPlayMode(PLAYER_MODE_E enPlayerMode);
    //add one program, then the new program be focused. return focus prognum, -1 means failed
    //virtual int  AddProgram(PROGRAM_PLAYER_TYPE_E eProgPlayType);
    //del one program,  must be STOP.
    //virtual int  DelProgram(int ProgNum);
    //get  the current focus prognum
    //virtual int  GetCurFocusProgram();
    //set the current focus program
    //virtual int  SetCurFocusProgram(int ProgNum);
    //put ts buff by ProgNum
    //virtual int WriteDataS(int ProgNum,unsigned char* pBuffer, unsigned int nSize);
    virtual int GetRealTimeFrameRate();
    virtual int GetVideoFrameRate();
    virtual int GetPlayerInstanceNo();
    int checkLogMask();
	char* convertVideoFormat2Mime(vformat_t);

	//16位色深需要设置colorkey来透出视频；

	virtual bool StartRender();
	virtual bool SubtitleShowHide(bool bShow);
	virtual int playerback_getStatusInfo(IPTV_ATTR_TYPE_e enAttrType, int *value);
	virtual void ClearLastFrame();
	virtual bool SetErrorRecovery(int mode);
	virtual void SetVideoHole(int x,int y,int w,int h);
	virtual void writeScaleValue();
	virtual int GetVideoDropNumber();
	virtual int GetVideoTotalNumber();
	virtual void GetVideoResolution();
	virtual int GetCurrentVidPTS(unsigned long long  *pPTS);
#ifdef IPTV_ZTE_SUPPORT
    virtual void GetVideoInfo (int *width,int *height ,int  *ratio,int *frame_format);
#else
	virtual void GetVideoInfo (int *width,int *height ,int  *ratio);
#endif
    virtual status_t setDataSource(const char *path, const KeyedVector<String8, String8> *headers = NULL);
    void createBlackOverlay();

private:
    FILE* mFp;
    VIDEO_PARA_T mSoftVideoPara;
    int mSoftNativeX;
    int mSoftNativeY;
    int mSoftNativeWidth;
    int mSoftNativeHeight;
    PLAYER_STREAMTYPE_E ctcStreamType;

    sp<SurfaceComposerClient> mSoftComposerClient;
    sp<SurfaceControl> mSoftControl;
    sp<Surface> mSoftSurface;
    sp<SurfaceComposerClient> mSoftComposerClient2;
    sp<SurfaceControl> mSoftControl2;
    sp<Surface> mSoftSurface2;

    bool mIsPlaying;
    int mApkUiWidth;
    int mApkUiHeight;
    sp<LivePlayer> mLivePlayer;
    int pipe_fd[2];
	int place_holder_fd[3];
    int mVolume;
    sp<ALooper> mLooper;
    int mDataWrited;
    char* mProbeBuffer;
    int mProbeBufferSize;
    int mInstanceNo;
    int mTsCount;
    int mTsSize;
    int mLogLevel;
    int mLogInterval;

    List<sp<ABuffer> > mTsInputBufferQueue;
    List<sp<ABuffer> > mTsInputBufferQueue2;
    size_t mTsInputBufferQueueSize;
    size_t mTsInputBufferQueueSize2;
    int64_t mTotalTsBufferSize;
    Mutex mTsInputBufferQueueLock;

    uint64_t mTotalDataReceived;
    Mutex mTsDataSizeLock;
    int mTsDataSize;
    int64_t mLastWriteTimeUs;
    int64_t mStartWriteTimeUs;
    uint64_t mLastWriteSize;
    bool mSupportMutilDecoder;
    bool mSupportVideoFormat;
    bool mSupportAudioFormat;

	size_t mSleepCount;
    uint32_t mProbeMaxBufSize;

    class RunWorker: public Thread {
    public:
        enum dt_statue_t {
            STATUE_STOPPED,
            STATUE_RUNNING,
            STATUE_PAUSE
        };
        enum dt_module_t{
            MODULE_START_PLAYER,
			MODULE_DATA_FEEDER,
        };
        dt_module_t mTaskModule;
        bool mFlag;
        RunWorker(CTsHwOmxPlayer* player, dt_module_t module);
        int32_t  start ();
        int32_t  stop ();
        int32_t  pause ();
        virtual bool threadLoop();
        dt_statue_t mTaskStatus;
    private:
        CTsHwOmxPlayer* mPlayer;
    };

    sp<RunWorker> mRunWorkerStartPlayer;
    sp<RunWorker> mRunWorkerDataFeeder;
    //PAUDIO_PARA_T mAudioPara;
    //PVIDEO_PARA_T mVideoPara;
};

#endif  //  _CTC_HWOMXPLAYER_H_

