========================
**libtasking** testsuite
========================

:Author: Michal Olech <michal.olech@students.mimuw.edu.pl>


Prerequisites
-------------

To compile this sources ``binutils``, ``gcc``, ``newlib`` and ``gdb`` for ARM
Cortex-M4 should be available in path and the toolchain prefix environment
variable should be set, e.g.:

    LIBTASKING_TESTSUITE_TOOLCHAIN=arm-eabi-

Tested versions:

========  =====
binutils  2.24
gcc       4.9.2
newlib    2.0.0
gdb       7.8.1
========  =====


Compilation
-----------

To build all tests, run:

    make

To build a specific test, e.g. basic, run:

    make -C tests/basic

To build and program a specific test, run:

    make -C tests/basic prog

To build and perform automatic tests, run:

    make test

To build and perform a specific automatic test, run:

    make -C tests/low_power test

To run performance tests:

    make perf

To generate performance report:

    make perf_report
