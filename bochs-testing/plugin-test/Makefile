SUBDIRS = test1-static test2-dynamic test3-twomodules test4-interdep

all:
	for i in $(SUBDIRS); do make -C $$i all; done

clean:
	for i in $(SUBDIRS); do make -C $$i clean; done
