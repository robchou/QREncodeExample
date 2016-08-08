LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LIBQRENCODE_SRC_PATH := $(LOCAL_PATH)/libqrencode
LOCAL_MODULE := qrencode
LOCAL_SRC_FILES :=  $(LIBQRENCODE_SRC_PATH)/bitstream.c \
					$(LIBQRENCODE_SRC_PATH)/mask.c      \
					$(LIBQRENCODE_SRC_PATH)/mmask.c     \
					$(LIBQRENCODE_SRC_PATH)/mqrspec.c   \
					$(LIBQRENCODE_SRC_PATH)/qrenc.c     \
					$(LIBQRENCODE_SRC_PATH)/qrencode.c  \
					$(LIBQRENCODE_SRC_PATH)/qrinput.c   \
					$(LIBQRENCODE_SRC_PATH)/qrspec.c    \
					$(LIBQRENCODE_SRC_PATH)/rsecc.c     \
					$(LIBQRENCODE_SRC_PATH)/split.c     \
					qrencode_jni.c
LOCAL_CFLAGS := -Wall -O3
LOCAL_LDLIBS := -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)
