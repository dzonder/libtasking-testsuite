TESTDIR = tests

ALL_TESTS := $(dir $(shell cd $(TESTDIR); find . -name 'Makefile'))
AUTO_TESTS := $(dir $(shell cd $(TESTDIR); find . -name 'test.py'))
PERF_TESTS := $(shell cd $(TESTDIR); find . -type d -name 'perf_*')

PERF_REPORT_OPTIMIZATIONS = O0 O3 Os

.PHONY: all clean tags perf perf_report plot_report $(ALL_TESTS)

all : $(ALL_TESTS)

$(ALL_TESTS) :
	$(MAKE) -C $(TESTDIR)/$@ all

clean :
	@rm -rvf build *.log *.out *.png

mrproper : clean
	@rm -rf report_*

tags :
	ctags -R .
	cscope -Rb

test :
	@echo "" > test.log
	@for d in $(AUTO_TESTS); do                                \
		$(MAKE) -C $(TESTDIR)/$$d test >> test.log 2>&1;   \
		if [ "$$?" -eq "0" ]; then                         \
			echo ".\t$$d";                             \
		else                                               \
			echo "F\t$$d";                             \
		fi;                                                \
	done
	@echo "Tests failed: `cat test.log | grep '& FAILED' | wc -l`"

perf :
	@for d in $(PERF_TESTS); do $(MAKE) -C $(TESTDIR)/$$d test; done

perf_report :
	@for opt in $(PERF_REPORT_OPTIMIZATIONS); do \
		make clean;                          \
		OPTIMIZATIONS=-$$opt make;           \
		make perf;                           \
		mkdir report_$$opt;                  \
		mv *.out report_$$opt;               \
	done

plot_report :
	@python scripts/report.py
	@feh *.png
