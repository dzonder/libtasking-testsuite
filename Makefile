TESTS := $(dir $(shell find tests -name 'Makefile'))

.PHONY: all clean tags $(TESTS)

all : $(TESTS)

$(TESTS) :
	$(MAKE) -C $@ all

clean :
	@for d in $(TESTS); do $(MAKE) -C $$d clean; done

tags :
	ctags -R .
	cscope -Rb
