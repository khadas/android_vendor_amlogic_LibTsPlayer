#define LOG_NDEBUG 0
#define LOG_TAG "am-ffextractor"

#include <stdio.h>
#include <stdlib.h>
#include <utils/Log.h>
#include <utils/Mutex.h>
#include <utils/List.h>
#include <utils/Timers.h>
#include <cutils/properties.h>

// #ifdef __cplusplus
extern "C"
{
// #endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
//#include <libavfilter/avfiltergraph.h>
//#include <libavfilter/buffersink.h>
//#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
// #ifdef __cplusplus
}
// #endif
#include "amffextractor.h"
#define USE_AVFILTER 0
#define PTS_FREQ 90000
#define LOG_LINE() ALOGV("[%s:%d] >", __FUNCTION__, __LINE__);

using namespace android;
unsigned char   *aviobuffer;
AVIOContext     *avio;
uint8_t         *buffer = NULL;
AVFormatContext *pFormatCtx = NULL;
AVDictionary    *optionsDict = NULL;
AVStream 		*pStream;
int stream_changed = 0;
int inited = 0;
int videoStream;
int audioStream;
float a_time_base_ratio = 0.00;
float v_time_base_ratio = 0.00;

AVRational time_base = {1, 1000000};
FILE* am_fp;
static int amprop_dumpfile;
int am_ffextractor_init(int (*read_cb)(void *opaque, uint8_t *buf, int size), MediaInfo *pMi) {
	if (inited)
		return 0;
	static int tmpfileindex = 0;
	char args[512];
	char tmpfilename[1024] = "";
	char value[PROPERTY_VALUE_MAX] = {0};

	property_get("iptv.dumppath", value, "/storage/external_storage/sda");
	sprintf(tmpfilename, "%s/am_Live%d.ts", value, tmpfileindex);
	tmpfileindex++;
	memset(value, 0, PROPERTY_VALUE_MAX);
    property_get("amiptv.dumpfile", value, "0");
    amprop_dumpfile = atoi(value);
	if(amprop_dumpfile)
	am_fp = fopen(tmpfilename, "wb+");

	av_register_all();

	aviobuffer = (unsigned char *) av_malloc(32768);
	if (!aviobuffer) {
		ALOGE("aviobuffer malloc failed");
		goto fail1;
	}
	avio = avio_alloc_context(aviobuffer, 32768, 0, NULL, read_cb, NULL, NULL);
	if (!avio) {
		ALOGE("avio_alloc_context failed");
		av_free(aviobuffer);
		goto fail1;
	}
	pFormatCtx = avformat_alloc_context();
	if (pFormatCtx == NULL) {
		ALOGD("avformat_alloc_context failed");
		goto fail2;
	}

	pFormatCtx->pb = avio;
	pFormatCtx->probesize = 128 * 1024;
	if (avformat_open_input(&pFormatCtx, NULL, NULL, NULL) != 0) {
		ALOGE("Couldn't open input stream.\n");
		goto fail2;
	} else
		ALOGD("avformat_open_input success");

	videoStream = -1;
	audioStream = -1;
	unsigned int i;
	for (i = 0; i < pFormatCtx->nb_streams; i++){
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	}
	ALOGD("nb_streams=%u, videoStream=%d", pFormatCtx->nb_streams, videoStream);
	if(videoStream != -1){
		pStream = pFormatCtx->streams[videoStream];
		ALOGV("has video,den:%d\n",pStream->time_base.den);
		ALOGV("num:%d\n", pStream->time_base.num);
		if (0 != pStream->time_base.den)
			v_time_base_ratio = PTS_FREQ * ((float)pStream->time_base.num / pStream->time_base.den);
			ALOGV("v_time_base_ratio:%f\n",v_time_base_ratio);
	}

	for (i = 0; i < pFormatCtx->nb_streams; i++){
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioStream = i;
			break;
		}
	}
	ALOGD("nb_streams=%u, audioStream=%d", pFormatCtx->nb_streams, audioStream);
	if(audioStream != -1){
		pStream = pFormatCtx->streams[audioStream];
		ALOGV("has audio,den:%d\n",pStream->time_base.den);
		ALOGV("num:%d\n", pStream->time_base.num);
		if (0 != pStream->time_base.den)
			a_time_base_ratio = PTS_FREQ * ((float)pStream->time_base.num / pStream->time_base.den);
			ALOGV("a_time_base_ratio:%f\n",a_time_base_ratio);
	}


	/*
	if(avformat_find_stream_info(pFormatCtx, NULL)<0) {
		return -1; // Couldn't find stream information
	}
	*/
	
/*	ALOGI("width:%d, height:%d", pCodecCtx->width, pCodecCtx->height);
	pMi->width = pCodecCtx->width;
	pMi->height = pCodecCtx->height;*/

	inited = 1;
	av_dump_format(pFormatCtx, 0, "", 0);
	ALOGD("################ffextractor init successful################");
	return 0;

fail3:
	avformat_close_input(&pFormatCtx);
fail2:
	if(avio->buffer != NULL)
		av_free(avio->buffer);
	if(avio != NULL)
		av_free(avio);
	avio = NULL;
fail1:
	return -1;
}

void am_ffextractor_read_packet(void *buffer, int *size, int *index, int64_t *pts) 
{
	AVPacket  packet;
	static int write_number = 0;

	if (!inited) {
		return;
	}
	if( buffer == NULL){
		ALOGV("packet buffer is null\n");
		return;
	}
	packet.stream_index = -1;
	int ret = av_read_frame(pFormatCtx, &packet);
	ALOGV("demux stream_index=%d,size :%d\n",
	packet.stream_index,packet.size);
	if (ret < 0) {
		ALOGD(">>>>>av_read_frame failed<<<<<");
		return;
	}
	if (packet.stream_index == videoStream) {
		*index = 0;
		/*ALOGV("index :%d,pts:%lld,size:%d\n",
			*index,packet.pts,packet.size);*/
		if((am_fp != NULL) && (packet.size > 0)&&(write_number < 100)) {
			if(amprop_dumpfile){
            	fwrite(packet.data, 1, packet.size, am_fp);
				write_number++;
            	ALOGV("nSize[%d] write_number:%d\n", packet.size,write_number);
				if(write_number == 100)
					fclose(am_fp);	
			}
		}
		*pts = (double)packet.pts * (double)v_time_base_ratio;
	}else if (packet.stream_index == audioStream){
		*index = 1;
		*pts = (double)packet.pts * (double)a_time_base_ratio;

	//ALOGV("audio index :%d,pts:%lld,dts:%lld,size:%d\n",*index,packet.pts,packet.dts,packet.size);
	}

	if(packet.size > 0){
		memcpy(buffer,packet.data,packet.size);
		*size = packet.size;	
    }

	av_free_packet(&packet);
}

void am_ffextractor_deinit() {
	if (inited == 0) {
		ALOGW("ffextractor not inited, no need to deinit");
		return;
	}
	if (buffer != NULL)
		av_free(buffer);
	buffer = NULL;
	if(am_fp != NULL) {
        fclose(am_fp);
        am_fp = NULL;
    }
	avformat_close_input(&pFormatCtx);
	
	if(avio->buffer != NULL)
		av_free(avio->buffer);

	if(avio != NULL)
		av_free(avio);
	avio = NULL;
	inited = 0;
	ALOGD("################amffextractor de-init successful################");
}
int64_t getCurrentTimeMs()
{
	return systemTime(SYSTEM_TIME_MONOTONIC) / 1000000;
}
int64_t getCurrentTimeUs()
{
	return systemTime(SYSTEM_TIME_MONOTONIC) / 1000;
}
int checkLogMask() 
{
	char levels_value[92];
	int ret = 0;
	if(property_get("iptv.soft.logmask",levels_value,NULL)>0)
		sscanf(levels_value, "%d", &ret);
	return ret;
}
