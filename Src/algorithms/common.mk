.DELETE_ON_ERROR:
.PHONY: get_results OUT_FRAGMENTS
SRC_TYPE ?= cpp
# RUN_TYPE ?= flock $$(LOCK_DIR)/core$(CORE).lock taskset -c $(CORE)
RUN_TYPE ?= bash $(SRC_DIR)/util/run_on_free.sh "$(CORES)" --
ALG_TYPE ?= DEFAULT_TYPE
ALG_C_FLAGS ?=

# Requested range, in order to save unnecessary tests
ALG_LOWER ?= $(LOWER)
ALG_UPPER ?= $(UPPER)
REQ_UPPER ?= $(ALG_UPPER)
REQ_LOWER ?= $(ALG_LOWER)
RANGE = $(shell seq\
	$(call max,$(ALG_LOWER),$(REQ_LOWER))\
	$(call min,$(ALG_UPPER),$(REQ_UPPER))\
)

# Implement cache optimization with bench_prefix and bench_sufix variable that is empty when not needed
#1: N
#2: RUN TYPE
#3: BENCH TYPE
ifndef BENCH_TEMPLATE
define BENCH_TEMPLATE
$$(BENCH_BIN)/$$(ALG)/$(3)/$$(ALG)$$(ALG_FLAGS)_LOGN$(1)$$(CONFIG_ID): $$(UTIL_DIR)/$(3).cpp $$(UTIL_DIR)/$(3).hpp $$(SRC_DIR)/algorithms/$$(ALG)/$$(ALG).hpp $$(UTIL_DIR)/TestUtils.hpp
	@mkdir -p $$(BENCH_BIN)/$$(ALG)/$(3)
	$$(CC) $(FLAGS) $$($(3)_FLAGS)\
		-D ALG=$$(ALG)\
		-D DATATYPE=$$(DATATYPE)\
		-D RUNS=$$(RUNS)\
		-D QUANT=$$(QUANT)\
		-D TEST_LOG_N=$(1)\
		-D TESTING\
		-D $$(ALG_TYPE)\
		$$(ALG_C_FLAGS)\
		-include $$(SRC_DIR)/algorithms/$$(ALG)/$$(ALG).hpp\
		 -o $$@\
		$$(UTIL_DIR)/$(3).cpp 

$$(TMP_DIR)/$(3)/fragments/$$(ALG)_LOGN$(1)$$(CONFIG_ID).out: $$(BENCH_BIN)/$$(ALG)/$(3)/$$(ALG)$$(ALG_FLAGS)_LOGN$(1)$$(CONFIG_ID)
	@mkdir -p $$(TMP_DIR)/$(3)/fragments
	$(2) $$($(3)_PREFIX) $$< $$($(3)_SUFFIX) &> $$@
endef
endif

$(foreach b, $(BENCH_TYPES),\
	$(foreach n, $(RANGE),\
		$(eval $(call BENCH_TEMPLATE,$(n),$(RUN_TYPE),$(b)))\
	)\
)

#1: Bench Type
ifndef BENCH_TYPE_TEMPLATE
define BENCH_TYPE_TEMPLATE
OUT_FRAGMENTS_$(1) =\
	$$(foreach n,$$(RANGE),\
		$$(TMP_DIR)/$(1)/fragments/$$(ALG)_LOGN$$(n)$$(CONFIG_ID).out\
	)

$$(TMP_DIR)/$(1)/results/$$(ALG)$$(CONFIG_ID): $$(OUT_FRAGMENTS_$(1)) 
	@mkdir -p $$(TMP_DIR)/$(1)/results
	@cat $$(OUT_FRAGMENTS_$(1)) > $$@

$(1)_result: $$(TMP_DIR)/$(1)/results/$$(ALG)$$(CONFIG_ID)

get_$(1): 
	@echo $$(TMP_DIR)/$(1)/results/$$(ALG)$$(CONFIG_ID)
endef
endif

$(TEST_BIN)/$(ALG)_$(TEST_LOG_N): $(UTIL_DIR)/Verify.cpp $(UTIL_DIR)/Verify.hpp $(SRC_DIR)/algorithms/$(ALG)/$(ALG).hpp
	@mkdir -p $(TEST_BIN)
	$(CC) $(FLAGS) \
	-D SHUFFLER=$(ALG) \
	-D DATATYPE=$(DATATYPE) \
	-D TEST_LOG_N=$(TEST_LOG_N) \
	-D $(ALG_TYPE) \
	$(ALG_C_FLAGS) \
	-include $(SRC_DIR)/algorithms/$(ALG)/$(ALG).hpp \
	-o $@ \
	$(UTIL_DIR)/Verify.cpp


$(foreach b,$(BENCH_TYPES),\
	$(eval $(call BENCH_TYPE_TEMPLATE,$(b)))\
)
debug:
	@echo $(OUT_FRAGMENTS_throughput)

