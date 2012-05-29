LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := TsPlayer.cpp


LOCAL_C_INCLUDES := $(LOCAL_PATH)  \
                     $(LOCAL_PATH)/../amcodec/codec  \
		     $(LOCAL_PATH)/../amcodec/audio_ctl \
		     $(LOCAL_PATH)/../amadec/include \
		     $(LOCAL_PATH)/../amcodec/include \

										
LOCAL_ARM_MODE := arm
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libmedia  libdl
LOCAL_SHARED_LIBRARIES += libbinder

LOCAL_LDLIBS    := -lc -landroid_runtime
LOCAL_STATIC_LIBRARIES :=  libamcodec libamadec
LOCAL_MODULE    := libTsPlayer
#LOCAL_CFLAGS=-ldl
LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)
