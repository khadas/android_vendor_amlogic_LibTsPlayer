################################################################################
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS
$(warning Kitkat)
$(warning $(LOCAL_PATH))
LOCAL_SRC_FILES:= test_main.cpp


ifneq (,$(wildcard vendor/amlogic/frameworks/av/LibPlayer))
LIBPLAYER_PATH:=$(TOP)/vendor/amlogic/frameworks/av/LibPlayer
else
LIBPLAYER_PATH := $(TOP)/packages/amlogic/LibPlayer
endif
LOCAL_C_INCLUDES := \
	$(LIBPLAYER_PATH)/amcodec/include \
	$(LIBPLAYER_PATH)/amcodec/amsub_ctl \
	$(JNI_H_INCLUDE)/ \
	$(LOCAL_PATH)/../include \
	$(TOP)/frameworks/av/ \
	$(TOP)/external/ffmpeg \
	$(TOP)/frameworks/av/media/libstagefright/include \
	$(TOP)/frameworks/native/include/media/openmax \

LOCAL_SHARED_LIBRARIES := \
	libCTC_MediaProcessor liblog libbinder libutils libcutils libamffmpeg
LOCAL_CFLAGS += -Wno-multichar

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= ctc_player

include $(BUILD_EXECUTABLE)

##########################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= test_main_s.cpp


ifneq (,$(wildcard vendor/amlogic/frameworks/av/LibPlayer))
LIBPLAYER_PATH:=$(TOP)/vendor/amlogic/frameworks/av/LibPlayer
else
LIBPLAYER_PATH := $(TOP)/packages/amlogic/LibPlayer
endif
LOCAL_C_INCLUDES := \
	$(LIBPLAYER_PATH)/amcodec/include \
	$(LIBPLAYER_PATH)/amcodec/amsub_ctl \
	$(JNI_H_INCLUDE)/ \
	$(LOCAL_PATH)/../include \
	$(TOP)/frameworks/av/ \
	$(TOP)/external/ffmpeg \
	$(TOP)/frameworks/av/media/libstagefright/include \
	$(TOP)/frameworks/native/include/media/openmax \

LOCAL_SHARED_LIBRARIES := \
	libCTC_MediaProcessor liblog libbinder libutils libcutils libamffmpeg
LOCAL_CFLAGS += -Wno-multichar

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= player

include $(BUILD_EXECUTABLE)

###########################################################


