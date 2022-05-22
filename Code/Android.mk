LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	RL_test.c

LOCAL_SHARED_LIBRARIES := \
	libcutils

LOCAL_MODULE:= test-RL

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
