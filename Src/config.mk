TARGET ?= x86
CC = g++
FLAGS ?= -std=c++17 -O2 -ffast-math -march=native 
NVCC = nvcc
NVCCFLAGS = -Wno-deprecated-gpu-targets -g -diag-suppress 550 -x cu
BIN_DIR = $(ROOT)/bin/$(TARGET)
SRC_DIR = $(ROOT)
TMP_DIR = $(ROOT)/tmp/$(TARGET)
LOCK_DIR = $(ROOT)/locks
RESULTS_DIR = $(ROOT)/results/$(TARGET)
UTIL_DIR = $(ROOT)/util
ALG_DIR = $(ROOT)/algorithms
BENCH_SRC = $(ROOT)/benchmarks
BENCH_BIN = $(BIN_DIR)/benchmarks
TEST_BIN = $(BIN_DIR)/tests
PLOTTER = $(ROOT)/util/plot.py
TABLER = $(ROOT)/util/tables.py

$(shell mkdir -p $(LOCK_DIR))

LAST_CORE = 7
CORES = $(shell seq 0 $(LAST_CORE))
LOCK_ALL_CORES = $(foreach n,$(shell seq 0 $(LAST_CORE)), flock $(LOCK_DIR)/core$(n).lock)

ALG2BENCH = uv run $(UTIL_DIR)/generate_benchmarks.py
RAW2CSV ?= uv run $(UTIL_DIR)/results2csv.py

# BENCHMARKS
DATATYPE = char
LOWER = 4
UPPER = 28
RUNS ?= 1000
QUANT = 0.025
LOG_B_CPU ?= 7
LOG_B_CU ?= 5
RECURS = 1
CONFIG_ID =_RUNS$(RUNS)_LOGBC$(LOG_B_CPU)_LOGBG$(LOG_B_CU)
TEST_LOG_N ?= 6

min = $(shell [ $(1) -lt $(2) ] && echo $(1) || echo $(2))
max = $(shell [ $(1) -lt $(2) ] && echo $(2) || echo $(1))


# Helper categories
OPEN_MP_LOWER = 16
OPEN_MP_UPPER = 28
OPEN_MP_DENOM = OpenMPLIBRAShuffle
OPEN_MP =\
OpenMPfCOBRA\
OpenMPLIBRAShuffle\
OpenMPSemiRecursiveShuffle

CUDA_LOWER = 15
CUDA_UPPER = 28
CUDA_DENOM = CULIBRAShuffle 
CUDA =\
CUDANaive\
CUDAReverse\
CULIBRAShuffle

CACHED_LOWER = $(shell echo $$((2 * $(LOG_B_CPU) + 2)))
CACHED_UPPER = 28
CACHED_DENOM = tLIBRAShuffle 
CACHED =\
COBRAShuffle\
fCOBRA\
tLIBRAShuffle\
SemiRecursiveShuffle\
RecursiveShuffle\

NAIVE_LOWER =  8
NAIVE_UPPER = 15
NAIVE_DENOM = LIBRAShuffle
NAIVE = \
LIBRAShuffle\
LocalPairwiseShuffle\
UnrolledShuffle\
NaiveShuffle\
XORShuffle\
TableShuffle

LIBRA_LOWER = 4
LIBRA_UPPER = 28
LIBRA_DENOM = LIBRAShuffle
LIBRA = LIBRAShuffle

ACROSS_LOWER = 4
ACROSS_UPPER = 28
ACROSS_DENOM = tLIBRAShuffle
ACROSS = LIBRAShuffle\
tLIBRAShuffle\
XORShuffle\
COBRAShuffle\
UnrolledShuffle

HIGH_END_RANK_LOWER = $(CACHED_LOWER)
HIGH_END_RANK_UPPER = $(CACHED_UPPER)
HIGH_END_RANK_DENOM = tLIBRAShuffle
HIGH_END_RANK = LIBRAShuffle\
tLIBRAShuffle\
XORShuffle\
NaiveShuffle\
COBRAShuffle\
RecursiveShuffle\
SemiRecursiveShuffle\
fCOBRA


CATEGORIES = OPEN_MP CUDA CACHED NAIVE LIBRA ACROSS HIGH_END_RANK

ALG_TYPES = \
	DEFAULT_TYPE\
	COBRA_TYPE\
	FCOBRA_TYPE\
	SEMI_REC_TYPE\
	GPU_TYPE\
	COBRA_GPU_TYPE
# Benchmark types
BENCH_TYPES = \
	throughput\
	latency\
	cache_misses\
	branch_misses

throughput_SYMBOL = T
latency_SYMBOL = L

cache_misses_SYMBOL = C
CPU_CACHE_SIZE = 49152
CPU_CACHE_ASSOC = 12
CPU_CACHE_LINE = 64
CPU_CACHE_CONFIG = $(CPU_CACHE_SIZE),$(CPU_CACHE_ASSOC),$(CPU_CACHE_LINE)
cache_misses_PREFIX = valgrind --max-threads=50000 --instr-at-start=no --tool=cachegrind --cache-sim=yes --D1=$(CPU_CACHE_CONFIG)
cache_misses_SUFFIX = 2>&1 | bash $(UTIL_DIR)/cache_parse.sh
cache_misses_FLAGS = -g

branch_misses_SYMBOL = B
branch_misses_PREFIX = valgrind --max-threads=50000 --instr-at-start=no --tool=cachegrind --branch-sim=yes --D1=$(CPU_CACHE_CONFIG)
branch_misses_SUFFIX = 2>&1 | bash $(UTIL_DIR)/branch_parse.sh
branch_misses_FLAGS = -g
