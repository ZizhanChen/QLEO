LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    baseline.cpp\
    emd.cpp\
    hist.cpp\
    qoe.cpp\
    ssim.cpp
LOCAL_PROPRIETARY_MODULE := true
LOCAL_HEADER_LIBRARIES := \
   libhardware_headers 
LOCAL_MODULE:= zizhan_baseline
LOCAL_SHARED_LIBRARIES := \
            libc \
			liblog \
			libcutils \
			libsync \
			libutils \
			libhardware_legacy
LOCAL_CFLAGS += -fpermissive
LOCAL_CFLAGS += -Wno-error
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLE
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    crowd_dbs.cpp\
    emd.cpp\
    hist.cpp\
    qoe.cpp\
    ssim.cpp
LOCAL_PROPRIETARY_MODULE := true
LOCAL_HEADER_LIBRARIES := \
   libhardware_headers 
LOCAL_MODULE:= zizhan_crowd_dbs
LOCAL_SHARED_LIBRARIES := \
            libc \
			liblog \
			libcutils \
			libsync \
			libutils \
			libhardware_legacy
LOCAL_CFLAGS += -fpermissive
LOCAL_CFLAGS += -Wno-error
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLE
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    dbs_al.cpp\
    emd.cpp\
    hist.cpp\
    qoe.cpp\
    ssim.cpp
LOCAL_PROPRIETARY_MODULE := true
LOCAL_HEADER_LIBRARIES := \
   libhardware_headers 
LOCAL_MODULE:= zizhan_dbs_al
LOCAL_SHARED_LIBRARIES := \
            libc \
			liblog \
			libcutils \
			libsync \
			libutils \
			libhardware_legacy
LOCAL_CFLAGS += -fpermissive
LOCAL_CFLAGS += -Wno-error
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLE
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    q_learning.cpp\
    emd.cpp\
    hist.cpp\
    qoe.cpp\
    ssim.cpp
LOCAL_PROPRIETARY_MODULE := true
LOCAL_HEADER_LIBRARIES := \
   libhardware_headers 
LOCAL_MODULE:= zizhan_q_learning
LOCAL_SHARED_LIBRARIES := \
            libc \
			liblog \
			libcutils \
			libsync \
			libutils \
			libhardware_legacy
LOCAL_CFLAGS += -fpermissive
LOCAL_CFLAGS += -Wno-error
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLE
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    tm.cpp\
    emd.cpp\
    hist.cpp\
    qoe.cpp\
    ssim.cpp
LOCAL_PROPRIETARY_MODULE := true
LOCAL_HEADER_LIBRARIES := \
   libhardware_headers 
LOCAL_MODULE:= zizhan_tm
LOCAL_SHARED_LIBRARIES := \
            libc \
			liblog \
			libcutils \
			libsync \
			libutils \
			libhardware_legacy
LOCAL_CFLAGS += -fpermissive
LOCAL_CFLAGS += -Wno-error
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLE
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
include $(BUILD_EXECUTABLE)