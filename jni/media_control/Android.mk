LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(TELECOM_VFORMAT_SUPPORT),true)
LOCAL_CFLAGS += -DTELECOM_VFORMAT_SUPPORT
endif
ifeq ($(IPTV_ZTE_SUPPORT),true)
LOCAL_CFLAGS += -DIPTV_ZTE_SUPPORT
endif
ifeq ($(TELECOM_QOS_SUPPORT),true)
LOCAL_CFLAGS += -DTELECOM_QOS_SUPPORT
endif

LOCAL_ARM_MODE := arm
LOCAL_MODULE    := libCTC_AmMediaControl
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	CTC_AmMediaControl.cpp

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

LOCAL_C_INCLUDES := \
	$(JNI_H_INCLUDE)/ \
	$(LOCAL_PATH)/../include \
	$(TOP)/frameworks/av/include \


LOCAL_SHARED_LIBRARIES += libutils libz libbinder
LOCAL_SHARED_LIBRARIES += liblog libcutils libdl libgui
LOCAL_SHARED_LIBRARIES += libCTC_AmMediaProcessor libffmpeg30


ifeq ($(TARGET_USE_OPTEEOS),true)
LOCAL_SHARED_LIBRARIES += libtelecom_iptv
LOCAL_CFLAGS += -DUSE_OPTEEOS
endif

include $(BUILD_SHARED_LIBRARY)

#######################################################################################
include $(CLEAR_VARS)

ifeq ($(TELECOM_VFORMAT_SUPPORT),true)
LOCAL_CFLAGS += -DTELECOM_VFORMAT_SUPPORT
endif

ifeq ($(TELECOM_QOS_SUPPORT),true)
LOCAL_CFLAGS += -DTELECOM_QOS_SUPPORT
endif

LOCAL_ARM_MODE := arm
LOCAL_MODULE    := libCTC_AmlPlayer
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := CTC_AmlPlayer.cpp


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
ifneq (,$(wildcard vendor/amlogic/frameworks/av/LibPlayer))
LIBPLAYER_PATH:=$(TOP)/vendor/amlogic/frameworks/av/LibPlayer
SUBTITLE_SERVICE_PATH:=$(TOP)/vendor/amlogic/apps/SubTitle
else
LIBPLAYER_PATH := $(TOP)/packages/amlogic/LibPlayer
SUBTITLE_SERVICE_PATH:=$(TOP)/packages/amlogic/SubTitle
endif
LOCAL_C_INCLUDES := \
	$(LIBPLAYER_PATH)/amplayer/player/include \
	$(LIBPLAYER_PATH)/amplayer/control/include \
	$(LIBPLAYER_PATH)/amffmpeg \
	$(LIBPLAYER_PATH)/amcodec/include \
	$(LIBPLAYER_PATH)/amcodec/amsub_ctl \
	$(LIBPLAYER_PATH)/amadec/include \
	$(LIBPLAYER_PATH)/amavutils/include \
	$(LIBPLAYER_PATH)/amsubdec \
	$(JNI_H_INCLUDE)/ \
	$(LOCAL_PATH)/../include \
	$$(TOP)/frameworks/av/ \
	$(SUBTITLE_SERVICE_PATH)/service \
	$(TOP)/frameworks/av/media/libstagefright/include \
	$(TOP)/frameworks/native/include/media/openmax
#LOCAL_STATIC_LIBRARIES := libamcodec libamadec libavformat libavcodec libavutil
LOCAL_STATIC_LIBRARIES := libamcodec libamadec

LOCAL_SHARED_LIBRARIES += libamplayer libutils libmedia libz libbinder libamavutils libamsubdec
LOCAL_SHARED_LIBRARIES +=liblog libcutils libdl libCTC_MediaProcessor
LOCAL_SHARED_LIBRARIES +=libgui libsubtitleservice libui


ifeq ($(TARGET_USE_OPTEEOS),true)
LOCAL_SHARED_LIBRARIES += libtelecom_iptv
LOCAL_CFLAGS += -DUSE_OPTEEOS
endif

include $(BUILD_SHARED_LIBRARY)

#include $(BUILD_EXECUTABLE)
