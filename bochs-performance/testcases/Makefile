CC=gcc
CFLAGS=-O2
LIST=blur-no-opt blur-O2 blur-profile blur-O2-unroll blur-O2-func blur-O2-switch blur-O2-switch-call blur-O2-fnptr-switch  blur-O2-fnptr-table blur-O2-opcode-switch blur-O2-opcode-fnptr blur-O2-opcode-translated1 blur-O2-opcode-translated2 blur-O2-opcode-translated3 blur-O2-opcode-dynamic1 blur-O2-opcode-dynamic1-test blur-O2-opcode-dynamic2 blur-O2-opcode-dynamic2-test blur-O2-opcode-dynamic3 blur-translate

all: $(LIST) 

blur-no-opt: blur.c blur-opcode.c
	$(CC) -o blur-no-opt blur.c

blur-O2: blur.c
	$(CC) $(CFLAGS) -S blur.c -o blur-O2.s
	$(CC) $(CFLAGS) -o blur-O2 blur.c

blur-profile: blur.c 
	$(CC) $(CFLAGS) -pg -fprofile-arcs -ftest-coverage blur.c -o blur-profile

blur-O2-unroll: blur.c
	$(CC) $(CFLAGS) -DBLUR_UNROLL_INNER -o blur-O2-unroll blur.c
	$(CC) $(CFLAGS) -DBLUR_UNROLL_INNER -o blur-O2-unroll.s -S blur.c

 blur-O2-func: blur.c
	$(CC) $(CFLAGS) -DBLUR_USE_FUNCTION_CALL -o blur-O2-func blur.c
	$(CC) $(CFLAGS) -DBLUR_USE_FUNCTION_CALL -o blur-O2-func.s -S blur.c

blur-O2-switch: blur.c
	$(CC) $(CFLAGS) -DBLUR_USE_SWITCH -o blur-O2-switch blur.c
	$(CC) $(CFLAGS) -DBLUR_USE_SWITCH -o blur-O2-switch.s -S blur.c

blur-O2-switch-call: blur.c
	$(CC) $(CFLAGS) -DBLUR_USE_SWITCH_CALL -o blur-O2-switch-call blur.c
	$(CC) $(CFLAGS) -DBLUR_USE_SWITCH_CALL -o blur-O2-switch-call.s -S blur.c

blur-O2-fnptr-switch: blur.c
	$(CC) $(CFLAGS) -DBLUR_FNPTR_SWITCH -o blur-O2-fnptr-switch blur.c
	$(CC) $(CFLAGS) -DBLUR_FNPTR_SWITCH -o blur-O2-fnptr-switch.s -S blur.c

blur-O2-fnptr-table: blur.c
	$(CC) $(CFLAGS) -DBLUR_FNPTR_TABLE -o blur-O2-fnptr-table blur.c
	$(CC) $(CFLAGS) -DBLUR_FNPTR_TABLE -o blur-O2-fnptr-table.s -S blur.c

blur-O2-opcode-switch: blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_USE_SWITCH -o blur-O2-opcode-switch blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_USE_SWITCH -o blur-O2-opcode-switch.s -S blur-opcode.c

blur-O2-opcode-fnptr: blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_USE_FNPTR -o blur-O2-opcode-fnptr blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_USE_FNPTR -o blur-O2-opcode-fnptr.s -S blur-opcode.c

blur-O2-opcode-translated1: blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_TRANSLATED1 -o blur-O2-opcode-translated1 blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_TRANSLATED1 -o blur-O2-opcode-translated1.s -S blur-opcode.c

blur-O2-opcode-translated2: blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_TRANSLATED2 -o blur-O2-opcode-translated2 blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_TRANSLATED2 -o blur-O2-opcode-translated2.s -S blur-opcode.c

blur-O2-opcode-translated3: blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_TRANSLATED3 -o blur-O2-opcode-translated3 blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_TRANSLATED3 -o blur-O2-opcode-translated3.s -S blur-opcode.c

blur-O2-opcode-dynamic1: blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE1 -o blur-O2-opcode-dynamic1 blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE1 -o blur-O2-opcode-dynamic1.s -S blur-opcode.c

blur-O2-opcode-dynamic1-test: blur-opcode.c translate1.c translate1-defs.h
	$(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE1_TEST -o blur-O2-opcode-dynamic1-test blur-opcode.c translate1.c
	$(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE1_TEST -o blur-O2-opcode-dynamic1-test.s -S blur-opcode.c

translate1.c: blur-O2-opcode-dynamic1
	rm -rf translate1.c
	./blur-O2-opcode-dynamic1
	test -f translate1.c

blur-O2-opcode-dynamic2: blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE2 -o blur-O2-opcode-dynamic2 blur-opcode.c
	$(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE2 -o blur-O2-opcode-dynamic2.s -S blur-opcode.c

blur-O2-opcode-dynamic2-test: blur-opcode.c translate2.c translate2-defs.h
	$(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE2_TEST -o blur-O2-opcode-dynamic2-test blur-opcode.c translate2.c
	$(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE2_TEST -o blur-O2-opcode-dynamic2-test.s -S blur-opcode.c

translate2.c: blur-O2-opcode-dynamic2
	rm -rf translate2.c
	./blur-O2-opcode-dynamic2
	test -f translate2.c

blur-O2-opcode-dynamic3: blur-opcode.c
	libtool $(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE3 -o blur-O2-opcode-dynamic3 blur-opcode.c -lltdl -export-dynamic
	$(CC) $(CFLAGS) -DBLUR_DYNAMIC_TRANSLATE3 -o blur-O2-opcode-dynamic3.s -S blur-opcode.c

blur-translate: blur-translate.c
	libtool $(CC) $(CFLAGS) -DUSE_DYNAMIC_TRANSLATION -o blur-translate blur-translate.c -lltdl -export-dynamic
	$(CC) $(CFLAGS) -S blur-translate.c


dltest:
	libtool gcc -c translate2.c 
	libtool gcc translate2.lo -o libtranslate2.la -rpath /tmp/bochslib
	libtool gcc -c 1.c
	libtool gcc 1.o -o test1 -export-dynamic -lltdl

run-all::
	for i in $(LIST); do rm -f blur.out; echo Running $$i; ./$$i; echo -n "checksum of output: "; md5sum blur.out; done

clean::
	rm -rf *.o *.s blur-no-opt blur-O* blur-profile gmon.out blur.out *.bb *.bbg *.da *.gcov translate1.c translate2.c translate3.c *.lo *.la .libs $(LIST)

# blur-no-opt: 11.98
# blur-O1: 6.56
# blur-O2: 6.31
# blur-O3: 8.29
# blur-O4: 8.28
# blur-O5: 8.23
# blur-O6: 8.07

# now break up the task into subfunctions and call them separately
# so that I can measure the functin call overhead.
