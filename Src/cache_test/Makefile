ROOT = $(realpath ..)
include ../config.mk

B_SIZES := $(shell seq 2 10)
ALG := tLIBRAShuffle
ALG_GPU := CULIBRAShuffle
RAW2CSV = uv run $(UTIL_DIR)/b_results2csv.py 
RAW2CSV_GPU = uv run $(UTIL_DIR)/b_results2csv_gpu.py 
PLOTTER = uv run $(ROOT)/util/plot.py

BLOCK_SIZES_IMAGE := $(ROOT)/images/$(ALG)_blocksizes.png
BLOCK_SIZES_CSV := $(ROOT)/results/$(ALG)_blocksizes.csv
BLOCK_SIZES_GPU_IMAGE := $(ROOT)/images/$(ALG)_blocksizes_gpu.png
BLOCK_SIZES_GPU_CSV := $(ROOT)/results/$(ALG)_blocksizes_gpu.csv


#1: bsize
define B_TEST_CPU
size_cpu_$(1):
	@$$(MAKE) --no-print-directory -C $$(ALG_DIR)/$$(ALG) LOG_B_CPU=$(1) $$(TMP_DIR)/throughput/results/$$(ALG)_RUNS$$(RUNS)_LOGBC$(1)_LOGBG$$(LOG_B_CU)
endef

$(foreach b, $(B_SIZES),\
	$(eval $(call B_TEST_CPU,$(b)))\
)
OUT_CPU = $(foreach b, $(B_SIZES),\
	$(TMP_DIR)/throughput/results/$(ALG)_RUNS$(RUNS)_LOGBC$(b)_LOGBG$(LOG_B_CU)\
)\
$(TMP_DIR)/throughput/results/LIBRAShuffle_RUNS$(RUNS)_LOGBC$(LOG_B_CPU)_LOGBG$(LOG_B_CU)
# TODO: repace above with bellow
# $(TMP_DIR)/throughput/results/LIBRAShuffle_RUNS$(RUNS)_LOGBC$(LOG_B_CPU)_LOGBG$(LOG_B_CU)_throughput

SIZES_CPU = $(foreach b, $(B_SIZES),\
	size_cpu_$(b)\
)
$(BLOCK_SIZES_CSV): $(SIZES_CPU) $(UTIL_DIR)/b_results2csv.py
	@echo $(SIZES_CPU)
	$(RAW2CSV) $(OUT_CPU) > $@
csv_cpu: $(BLOCK_SIZES_CSV)
$(BLOCK_SIZES_IMAGE) : $(BLOCK_SIZES_CSV)
	$(PLOTTER) --filename $@ --data $< --test_class throughput --range 15 28
png_cpu: $(BLOCK_SIZES_IMAGE)

#1: bsize
define B_TEST_GPU
size_gpu_$(1):
	@$$(MAKE) --no-print-directory -C $$(ROOT)/src/algorithms/$$(ALG_GPU) LOG_B_CU=$(1) $$(TMP_DIR)/throughput/results/$$(ALG_GPU)_RUNS$$(RUNS)_LOGBC$$(LOG_B_CPU)_LOGBG$(1)_throughput
endef

$(foreach b, $(B_SIZES),\
	$(eval $(call B_TEST_GPU,$(b)))\
)
OUT_GPU = $(foreach b, $(B_SIZES),\
	$(TMP_DIR)/throughput/results/$(ALG_GPU)_RUNS$(RUNS)_LOGBC$(LOG_B_CPU)_LOGBG$(b)_throughput\
)
SIZES_GPU = $(foreach b, $(B_SIZES),\
	size_gpu_$(b)\
)
$(BLOCK_SIZES_GPU_CSV): $(SIZES_GPU)
	@echo $(SIZES_GPU)
	$(RAW2CSV_GPU) $(OUT_GPU) > $@
csv_gpu: $(BLOCK_SIZES_GPU_CSV)
$(BLOCK_SIZES_GPU_IMAGE) : $(BLOCK_SIZES_GPU_CSV)
	$(PLOTTER) --filename $@ --data $< --test_class throughput
png_gpu: $(BLOCK_SIZES_GPU_IMAGE)



# CALCULATE BASELINE CACHE MISSES FOR ANY ALGORITHM
OFFSET := $(ROOT)/results/cache_misses/offset

STUB :=  $(ROOT)/bin/benchmarks/NOOP/cache_misses/NOOP_LOGN8_RUNS$(RUNS)_LOGBC$(LOG_B_CPU)_LOGBG$(LOG_B_CU)

$(STUB):
	@$(MAKE) --no-print-directory -C $(ROOT)/src/algorithms/NOOP $(STUB)

stub: $(STUB)
PREFIX := $(cache_misses_PREFIX)
SUFFIX := $(cache_misses_SUFFIX)
$(OFFSET): $(STUB)
	$(PREFIX) $(STUB) $(SUFFIX) > $@
offset: $(OFFSET)

