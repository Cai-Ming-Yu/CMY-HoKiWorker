LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(LOCAL_PATH)/hokiworker.cpp  \
  $(LOCAL_PATH)/../../../thirdparty/miniz/miniz.c \
  $(wildcard $(LOCAL_PATH)/../../../thirdparty/zstd/lib/common/*.c) \
  $(wildcard $(LOCAL_PATH)/../../../thirdparty/zstd/lib/compress/*.c) \
  $(wildcard $(LOCAL_PATH)/../../../thirdparty/zstd/lib/decompress/*.c) \
  $(LOCAL_PATH)/../../../thirdparty/zstd/lib/decompress/huf_decompress_amd64.S \
  $(LOCAL_PATH)/../../../thirdparty/tiny-AES-c/aes.c \
  $(LOCAL_PATH)/../../../thirdparty/cpp-base64/base64.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../ \
  $(LOCAL_PATH)/../../../thirdparty/zlib/ \
  $(LOCAL_PATH)/../../../thirdparty/zstd/lib \
  $(LOCAL_PATH)/../../../thirdparty/

LOCAL_MODULE := HoKiWorker

LOCAL_MODULE_FILENAME := hokiworker

COMMAND := -static -Wl,--strip-all -Os -fPIC -pipe -g0 -flto=thin -Wl,--gc-sections -ffunction-sections -fdata-sections -falign-functions -falign-loops -Wl,--sort-section=alignment -fvisibility=hidden -ffast-math -fno-stack-protector -fno-builtin -fmerge-all-constants -fmerge-constants -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-strict-aliasing -fno-rtti -fno-exceptions -fomit-frame-pointer -fno-stack-check -fms-extensions -fdeclspec -fexceptions

LOCAL_CFLAGS := $(COMMAND) -std=c89
LOCAL_CPPFLAGS := -std=c++17
LOCAL_LDFLAGS := $(COMMAND)

include $(BUILD_EXECUTABLE)