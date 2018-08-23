#ifndef _TSPLAYER_H_
#define _TSPLAYER_H_
#include <android/log.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <gui/Surface.h>

using namespace android;
extern "C" {
#include <amports/vformat.h>
#include <amports/aformat.h>
#include <amports/amstream.h>
#include <codec.h>
#include <codec_info.h>
}

#include <string.h>
#include <utils/Timers.h>


#define lock_t          pthread_mutex_t
#define lp_lock_init(x,v)   pthread_mutex_init(x,v)
#define lp_lock_deinit(x)   pthread_mutex_destroy(x)
#define lp_lock(x)      pthread_mutex_lock(x)
#define lp_trylock(x)   pthread_mutex_trylock(x)
#define lp_unlock(x)    pthread_mutex_unlock(x)



#define IN
#define OUT

typedef struct{
	void *(*init)(IN int flags);
  int (*read)(IN void *handle, IN unsigned char *buf, IN int size);
  int (*ready)(IN void *handle);
  int (*exit)(IN void *handle);
}wiptv_callback;


#define TRICKMODE_NONE       0x00
#define TRICKMODE_I          0x01
#define TRICKMODE_I_HEVC     0x07
#define TRICKMODE_FFFB       0x02
#define MAX_AUDIO_PARAM_SIZE 10
#define MAX_SUBTITLE_PARAM_SIZE 10

typedef enum
{
    PLAYER_STREAMTYPE_NULL		= -1,
	PLAYER_STREAMTYPE_TS,					//TS鏁版嵁
    PLAYER_STREAMTYPE_VIDEO,				//ES Video鏁版嵁
    PLAYER_STREAMTYPE_AUDIO,				//ES Audio鏁版嵁
    PLAYER_STREAMTYPE_SUBTITLE,
    PLAYER_STREAMTYPE_MAX,
}PLAYER_STREAMTYPE_E;
typedef struct{
	unsigned short	pid;//pid
	int				nVideoWidth;//视频宽度
	int				nVideoHeight;//视频高度
	int				nFrameRate;//帧率
	vformat_t		vFmt;//视频格式
	unsigned long	cFmt;//编码格式
	uint32_t		nExtraSize;
	uint8_t			*pExtraData;
}VIDEO_PARA_T, *PVIDEO_PARA_T;
typedef struct{
	unsigned short	pid;//pid
	int				nChannels;//声道数
	int				nSampleRate;//采样率
	aformat_t		aFmt;//音频格式
	int				nExtraSize;
	unsigned char*	pExtraData;
}AUDIO_PARA_T, *PAUDIO_PARA_T;

#ifndef AVCODEC_AVCODEC_H
typedef enum {
	/* subtitle codecs */
    CTC_CODEC_ID_DVD_SUBTITLE= 0x17000,
    CTC_CODEC_ID_DVB_SUBTITLE,
    CTC_CODEC_ID_TEXT,  ///< raw UTF-8 text
    CTC_CODEC_ID_XSUB,
    CTC_CODEC_ID_SSA,
    CTC_CODEC_ID_MOV_TEXT,
    CTC_CODEC_ID_HDMV_PGS_SUBTITLE,
    CTC_CODEC_ID_DVB_TELETEXT,
    CTC_CODEC_ID_SRT,
    CTC_CODEC_ID_MICRODVD,
}SUB_TYPE;
#endif
typedef struct{
    unsigned short pid;//pid
	int sub_type;
}SUBTITLE_PARA_T, *PSUBTITLE_PARA_T;

typedef struct ST_LPbuffer{
    unsigned char *rp;
    unsigned char *wp;
    unsigned char *buffer;
    unsigned char *bufferend;
    int valid_can_read;
    bool enlpflag;
}LPBUFFER_T;

typedef struct ctsplayer_state {
    int valid;

    // setting
    int64_t last_update_time;
    /*for caton info*/
    struct codec_quality_info quality_info;
    int catoning;
    int caton_start_underflow;
    int64_t caton_start_time;
    /*for calc avg stream bitrate*/
    int64_t bytes_record_starttime_ms;
    int64_t bytes_record_start;
    int64_t bytes_record_cur;

    // player info
    int first_picture_comming;
    int64_t first_frame_pts;
    int64_t avdiff;
    int ff_mode;
    int ts_error;
    int ts_sync_loss;
    int ecm_error;

    // video info
    int64_t vpts;
    int video_ratio;
    int video_rWH = 0;
    int Video_frame_format = 0;
    int video_width;
    int video_height;
    int vbuf_size;
    int vbuf_used;
    int vdec_total;
    int vdec_error;
    int vdec_drop;
    int vdec_underflow;
    int vpts_error;
    int frame_rate;
    int current_fps;

    // audio info
    int64_t apts;
    int abuf_size;
    int abuf_used;
    int adec_error;
    int adec_drop;
    int adec_underflow;
    int apts_error;

    //audio decode info
    int samplate;
    int channel;
    int bitrate;
    int audio_bps;
    int audio_type;

    //other info
    int stream_bitrate;//avg from writedata, duration 1 sec
    int caton_times; //the num of carton
    int caton_time;
};

typedef enum {
    IPTV_PLAYER_EVT_STREAM_VALID=0,
    IPTV_PLAYER_EVT_FIRST_PTS,     //first frame decoded event
    IPTV_PLAYER_EVT_VOD_EOS,       //VOD EOS event
    IPTV_PLAYER_EVT_ABEND,         //under flow event
    IPTV_PLAYER_EVT_PLAYBACK_ERROR,// playback error event
    IPTV_PLAYER_EVT_VID_FRAME_ERROR =0x200,// 视频解码错误
    IPTV_PLAYER_EVT_VID_DISCARD_FRAME,// 视频解码丢帧
    IPTV_PLAYER_EVT_VID_DEC_UNDERFLOW,// 视频解码下溢
    IPTV_PLAYER_EVT_VID_PTS_ERROR,// 视频解码Pts错误
    IPTV_PLAYER_EVT_AUD_FRAME_ERROR,// 音频解码错误
    IPTV_PLAYER_EVT_AUD_DISCARD_FRAME,// 音频解码丢弃
    IPTV_PLAYER_EVT_AUD_DEC_UNDERFLOW,//音频解码下溢
    IPTV_PLAYER_EVT_AUD_PTS_ERROR,// 音频PTS错误
    IPTV_PLAYER_EVT_BUTT
}IPTV_PLAYER_EVT_e;

typedef enum {
    IPTV_PLAYER_ATTR_VID_ASPECT=0,  /* 视频宽高比 0--640*480，1--720*576，2--1280*720，3--1920*1080,4--3840*2160,5--others等标识指定分辨率*/
    IPTV_PLAYER_ATTR_VID_RATIO,     //视频宽高比, 0代表4：3，1代表16：9
    IPTV_PLAYER_ATTR_VID_SAMPLETYPE,     //帧场模式, 1代表逐行源，0代表隔行源
    IPTV_PLAYER_ATTR_VIDAUDDIFF,     //音视频播放diff
    IPTV_PLAYER_ATTR_VID_BUF_SIZE,     //视频缓冲区大小
    IPTV_PLAYER_ATTR_VID_USED_SIZE,     //视频缓冲区使用大小
    IPTV_PLAYER_ATTR_AUD_BUF_SIZE,     //音频缓冲区大小
    IPTV_PLAYER_ATTR_AUD_USED_SIZE,     //音频缓冲区已使用大小
    IPTV_PLAYER_ATTR_AUD_SAMPLERATE,     //音频缓冲区已使用大小
    IPTV_PLAYER_ATTR_AUD_BITRATE,     //音频缓冲区已使用大小
    IPTV_PLAYER_ATTR_AUD_CHANNEL_NUM,     //音频缓冲区已使用大小
    IPTV_PLAYER_ATTR_VID_FRAMERATE = 18, //video frame rate
    IPTV_PLAYER_ATTR_BUTT,
    IPTV_PLAYER_ATTR_V_HEIGHT, //video height
    IPTV_PLAYER_ATTR_V_WIDTH,  //video width
    IPTV_PLAYER_ATTR_STREAM_BITRATE,//stream bitrate
    IPTV_PLAYER_ATTR_CATON_TIMES,  //the num of caton
    IPTV_PLAYER_ATTR_CATON_TIME,    //the time of caton total time
}IPTV_ATTR_TYPE_e;

typedef void (*IPTV_PLAYER_EVT_CB)(IPTV_PLAYER_EVT_e evt, void *handler);


typedef enum {
    VID_FRAME_TYPE_UNKNOWN = 0,
    VID_FRAME_TYPE_I,
    VID_FRAME_TYPE_P,
    VID_FRAME_TYPE_B,
    VID_FRAME_TYPE_IDR,
    VID_FRAME_TYPE_BUTT,
}VID_FRAME_TYPE_e;

typedef struct {
    VID_FRAME_TYPE_e enVidFrmType;
    int  nVidFrmSize;
    int  nMinQP;
    int  nMaxQP;
    int  nAvgQP;
    int  nMaxMV;
    int  nMinMV;
    int  nAvgMV;
    int  SkipRatio;
    int  nUnderflow;
    #if 0
    int  nVidFrmQP;
    int  nVidFrmPTS;
    int  nMaxSkip;
    int  nMinSkip;
    int  nAvgSkip;
    #endif
}VIDEO_FRM_STATUS_INFO_T;

#ifdef TELECOM_QOS_SUPPORT
typedef enum {
    IPTV_PLAYER_PARAM_EVT_VIDFRM_STATUS_REPORT = 0,
    IPTV_PLAYER_PARAM_EVT_BUTT
}IPTV_PLAYER_PARAM_Evt_e;

typedef void (*IPTV_PLAYER_PARAM_EVENT_CB)( void *hander, IPTV_PLAYER_PARAM_Evt_e enEvt, void *pParam);
#endif

typedef struct {
    int abuf_size;
    int abuf_data_len;
    int abuf_free_len;
    int vbuf_size;
    int vbuf_data_len;
    int vbuf_free_len;
}AVBUF_STATUS, *PAVBUF_STATUS;

int enable_gl_2xscale(const char *);

int Active_osd_viewport(int , int );

class CTsPlayer;
class ITsPlayer{
public:
    ITsPlayer(){}
    virtual ~ITsPlayer(){}
public:
    virtual int  GetPlayMode()=0;
    //显示窗口
    virtual int  SetVideoWindow(int x,int y,int width,int height)=0;
    //x显示视频
    virtual int  VideoShow(void)=0;
    //隐藏视频
    virtual int  VideoHide(void)=0;
    //初始化视频参数
    virtual void InitVideo(PVIDEO_PARA_T pVideoPara)=0;
    //初始化音频参数
    virtual void InitAudio(PAUDIO_PARA_T pAudioPara)=0;
    //开始播放
    virtual bool StartPlay()=0;
    //把ts流写入
    virtual int WriteData(unsigned char* pBuffer, unsigned int nSize)=0;
    //暂停
    virtual bool Pause()=0;
    //继续播放
    virtual bool Resume()=0;
    //快进快退
    virtual bool Fast()=0;
    //停止快进快退
    virtual bool StopFast()=0;
    //停止
    virtual bool Stop()=0;
    //定位
    virtual bool Seek()=0;
    //设定音量
    virtual bool SetVolume(int volume)=0;
    //获取音量
    virtual int GetVolume()=0;
    //设定视频显示比例
    virtual bool SetRatio(int nRatio)=0;
    //获取当前声道
    virtual int GetAudioBalance()=0;
    //设置声道
    virtual bool SetAudioBalance(int nAudioBalance)=0;
    //获取视频分辩率
    virtual void GetVideoPixels(int& width, int& height)=0;
    virtual bool IsSoftFit()=0;
    virtual void SetEPGSize(int w, int h)=0;
    virtual void SetSurface(Surface* pSurface)=0;

    //16位色深需要设置colorkey来透出视频；
    virtual void SwitchAudioTrack(int pid) = 0;
    virtual void SwitchSubtitle(int pid) = 0;
    virtual void SetProperty(int nType, int nSub, int nValue) = 0;
    virtual int64_t GetCurrentPlayTime() = 0;
    virtual void leaveChannel() = 0;
    virtual void playerback_register_evt_cb(IPTV_PLAYER_EVT_CB pfunc, void *hander) = 0;
#ifdef TELECOM_QOS_SUPPORT
    virtual void RegisterParamEvtCb(void *hander, IPTV_PLAYER_PARAM_Evt_e enEvt, IPTV_PLAYER_PARAM_EVENT_CB  pfunc) = 0;
#endif
    virtual int playerback_getStatusInfo(IPTV_ATTR_TYPE_e enAttrType, int *value)=0;

    virtual void ClearLastFrame() = 0;
    virtual void BlackOut(int EarseLastFrame)= 0;
    virtual bool SetErrorRecovery(int mode) = 0;
    virtual void GetAvbufStatus(PAVBUF_STATUS pstatus) = 0;
    virtual int GetRealTimeFrameRate() = 0;
    virtual int GetVideoFrameRate() = 0;
    virtual int GetVideoDropNumber() = 0;
    virtual int GetVideoTotalNumber() = 0;
    virtual void GetVideoResolution() = 0;
	virtual void InitSubtitle(PSUBTITLE_PARA_T pSubtitlePara)=0;
    virtual bool SubtitleShowHide(bool bShow) = 0;
    virtual void SetVideoHole(int x,int y,int w,int h) = 0;
    virtual void writeScaleValue() = 0;
    virtual void SwitchAudioTrack_ZTE(PAUDIO_PARA_T pAudioPara)= 0;
	virtual int GetCurrentVidPTS(unsigned long long *pPTS)=0;
	virtual void GetVideoInfo(int *width, int *height, int *ratio)=0;
	virtual int GetPlayerInstanceNo() = 0;
	virtual void ExecuteCmd(const char* cmd_str) = 0;
	virtual int SoftWriteData(PLAYER_STREAMTYPE_E type, uint8_t *pBuffer, uint32_t nSize, uint64_t timestamp) = 0;
    virtual status_t setDataSource(const char *path, const KeyedVector<String8, String8> *headers = NULL) = 0;

	/*end add*/

};

class CTsPlayer : public ITsPlayer
{
public:
    CTsPlayer();
#ifdef USE_OPTEEOS
    CTsPlayer(bool DRMMode);
    CTsPlayer(bool DRMMode, bool omx_player);
#else
    CTsPlayer(bool omx_player);
#endif
    virtual ~CTsPlayer();
public:
    //取得播放模式
    virtual int  GetPlayMode();
    //显示窗口
    virtual int  SetVideoWindow(int x,int y,int width,int height);
    //x显示视频
    virtual int  VideoShow(void);
    //隐藏视频
    virtual int  VideoHide(void);
    //初始化视频参数
    virtual void InitVideo(PVIDEO_PARA_T pVideoPara);
    //初始化音频参数
    virtual void InitAudio(PAUDIO_PARA_T pAudioPara);
    virtual void InitSubtitle(PSUBTITLE_PARA_T pSubtitlePara);
    //开始播放
    virtual bool StartPlay();
    virtual bool StartRender();
    //把ts流写入
    virtual int WriteData(unsigned char* pBuffer, unsigned int nSize);
    //暂停
    virtual bool Pause();
    //继续播放
    virtual bool Resume();
    //快进快退
    virtual bool Fast();
    //停止快进快退
    virtual bool StopFast();
    //停止
    virtual bool Stop();
    //定位
    virtual bool Seek();
    //设定音量
    virtual bool SetVolume(int volume);
    //获取音量
    virtual int GetVolume();
    //设定视频显示比例
    virtual bool SetRatio(int nRatio);
    //获取当前声道
    virtual int GetAudioBalance();
    //设置声道
    virtual bool SetAudioBalance(int nAudioBalance);
    //获取视频分辩率
    virtual void GetVideoPixels(int& width, int& height);
    virtual bool IsSoftFit();
    virtual void SetEPGSize(int w, int h);
    virtual void SetSurface(Surface* pSurface);
    //16位色深需要设置colorkey来透出视频；
    virtual void SwitchAudioTrack(int pid);
    virtual void SwitchSubtitle(int pid);
    virtual bool SubtitleShowHide(bool bShow);
    virtual void SetProperty(int nType, int nSub, int nValue);
    virtual int64_t GetCurrentPlayTime();
    virtual void leaveChannel();
    virtual void playerback_register_evt_cb(IPTV_PLAYER_EVT_CB pfunc, void *hander);
#ifdef TELECOM_QOS_SUPPORT
    virtual void RegisterParamEvtCb(void *hander, IPTV_PLAYER_PARAM_Evt_e enEvt, IPTV_PLAYER_PARAM_EVENT_CB  pfunc);
#endif
    virtual int playerback_getStatusInfo(IPTV_ATTR_TYPE_e enAttrType, int *value);
    virtual void SwitchAudioTrack_ZTE(PAUDIO_PARA_T pAudioPara);
    virtual void ClearLastFrame();
    virtual void BlackOut(int EarseLastFrame);
    virtual bool SetErrorRecovery(int mode);
    virtual void GetAvbufStatus(PAVBUF_STATUS pstatus);
    virtual void SetVideoHole(int x,int y,int w,int h);
    virtual void writeScaleValue();
    virtual int GetRealTimeFrameRate();
    virtual int GetVideoFrameRate();
    virtual void update_nativewindow();

    virtual int GetVideoDropNumber();
    //virtual void Report_Audio_paramters();
    virtual int GetVideoTotalNumber();
    virtual void GetVideoResolution();
	virtual int GetCurrentVidPTS(unsigned long long *pPTS);
	virtual void GetVideoInfo(int *width, int *height, int *ratio);
	virtual int GetPlayerInstanceNo();
	virtual void ExecuteCmd(const char* cmd_str);
    virtual void readExtractor();
    virtual int updateCTCInfo();
    virtual int SoftWriteData(PLAYER_STREAMTYPE_E type, uint8_t *pBuffer, uint32_t nSize, uint64_t timestamp);
    virtual status_t setDataSource(const char *path, const KeyedVector<String8, String8> *headers = NULL) {return 0;}
    /*end add*/
    bool mIsOmxPlayer;

protected:
    int		m_bLeaveChannel;

private:
    AUDIO_PARA_T a_aPara[MAX_AUDIO_PARAM_SIZE];
    SUBTITLE_PARA_T sPara[MAX_SUBTITLE_PARAM_SIZE];
    VIDEO_PARA_T vPara;
    int player_pid;
    codec_para_t  codec;
    codec_para_t  *pcodec;
    codec_para_t  *vcodec;
    codec_para_t  *acodec;
    codec_para_t  *scodec;
    bool		  m_bIsPlay;
    int			  m_nOsdBpp;
    int			  m_nAudioBalance;
    int			  m_nVolume;
    int           m_nEPGWidth;
    int           m_nEPGHeight;
    bool          m_bFast;
    bool          m_bSetEPGSize;
    bool          m_bWrFirstPkg;
    int	          m_nMode;
    sp<ANativeWindow> mNativeWindow;
    int width_old,width_new;
    int height_old,height_new;

    int frame_rate_ctc;
    int threshold_value;
    int threshold_ctl_flag;
    int underflow_ctc;
    int underflow_kernel;
    int underflow_tmp;
    int underflow_count;
    int qos_count;
    int prev_vread_buffer;
    int vrp_is_buffer_changed;
    int last_data_len;
    int last_data_len_statistics;

    IPTV_PLAYER_EVT_CB pfunc_player_evt;
    void *player_evt_hander;

#ifdef TELECOM_QOS_SUPPORT
    IPTV_PLAYER_PARAM_EVENT_CB  pfunc_player_param_evt;
    void *player_evt_param_handler;
#endif

    unsigned int writecount ;
    int64_t m_StartPlayTimePoint;
    /*+[SE] [BUG][BUG-170677][yinli.xia] added:2s later
        to statistics video frame when start to play*/
    int m_Frame_StartTime_Ctl;
    int64_t m_Frame_StartPlayTimePoint;
    bool    m_isSoftFit;
    FILE*	  m_fp;
    lock_t  mutex;
    pthread_t mThread[2];
    pthread_cond_t m_pthread_cond;
    pthread_cond_t s_pthread_cond;

    pthread_t readThread;
    virtual void checkAbend();
    virtual int checkunderflow();
    virtual void checkunderflow_type();
    virtual void checkBuffLevel();
    virtual void checkBuffstate();
    static void *threadCheckAbend(void *pthis);
    static void *threadReadPacket(void *pthis);

#ifdef TELECOM_QOS_SUPPORT
    virtual int ReportVideoFrameInfo(struct vframe_qos_s * pframe_qos);
#endif

    bool    m_isBlackoutPolicy;
    bool    m_bchangeH264to4k;
    lock_t  mutex_lp;
    lock_t  mutex_session;
    void checkVdecstate();
    bool    m_bIsPause;
    virtual bool iStartPlay( );
    virtual bool iStop( );
    int64_t m_PreviousOverflowTime;
    size_t  mInputQueueSize;
    ctsplayer_state m_sCtsplayerState;
    pthread_t mInfoThread;
    int mLastVdecInfoNum;
    static void * threadReportInfo(void *pthis);
    void update_caton_info(struct av_param_info_t * info);
    void update_stream_bitrate();
    bool CheckMultiSupported(int video_type);
	void * stop_thread(void );
    static void * init_thread(void *pthis);
    void thread_wait_timeUs(int microseconds);
    void thread_wake_up();
};

#endif
