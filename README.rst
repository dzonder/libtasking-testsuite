========================
**libtasking** testsuite
========================

:Author: Michal Olech <michal.olech@gmail.com>
:License: MIT


The test suite is run on a platform containing Kinetis Cortex-M4 MCU, 16 MHz
crystal oscillator, LED diode and Segment display. The following parts are
used:

===============  ===============
Component        Part name
===============  ===============
MCU              MK22FN1M0VLK12
16 MHz Crystal   7B-16.000MEEQ-T
LED              MCL056PURGW
Segment display  703-0186
===============  ===============

Prerequisites
-------------

To compile this sources ``binutils``, ``gcc``, ``newlib`` and ``gdb`` for ARM
Cortex-M4 should be available in path and the toolchain prefix environment
variable should be set, e.g.:

    LIBTASKING_TESTSUITE_TOOLCHAIN=arm-eabi-

Additionally J-Link Commander and J-Link GDBServer should be installed and
available in path.

Tested versions:

================  =======
Tool              Version
================  =======
binutils          2.24
gcc               4.9.2
newlib            2.0.0
gdb               7.8.1
J-Link Commander  4.9.8d
J-Link GDBServer  4.9.8d
================  =======


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
