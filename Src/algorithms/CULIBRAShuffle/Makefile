ROOT := $(realpath ../..)
include $(ROOT)/config.mk

CC = $(NVCC)
FLAGS = $(NVCCFLAGS)

ALG = CULIBRAShuffle
ALG_TYPE = COBRA_GPU_TYPE
ALG_C_FLAGS = -D TEST_LOG_BLOCK_WIDTH=$(LOG_B_CU)
ALG_LOWER = $(shell echo $$((2 * $(LOG_B_CU) + 2)))
RUN_TYPE = flock $(LOCK_DIR)/GPU.lock
include ../common.mk
