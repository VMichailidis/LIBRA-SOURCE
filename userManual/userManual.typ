#import "@preview/faithful-acmart:0.2.0": *
#let title = "Algorithm XXXX: LIBRA: A New, Portable And Parallelizable Approach To Perform The Bit-Reverse Shuffle"
#show: acmart.with(
  format: "acmlarge",
  title: title,

  // These publication details are placeholders; use the values supplied by your venue.
  journal: "TOMS",
  acm-volume: 1,
  acm-number: 1,
  acm-article: 1,
  acm-year: 2026,
  acm-month: 7,
  doi: "10.1145/nnnnnnn.nnnnnnn",
  copyright: "acmlicensed",

  authors: (
    (
      name: "Vasileios Michailidis",
      // note: [Both authors contributed equally.],
      email: "michailid.v@ac.upatras.gr",
    ),
  ),
  abstract: [
    This user guide documents the usage of the various implementations of the LIBRA Bit-Reverse Shuffle algorithm as described in our article "#title".

  ],
  keywords: ("ACM", "Typst", "paper template"),
)
#show math.equation: set text(font: "New Computer Modern Math")

= Introduction
The Bit-Reverse Shuffle algorithm described in our article "#title" is implemented twice in C++, once in OpenMP and once in CUDA.
Each Implementation is packaged as a portable single file header that can found in Src/algorihtms.
This manual is concerned with `LIBRAShuffle`, `tLIBRAShuffle`, `CULIBRAShuffle`, `OpenMPLIBRAShuffle` and how they may be benchmarked and integrated into a codebase.
= Example Code and Correctness Verification
In order to illustrate how to use the included header files, this archive comes with a rudimentary verification program that tests the correctness of each implementation.
== Verifying the implementations for an arbitrary problem size
In order to test all implementations for the default problem size which is $2^16$ elements run the following command
```bash
cd Src
make test_algorithms
```
To change the problem size to $2^"TEST_LOG_N"$, modify the parameter `TEST_LOG_N`
```bash
make TEST_LOG_N=20 test_algorithms
```
Note that all the algorithms other than `LIBRAShuffle` do not work with problem sizes smaller than their buffer size.
The default buffer size is $2^14$ elements, therefore the implementations cannot be run for smaller problem sizes.

== Integrating LIBRA in C++ codebases
Each provided header file defines a templated class with an `apply` method.
Usage of each class is illustrated in `Src/util/Verify.cpp` and `Src/util/Verify.hpp`.
In the cases of `LIBRAShuffle` and `tLIBRAShuffle`, the user may include the headers as they are.
In the case of `OpenMPLIBRAShuffle` the user needs to add the `-fopenmp` flag when compiling it.
Lastly, in the case of `CULIBRAShuffle` a CUDA compatible accelerator should be available to the host and `nvcc` should be used for compilation.
The included Makefiles document the compilation and execution of the included implementations in complete detail.

= Tuning the buffer size
Other than `LIBRAShuffle`, all the other implementations operate in chunks that fit inside the L1 cache of the executing processor.
In the accompanying paper we show that in order to achieve maximum throughput the user needs to select the maximum buffer size that fits inside the L1 cache.
Because the buffer size is an even power of two, controlling in requires changing the parameter `LOG_B_CPU` which controls the buffer size with the following relation:
$
  "Buffer Size" = 2^(2 times "LOG_B_CPU")
$
Finding the size of the L1 cache in a Unix system can be achieved by querying `/sys/devices/cpu0/cache`.
First, the user needs to find the index corresponding with the L1 data cache.
By querying `/sys/devices/cpu0/cache/index<n>/type` and `/sys/devices/cpu0/cache/index<n>/level` the user may identify the type of cache at index `<n>`.
```bash
$ cat /sys/devices/system/cpu/cpu0/cache/index0/level
1
$ cat /sys/devices/system/cpu/cpu0/cache/index0/type
Data
```
The above listing illustrates that index0 of cpu0 is the L1 data cache.
Then the size of the cache can be queried at `/sys/devices/cpu0/cache/index<n>/type`
```bash
$ cat /sys/devices/system/cpu/cpu0/cache/index0/size
48K
```
The value of `LOG_B_CPU` can be derived via the following relation:
$
  std.math.floor((log_2("Size"_"L1" / "Size"_"element")) / 2)
$

Similarly the buffer size can be tuned for CUDA devices with the following caveat.
The size of the cache available to each block of threads in a CUDA processor is derived via the following relation
$
  "Size"_"L1" = ("Shared mem per block") /("Max blocks per SM")
$
The provided target make get_CUDA_mem, queries the host's device for the aforementioned parameters.
The may derive the value of the parameter `LOG_B_CU` in a fashion similar to `LOG_B_CPU` and edit `Src/config.mk` appropriately.

= Reproducing results
The results published in the accompanying paper were generated with the help of a collection of makefiles that handle compiling, running, processing and plotting the required tests.
In order to generate the figures and tables attached in the original paper run
```bash
make figures
```
and
```bash
make tables
```
respectively.

== Running custom benchmarks
The aforementioned makefile system allows running custom tests.
In order to run a custom test the user must select the following:
1. *Type of benchmark:* The user needs to select between the supported benchmarks: throughput, latency, branch misses and cache misses.
2. *Interval of problem sizes:* Because problem sizes need to be a power of two, problem sizes are selected via their logarithm. The interval (4, 28) in this case is the interval of powers of two between $2^4 = 16$ and $2^28 = 268435456$.
3. *Algorithm or set of algorithms:* If the user needs the results of only a single algorithm, they may generate them be requesting the corresponding file with make.
In order to generate the throughput results of `tLIBRAShuffle` in the default allowed interval of that algorithm run:
```bash
cd algorithms/tLIBRAShuffle
make <path to Src>/tmp/x86/throughput/results/tLIBRAShuffle_RUNS1000_LOGBC7_LOGBG5
```
In order to benchmark and compare a set of algorithms, the user needs to first define a new category for testing and register it in `Src/config.mk`
For example the category of cached algorithms is defined with the following lines residing in `Src/config.mk`:
```Makefile
# Upper bound of tests
CACHED_LOWER = $(shell echo $$((2 * $(LOG_B_CPU) + 2)))
# Upper bound of tests
CACHED_UPPER = 28
# The algorithm that all other algorithms are compared against
CACHED_DENOM = tLIBRAShuffle
# The set of selected algorithms
CACHED =\
COBRAShuffle\
fCOBRA\
tLIBRAShuffle\
SemiRecursiveShuffle\
RecursiveShuffle
```
Registering a category simply requires appending the category name to the list of categories anywhere in `Src/config.mk`
```Makefile
CATEGORIES += <category name>
```
To run any of the preset benchmarks on the new category and generate the associated figure run:
```bash
make <category name>_<benchmark>_fig
```
In order to generate the associated table run:
```bash
make <category name>_<benchmark>_table
```
The raw data can also be generated with
```bash
make <path to Src>/results/x86/<benchmark>/<category name><config_id>.csv
```

== Testing Latency on different architectures
The implementation of the latency benchmark empties the L1 cache at every iteration in using inline x86 assembly.
If the user wishes to compile the tests for other architectures, first an architecture compliant implementation of `make_icache_evictor`, defined in `Src/util/latency.cpp`

== Parallel Benchmarking
Because benchmarks are dispatched by a unix-like operating system, the testing harness needs to manually bind one benchmark per processor at a time in order to ensure reliable results.
However, running benchmarks one at a time when multiple processors are available is inefficient.
The testing harness provisions locks for each benchmarking resource, namely the available cores and the GPU, in order to ensure that each running test has its own exclusive core to run on.
The user in turn can define the number of available cores in `config.mk` by tuning the parameter `LAST_CORE` which is the greatest available core id that will be used.
After `LAST_CORE` is set, the number of jobs that make can spawn can be set to an arbitrary amount via the `-j` flag without affecting test integrity.

= Using Nix Flakes for reproducible environments
In order to ensure reproducibility, all programs used to compile and run the tests included in this archive have had their versions pinned with the help of the nix package manager #ref(<FlakesOfficialNixOS>).
After installing the nix package manager, available on Linux and MacOS, run ```bash nix develop``` to enter a reproducible shell with all the appropriate dependencies installed.

= Verifying Portability
The two single core algorithms (`tLIBRAShuffle` and `LIBRAShuffle`) have been compiled for x86, ARM and RISC-V targets via the zig compiler's retargeting option.
Run the following command to produce the retargeted binaries:

```bash
make retarget
```

#bibliography("references.bib")
