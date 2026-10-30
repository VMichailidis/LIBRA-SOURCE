ROOT := $(realpath ../..)
include $(ROOT)/config.mk

ALG = OpenMPLIBRAShuffle
ALG_C_FLAGS = -D TEST_LOG_BLOCK_WIDTH=$(LOG_B_CPU)
ALG_TYPE = FCOBRA_TYPE

ALG_LOWER = $(shell echo $$((2 * $(LOG_B_CPU) + 2)))
RUN_TYPE = $(LOCK_ALL_CORES) taskset --cpu-list 0-$(LAST_CORE)
FLAGS += -fopenmp

include ../common.mk
