.PHONY: all clean tags

all :
	@for d in tests/*/; do make -C $$d all; done

clean :
	@for d in tests/*/; do make -C $$d clean; done

tags :
	ctags -R .
	cscope -Rb
