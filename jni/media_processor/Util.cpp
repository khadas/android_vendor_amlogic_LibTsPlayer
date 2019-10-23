#include "Util.h"
#include <stdlib.h>
#if ANDROID_PLATFORM_SDK_VERSION <= 27
#else
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#endif
#include <fcntl.h>
#include <android/log.h>
#include <sys/system_properties.h>
#include <cutils/properties.h>

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "CTC_Util", __VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , "CTC_Util", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "CTC_Util", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , "CTC_Util", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "CTC_Util", __VA_ARGS__)

OUTPUT_MODE get_display_mode()
{
    int fd;
    char mode[16] = {0};
    char path[64] = {"/sys/class/display/mode"};
    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        memset(mode, 0, 16); // clean buffer and read 15 byte to avoid strlen > 15	
        read(fd, mode, 15);
        mode[strlen(mode)] = '\0';
        close(fd);
        if(!strncmp(mode, "480i", 4) || !strncmp(mode, "480cvbs", 7)) {
            return OUTPUT_MODE_480I;
        } else if(!strncmp(mode, "480p", 4)) {
            return OUTPUT_MODE_480P;
        } else if(!strncmp(mode, "576i", 4) || !strncmp(mode, "576cvbs", 7)) {
            return OUTPUT_MODE_576I;
        } else if(!strncmp(mode, "576p", 4)) {
            return OUTPUT_MODE_576P;
        } else if(!strncmp(mode, "720p", 4)) {
            return OUTPUT_MODE_720P;
        } else if(!strncmp(mode, "1080i", 5)) {
            return OUTPUT_MODE_1080I;
        } else if(!strncmp(mode, "1080p", 5)) {
            return OUTPUT_MODE_1080P;
        } else if(!strncmp(mode, "2160p", 5)) {
            return OUTPUT_MODE_4K2K;
        } else if(!strncmp(mode, "smpte", 5)) {
            return OUTPUT_MODE_4K2KSMPTE;
        }
    } else {
        LOGE("get_display_mode open file %s error\n", path);
    }
    return OUTPUT_MODE_720P;
}

void getPosition(OUTPUT_MODE output_mode, int *x, int *y, int *width, int *height)
{
    char vaxis_newx_str[PROPERTY_VALUE_MAX] = {0};
    char vaxis_newy_str[PROPERTY_VALUE_MAX] = {0};
    char vaxis_width_str[PROPERTY_VALUE_MAX] = {0};
    char vaxis_height_str[PROPERTY_VALUE_MAX] = {0};

    switch(output_mode) {
    case OUTPUT_MODE_480I:
        property_get("ubootenv.var.480i_x", vaxis_newx_str, "0");
        property_get("ubootenv.var.480i_y", vaxis_newy_str, "0");
        property_get("ubootenv.var.480i_w", vaxis_width_str, "720");
        property_get("ubootenv.var.480i_h", vaxis_height_str, "480");
        break;
    case OUTPUT_MODE_480P:
        property_get("ubootenv.var.480p_x", vaxis_newx_str, "0");
        property_get("ubootenv.var.480p_y", vaxis_newy_str, "0");
        property_get("ubootenv.var.480p_w", vaxis_width_str, "720");
        property_get("ubootenv.var.480p_h", vaxis_height_str, "480");
        break;
    case OUTPUT_MODE_576I:
        property_get("ubootenv.var.576i_x", vaxis_newx_str, "0");
        property_get("ubootenv.var.576i_y", vaxis_newy_str, "0");
        property_get("ubootenv.var.576i_w", vaxis_width_str, "720");
        property_get("ubootenv.var.576i_h", vaxis_height_str, "576");
        break;
    case OUTPUT_MODE_576P:
        property_get("ubootenv.var.576p_x", vaxis_newx_str, "0");
        property_get("ubootenv.var.576p_y", vaxis_newy_str, "0");
        property_get("ubootenv.var.576p_w", vaxis_width_str, "720");
        property_get("ubootenv.var.576p_h", vaxis_height_str, "576");
        break;
    case OUTPUT_MODE_720P:
        property_get("ubootenv.var.720p_x", vaxis_newx_str, "0");
        property_get("ubootenv.var.720p_y", vaxis_newy_str, "0");
        property_get("ubootenv.var.720p_w", vaxis_width_str, "1280");
        property_get("ubootenv.var.720p_h", vaxis_height_str, "720");
        break;
    case OUTPUT_MODE_1080I:
        property_get("ubootenv.var.1080i_x", vaxis_newx_str, "0");
        property_get("ubootenv.var.1080i_y", vaxis_newy_str, "0");
        property_get("ubootenv.var.1080i_w", vaxis_width_str, "1920");
        property_get("ubootenv.var.1080i_h", vaxis_height_str, "1080");
        break;
    case OUTPUT_MODE_1080P:
        property_get("ubootenv.var.1080p_x", vaxis_newx_str, "0");
        property_get("ubootenv.var.1080p_y", vaxis_newy_str, "0");
        property_get("ubootenv.var.1080p_w", vaxis_width_str, "1920");
        property_get("ubootenv.var.1080p_h", vaxis_height_str, "1080");
        break;
    case OUTPUT_MODE_4K2K:
        property_get("ubootenv.var.4k2k_x", vaxis_newx_str, "0");
        property_get("ubootenv.var.4k2k_y", vaxis_newy_str, "0");
        property_get("ubootenv.var.4k2k_w", vaxis_width_str, "3840");
        property_get("ubootenv.var.4k2k_h", vaxis_height_str, "2160");
        break;
    case OUTPUT_MODE_4K2KSMPTE:
        property_get("ubootenv.var.4k2ksmpte_x", vaxis_newx_str, "0");
        property_get("ubootenv.var.4k2ksmpte_y", vaxis_newy_str, "0");
        property_get("ubootenv.var.4k2ksmpte_w", vaxis_width_str, "4096");
        property_get("ubootenv.var.4k2ksmpte_h", vaxis_height_str, "2160");
        break;
    default:
    	  *x = 0;
    	  *y = 0;
    	  *width = 1280;
    	  *height = 720;
        LOGW("UNKNOW MODE:%d", output_mode);
        return;
    }
    *x = atoi(vaxis_newx_str);
    *y = atoi(vaxis_newy_str);
    *width = atoi(vaxis_width_str);
    *height = atoi(vaxis_height_str);
}
