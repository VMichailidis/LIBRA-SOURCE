ROOT := $(realpath ../..)
include $(ROOT)/config.mk

ALG_LOWER = $(shell echo $$((2 * $(LOG_B_CPU) + 2)))
ALG = tLIBRAShuffle
ALG_C_FLAGS = -D TEST_LOG_BLOCK_WIDTH=$(LOG_B_CPU)
ALG_TYPE = COBRA_TYPE
include ../common.mk
