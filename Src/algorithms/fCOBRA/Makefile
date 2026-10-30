ROOT := $(realpath ../..)
include $(ROOT)/config.mk

ALG_LOWER = $(shell echo $$((2 * $(LOG_B_CPU) + 2)))
ALG_UPPER = $(shell echo $$((4 * $(LOG_B_CPU))))
ALG = fCOBRA
ALG_C_FLAGS = -D TEST_LOG_BLOCK_WIDTH=$(LOG_B_CPU)
ALG_TYPE = FCOBRA_TYPE


include ../common.mk
