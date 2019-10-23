################################################################################
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
$(warning Kitkat)
$(warning $(LOCAL_PATH))

LOCAL_SRC_FILES:= test_main.cpp

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

ifneq (,$(wildcard vendor/amlogic/frameworks/av/LibPlayer))
LIBPLAYER_PATH:=$(TOP)/vendor/amlogic/frameworks/av/LibPlayer
else
LIBPLAYER_PATH := $(TOP)/packages/amlogic/LibPlayer
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

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

LOCAL_C_INCLUDES += \
	$(LIBPLAYER_PATH)/amcodec/include \
	$(LIBPLAYER_PATH)/amcodec/amsub_ctl \
	$(LIBPLAYER_PATH)/amavutils/include \
	$(TOP)/external/ffmpeg \

else ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \>= 23))

LOCAL_C_INCLUDES += \
	$(HARDWARE_PATH)/media/amcodec/include \
	$(HARDWARE_PATH)/media/amavutils/include \
	$(TOP)/vendor/amlogic/common/external/ffmpeg \
#	$(TOP)/vendor/amlogic/common/iptvmiddlewave/AmIptvMedia/contrib/ffmpeg40 \

endif

LOCAL_SHARED_LIBRARIES :=		\
	libCTC_MediaProcessor		\
	libstagefright_foundation	\
	libui						\
	libgui						\
	liblog						\
	libbinder					\
	libutils					\
	libcutils					\

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

LOCAL_SHARED_LIBRARIES += libamffmpeg	\

else ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \>= 23))

LOCAL_SHARED_LIBRARIES += libffmpeg30	\

endif

LOCAL_CFLAGS += -Wno-multichar

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= ctc_player

include $(BUILD_EXECUTABLE)

##########################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= test_main_s.cpp

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

ifneq (,$(wildcard vendor/amlogic/frameworks/av/LibPlayer))
LIBPLAYER_PATH:=$(TOP)/vendor/amlogic/frameworks/av/LibPlayer
else
LIBPLAYER_PATH := $(TOP)/packages/amlogic/LibPlayer
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

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

LOCAL_C_INCLUDES +=	\
	$(LIBPLAYER_PATH)/amcodec/include \
	$(LIBPLAYER_PATH)/amcodec/amsub_ctl \
	$(LIBPLAYER_PATH)/amavutils/include \
	$(TOP)/external/ffmpeg \

else ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \>= 23))

LOCAL_C_INCLUDES +=	\
	$(HARDWARE_PATH)/media/amcodec/include				\
	$(HARDWARE_PATH)/media/amavutils/include			\
	$(TOP)/frameworks/av/media/libstagefright/include	\
	$(TOP)/frameworks/native/libs/nativewindow/include	\
	$(TOP)/vendor/amlogic/common/iptvmiddlewave/AmIptvMedia/contrib/ffmpeg40 \

endif

LOCAL_SHARED_LIBRARIES := \
	libCTC_MediaProcessor	\
	liblog	\
	libbinder	\
	libutils	\
	libcutils	\

ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \= 19))

LOCAL_SHARED_LIBRARIES += libamffmpeg	\

else ifeq (1, $(shell expr $(PLATFORM_SDK_VERSION) \>= 23))

LOCAL_SHARED_LIBRARIES += \
	libhidltransport \
	libffmpeg30 \
	libgui		\

endif

LOCAL_CFLAGS += -Wno-multichar

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= player

include $(BUILD_EXECUTABLE)

###########################################################


