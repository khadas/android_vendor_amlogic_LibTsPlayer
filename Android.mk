LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE    := libCTC_MediaProcessor
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	CTsPlayer.cpp \
	CTC_MediaControl.cpp \
	CTC_MediaProcessor.cpp

LIBPLAYER_PATH := $(TOP)/packages/amlogic/LibPlayer
LOCAL_C_INCLUDES := \
	$(LIBPLAYER_PATH)/amplayer/player/include \
	$(LIBPLAYER_PATH)/amplayer/control/include \
	$(LIBPLAYER_PATH)/amffmpeg \
	$(LIBPLAYER_PATH)/amcodec/include \
	$(LIBPLAYER_PATH)/amadec/include \
	$(JNI_H_INCLUDE)/ \
	$(LOCAL_PATH)/../include
#LOCAL_STATIC_LIBRARIES := libamcodec libamadec libavformat libavcodec libavutil 
LOCAL_STATIC_LIBRARIES := libamcodec libamadec 

LOCAL_SHARED_LIBRARIES += libamplayer libutils libmedia libz libbinder libamavutils
LOCAL_SHARED_LIBRARIES +=liblog libcutils libdl
LOCAL_SHARED_LIBRARIES +=libgui

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)
