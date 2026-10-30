ROOT := $(realpath ../..)
include $(ROOT)/config.mk

CC = $(NVCC)
FLAGS = $(NVCCFLAGS)

ALG = CUDATranspose
ALG_TYPE = GPU_TYPE

ALG_LOWER = $(call max,15,$(LOWER))
RUN_TYPE = flock $(LOCK_DIR)/GPU.lock
ALG_C_FLAGS = -D TESTING

include ../common.mk
