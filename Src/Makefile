.PHONY: clean verify results figures tables
ROOT := $(realpath .)
include config.mk
# TODO: Programmatically create cache misses offset
# TODO: Improve fidelity of data dependencies
# TODO: Remove hardcoded core from GPU Tests
# Top level target, make doc/paper.pdf to run all tests and compile results
clean:
	rm -rf $(BENCH_BIN)/*
	rm -rf $(TEST_BIN)/*
clear-locks:
	rm -rf $(TMP_DIR)/locks/*


ALGORITHMS = $(patsubst $(ALG_DIR)/%/, %,$(filter %/, $(wildcard $(ALG_DIR)/*/)))

#1: Category
#2: ALGORITHM
#3: Bench Type
define IMPORT_RESULT
$(1)_$(2)_$(3)_RESULT:
	@$$(MAKE) --no-print-directory -C $$(ALG_DIR)/$(2) REQ_UPPER=$$($(1)_UPPER) REQ_LOWER=$$($(1)_LOWER) $(3)_result 
endef

#1: Category
#2: Bench Type
define FIG_TEMPLATE
# import targets
$(1)_$(2)_RESULTS =\
	$$(foreach a,$$($(1)),\
		$(1)_$$(a)_$(2)_RESULT\
	)
$$(foreach a,$$($(1)),\
	$$(eval $$(call IMPORT_RESULT,$(1),$$(a),$(2)))\
)

# import results paths
$(1)_$(2)_FILES =\
	$$(foreach a,$$($(1)),\
		$$(shell $$(MAKE) --no-print-directory -C $$(ALG_DIR)/$$(a) REQ_UPPER=$$($(1)_LOWER) REQ_LOWER=$$($(1)_UPPER) get_$(2))\
	)
$$(shell mkdir -p $$(RESULTS_DIR)/$(2))
$$(RESULTS_DIR)/$(2)/$(1)$$(CONFIG_ID).csv: $$($(1)_$(2)_RESULTS)
	$$(RAW2CSV) $$($(1)_$(2)_FILES) > $$@

$$(shell mkdir -p $$(ROOT)/images)

$$(ROOT)/images/$(1)_$(2)$$(CONFIG_ID).png: $$(RESULTS_DIR)/$(2)/$(1)$$(CONFIG_ID).csv
	$$(PLOTTER) --logb_cpu $$(LOG_B_CPU) --logb_cu $$(LOG_B_CU) \
	--data $$(RESULTS_DIR)/$(2)/$(1)$$(CONFIG_ID).csv \
	--runs $$(RUNS) \
	--select $$($(1)) \
	--filename $$@ \
	--range $$($(1)_LOWER) $$($(1)_UPPER) \
	--test_class $(2) \
	--miss_offset 2

$(1)_$(2)_fig: $$(ROOT)/images/$(1)_$(2)$$(CONFIG_ID).png

$$(TMP_DIR)/$(2)/$(1)_$(2)_table.typ: $$(RESULTS_DIR)/$(2)/$(1)$$(CONFIG_ID).csv
	$$(TABLER) \
	--data $$(RESULTS_DIR)/$(2)/$(1)$$(CONFIG_ID).csv \
	--range $$($(1)_LOWER) $$($(1)_UPPER) \
	--denominator $$($(1)_DENOM) \
	--select $$($(1)) \
	--symbol $$($(2)_SYMBOL) \
	--name $(1)_$(2)_table > $$@

$(1)_$(2)_table: $$(TMP_DIR)/$(2)/$(1)_$(2)_table.typ
endef
$(foreach b,$(BENCH_TYPES),\
	$(foreach c,$(CATEGORIES),\
		$(eval $(call FIG_TEMPLATE,$(c),$(b)))\
	)\
)

$(ROOT)/images/visited_over_total.png: $(UTIL_DIR)/visited_over_total.py
	uv run $< $@
visited_over_total: $(ROOT)/images/visited_over_total.png

$(BIN_DIR)/CUDASharedMem: $(UTIL_DIR)/CUDASharedMem.cu
	$(NVCC) $< -o $@
get_CUDA_mem: $(BIN_DIR)/CUDASharedMem
	$<

	
blocksizes_cpu:
	@$(MAKE) --no-print-directory -C $(SRC_DIR)/cache_test $(ROOT)/images/tLIBRAShuffle_blocksizes.png
$(ROOT)/images/tLIBRAShuffle_blocksizes.png: blocksizes_cpu

$(ROOT)/images/THROUGHPUT_V_CACHE.png: $(UTIL_DIR)/plot-throughput-cache-misses.py \
	$(RESULTS_DIR)/cache_misses/LIBRA$(CONFIG_ID).csv \
	$(RESULTS_DIR)/throughput/LIBRA$(CONFIG_ID).csv
	uv run $< \
	--throughput $(RESULTS_DIR)/throughput/LIBRA$(CONFIG_ID).csv \
	--cache_misses $(RESULTS_DIR)/cache_misses/LIBRA$(CONFIG_ID).csv \
	-o $@

throughput_v_cache: $(ROOT)/images/THROUGHPUT_V_CACHE.png

get_config_id:
	@echo $(CONFIG_ID)

FIGURES =\
$(ROOT)/images/LIBRA_throughput$(CONFIG_ID).png\
$(ROOT)/images/LIBRA_cache_misses$(CONFIG_ID).png\
$(ROOT)/images/LIBRA_branch_misses$(CONFIG_ID).png\
$(ROOT)/images/NAIVE_throughput$(CONFIG_ID).png\
$(ROOT)/images/NAIVE_latency$(CONFIG_ID).png\
$(ROOT)/images/CACHED_throughput$(CONFIG_ID).png\
$(ROOT)/images/CACHED_latency$(CONFIG_ID).png\
$(ROOT)/images/OPEN_MP_throughput$(CONFIG_ID).png\
$(ROOT)/images/OPEN_MP_latency$(CONFIG_ID).png\
$(ROOT)/images/CUDA_throughput$(CONFIG_ID).png\
$(ROOT)/images/HIGH_END_RANK_throughput$(CONFIG_ID).png\
$(ROOT)/images/HIGH_END_RANK_latency$(CONFIG_ID).png\
$(ROOT)/images/tLIBRAShuffle_blocksizes.png\
$(ROOT)/images/visited_over_total.png\

figures: $(FIGURES)

TABLES =\
$(TMP_DIR)/throughput/LIBRA_throughput_table.typ\
$(TMP_DIR)/cache_misses/LIBRA_cache_misses_table.typ\
$(TMP_DIR)/branch_misses/LIBRA_branch_misses_table.typ\
$(TMP_DIR)/throughput/NAIVE_throughput_table.typ\
$(TMP_DIR)/latency/NAIVE_latency_table.typ\
$(TMP_DIR)/cache_misses/NAIVE_cache_misses_table.typ\
$(TMP_DIR)/throughput/CACHED_throughput_table.typ\
$(TMP_DIR)/latency/CACHED_latency_table.typ\
$(TMP_DIR)/cache_misses/CACHED_cache_misses_table.typ\
$(TMP_DIR)/throughput/OPEN_MP_throughput_table.typ\
$(TMP_DIR)/latency/OPEN_MP_latency_table.typ\
$(TMP_DIR)/throughput/CUDA_throughput_table.typ


TESTED_ALGS =\
LIBRAShuffle\
CULIBRAShuffle\
tLIBRAShuffle\
OpenMPLIBRAShuffle

RETARGETED_ALGS=\
LIBRAShuffle\
tLIBRAShuffle\

ARCHS := x86 arm64 riscv64

TARGET_x86     := x86_64-linux-gnu
CPU_x86        := x86_64_v3
TARGET_arm64   := aarch64-linux-gnu
CPU_arm64      := baseline
TARGET_riscv64 := riscv64-linux-gnu
CPU_riscv64    := baseline

#1: alg
#2: arch
RETARGET_TARGETS :=
define RETARGET_TEMPLATE
retarget_$(1)_$(2):
	@$$(MAKE) TARGET=$(2) \
	CC="zig c++" FLAGS="-target $$(TARGET_$(2)) -mcpu=$$(CPU_$(2))" \
	--no-print-directory -C \
	$$(ROOT)/algorithms/$(1) \
	$$(ROOT)/bin/$(2)/tests/$(1)_$$(TEST_LOG_N)

RETARGET_TARGETS+=retarget_$(1)_$(2)
endef

$(foreach arch,$(ARCHS),\
	$(foreach alg,$(RETARGETED_ALGS),\
		$(eval $(call RETARGET_TEMPLATE,$(alg),$(arch)))\
	)\
)

retarget: $(RETARGET_TARGETS)

#1: ALG
ifndef TEST_ALG
define TEST_ALG
test_$(1):
	@$$(MAKE) --no-print-directory -C $$(ROOT)/algorithms/$(1) $$(TEST_BIN)/$(1)_$$(TEST_LOG_N)
	$$(TEST_BIN)/$(1)_$$(TEST_LOG_N)
endef
endif
$(foreach alg,$(TESTED_ALGS),\
	$(eval $(call TEST_ALG,$(alg)))\
)

TESTED_BINS =\
$(foreach alg,$(TESTED_ALGS),\
	test_$(alg)\
)

test_algorithms: $(TESTED_BINS)

$(RESULTS_DIR)/tables.typ: $(TABLES)
	@cat $(TABLES) > $@

tables: $(RESULTS_DIR)/tables.typ

poster.pdf: $(FIGURES) $(ROOT)/doc/*.typ $(ROOT)/doc/references.bib $(ROOT)/doc/tables.typ
	@echo $(FIGURES)
	typst compile $(ROOT)/doc/algorithm.typ
algorithm.pdf: $(FIGURES) $(ROOT)/doc/*.typ $(ROOT)/doc/references.bib
	@echo $(FIGURES)
	typst compile $(ROOT)/doc/algorithm.typ
doc/paper.pdf: doc/paper.typ $(FIGURES) $(ROOT)/doc/tables.typ
	typst compile $<
poster: $(ROOT)/doc/poster.pdf
