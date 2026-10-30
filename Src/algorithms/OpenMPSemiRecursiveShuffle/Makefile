
ROOT := $(realpath ../..)
include $(ROOT)/config.mk

ALG = OpenMPSemiRecursiveShuffle
ALG_LOWER = 10

RUN_TYPE = $(LOCK_ALL_CORES) taskset --cpu-list 0-$(LAST_CORE)
FLAGS += -fopenmp

include ../common.mk
