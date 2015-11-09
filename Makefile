SRCDIRS =  Cascade

.PHONY: all clean 

all:
	@for i in $(SRCDIRS); do (echo Entering directory $$i; $(MAKE) -C $$i all) || exit $$?; done

clean:
	@for i in $(SRCDIRS); do $(MAKE) -C $$i clean || exit $$?; done
	@rm -rf lib


