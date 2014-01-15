LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE    := libCTC_MediaProcessorjni

LOCAL_SRC_FILES := CTC_MediaProcessor.cpp \
				   Proxy_MediaProcessor.cpp 

LIBPLAYER_PATH := $(TOP)/packages/amlogic/LibPlayer
LOCAL_C_INCLUDES += \
	$(ANDDROID_PLATFORM)/frameworks/native/include \
	$(LIBPLAYER_PATH)/amplayer/player/include \
	$(LIBPLAYER_PATH)/amplayer/control/include \
	$(LIBPLAYER_PATH)/amffmpeg \
	$(LIBPLAYER_PATH)/amcodec/include \
	$(LIBPLAYER_PATH)/amadec/include \
	$(LIBPLAYER_PATH)/amavutils/include \
	$(JNI_H_INCLUDE) \
	$(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES += 	\
    						libandroid_runtime \
    						libnativehelper \
    						libcutils \
    						libutils \
    						libbinder \
    						libmedia\
    						libgui \
							liblog \
							libCTC_MediaProcessor


include $(BUILD_SHARED_LIBRARY)