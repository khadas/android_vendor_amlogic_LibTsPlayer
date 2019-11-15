LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

$(warning $(TOP))
ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

ifeq ($(IPTV_ZTE_SUPPORT),true)
LOCAL_CFLAGS += -IPTV_ZTE_SUPPORT
endif
ifeq ($(TELECOM_VFORMAT_SUPPORT),true)
LOCAL_CFLAGS += -DTELECOM_VFORMAT_SUPPORT
endif
ifeq ($(TELECOM_QOS_SUPPORT),true)
LOCAL_CFLAGS += -DTELECOM_QOS_SUPPORT
endif

endif

LOCAL_CFLAGS += -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_ARM_MODE := arm
LOCAL_MODULE    := libCTC_MediaProcessor
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	CTsPlayer.cpp \
	subtitleservice.cpp \
	CTC_MediaControl.cpp \
	CTC_MediaProcessor.cpp \
	Util.cpp \


ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

LOCAL_SRC_FILES += \
	subtitleservice.cpp \
	CTsOmxPlayer.cpp \

else ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \>= 23))

LOCAL_SRC_FILES += \
	MemoryLeakTrackUtilTmp.cpp \

endif

OS_MAJOR_VER	:= $(shell echo $(PLATFORM_VERSION) | cut -d. -f1)
$(warning $(OS_MAJOR_VER))
ifeq ($(OS_MAJOR_VER),5)
$(warning Lollipop)
LOCAL_CFLAGS	+= -DANDROID5
LOCAL_C_INCLUDES += $(TOP)/external/stlport/stlport
LOCAL_SHARED_LIBRARIES += libstlport
endif
ifeq ($(OS_MAJOR_VER),4)
$(warning Kitkat)
LOCAL_CFLAGS	+= -DANDROID4
LOCAL_C_INCLUDES += $(TOP)/external/stlport/stlport
LOCAL_SHARED_LIBRARIES += libstlport
endif

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

ifneq (,$(wildcard vendor/amlogic/frameworks/av/LibPlayer))
LIBPLAYER_PATH:=$(TOP)/vendor/amlogic/frameworks/av/LibPlayer
SUBTITLE_SERVICE_PATH:=$(TOP)/vendor/amlogic/apps/SubTitle
else
LIBPLAYER_PATH := $(TOP)/packages/amlogic/LibPlayer
SUBTITLE_SERVICE_PATH:=$(TOP)/packages/amlogic/SubTitle
endif

else ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \>= 23))

HARDWARE_PATH := $(TOP)/hardware/amlogic

endif

LOCAL_C_INCLUDES := \
	$(JNI_H_INCLUDE)/ \
	$(LOCAL_PATH)/../include \
	$(TOP)/frameworks/av/ \
	$(TOP)/frameworks/av/media/libstagefright/include \
	$(TOP)/frameworks/native/include/media/openmax \


ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 28 && echo OK),OK)
LOCAL_C_INCLUDES += $(TOP)/vendor/amlogic/common/frameworks/services/subtiltleserver/subtitleServerHidlClient
endif

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

LOCAL_C_INCLUDES += \
	$(LIBPLAYER_PATH)/amplayer/player/include \
	$(LIBPLAYER_PATH)/amplayer/control/include \
	$(LIBPLAYER_PATH)/amffmpeg \
	$(LIBPLAYER_PATH)/amcodec/include \
	$(LIBPLAYER_PATH)/amcodec/amsub_ctl \
	$(LIBPLAYER_PATH)/amadec/include \
	$(LIBPLAYER_PATH)/amavutils/include \
	$(LIBPLAYER_PATH)/amsubdec \
	$(SUBTITLE_SERVICE_PATH)/service \

else ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \>= 23))

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include/android9 \
	$(HARDWARE_PATH)/LibAudio/amadec/include \
	$(HARDWARE_PATH)/media/amcodec/include \
	$(HARDWARE_PATH)/media/amvdec/include \
	$(HARDWARE_PATH)/media/amavutils/include \
	$(TOP)/frameworks/native/libs/nativewindow/include \
	$(HARDWARE_PATH)/gralloc/amlogic \
	$(TOP)/vendor/amlogic/common/external/dvb/include/am_adp \
	$(TOP)/vendor/amlogic/common/external/dvb/android/ndk/include \
	$(TOP)/vendor/verimatrix/iptvclient/include

endif

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

#LOCAL_STATIC_LIBRARIES := libamcodec libamadec libavformat libavcodec libavutil
LOCAL_STATIC_LIBRARIES := libamcodec libamadec

endif

ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 28 && echo OK),OK)
LOCAL_SHARED_LIBRARIES +=vendor.amlogic.hardware.subtitleserver@1.0 libhidltransport libbase libsubtitlebinder
endif

LOCAL_SHARED_LIBRARIES +=libutils libmedia libz libbinder
LOCAL_SHARED_LIBRARIES +=liblog libcutils libdl
LOCAL_SHARED_LIBRARIES +=libgui libui
LOCAL_SHARED_LIBRARIES +=libstagefright libstagefright_foundation libFFExtractor

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

LOCAL_SHARED_LIBRARIES +=libamsubdec libamavutils libamFFExtractor
LOCAL_SHARED_LIBRARIES +=libamplayer libsubtitleservice

else ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \>= 23))

LOCAL_SHARED_LIBRARIES +=libamavutils_sys libamcodec libaudioclient
LOCAL_SHARED_LIBRARIES +=libamgralloc_ext@2 libhidltransport libbase

endif

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

ifeq ($(NEED_AML_CTC_MIDDLE),true)
LOCAL_SHARED_LIBRARIES += libCTC_AmMediaControl libCTC_AmMediaProcessor libffmpeg30
endif

ifeq ($(TARGET_USE_OPTEEOS),true)
LOCAL_SHARED_LIBRARIES += libtelecom_iptv
LOCAL_CFLAGS += -DUSE_OPTEEOS
endif

endif

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE    := ctsplayer
LOCAL_MODULE_TAGS := samples
LOCAL_ARM_MODE := arm
#LOCAL_32_BIT_ONLY := true
LOCAL_SRC_FILES := main.cpp

LIBFFMPEG_PATH := $(TOP)/vendor/amlogic/common/external/ffmpeg
LIBMEDIA_PATH := $(TOP)/hardware/amlogic
SUBTITLE_SERVICE_PATH:=$(TOP)/vendor/amlogic/common/apps/SubTitle
LOCAL_C_INCLUDES := \
         $(JNI_H_INCLUDE)/ \
         $(LOCAL_PATH)/../include \
         $(LIBMEDIA_PATH)/LibAudio/amadec/include \
         $(LIBMEDIA_PATH)/media/amcodec/include \
         $(LIBMEDIA_PATH)/media/amvdec/include \
         $(LIBMEDIA_PATH)/media/amavutils/include \
         $(TOP)/frameworks/av/ \
         $(TOP)/frameworks/av/media/libstagefright/include \
         $(TOP)/frameworks/native/libs/nativewindow/include \
         $(SUBTITLE_SERVICE_PATH)/service \
         $(TOP)/frameworks/native/include/media/openmax \
         $(TOP)/hardware/amlogic/gralloc \
         $(LOCAL_PATH)/../../../../../external/dvb/include/am_adp  \

LOCAL_SHARED_LIBRARIES += libz libbinder  libamcodec  libmedia
LOCAL_SHARED_LIBRARIES +=liblog libcutils libdl liblog  libutils
LOCAL_SHARED_LIBRARIES += libgui  libFFExtractor #libsubtitleservice
LOCAL_SHARED_LIBRARIES += libCTC_MediaProcessor  #libstagefright libstagefright_foundation

include $(BUILD_EXECUTABLE)

