all: blur-no-opt blur-opcode

blur-no-opt: blur.c blur-opcode.c
	gcc -S blur.c -o blur.s
	gcc -o blur-no-opt blur.c
	gcc -O2 -S blur.c -o blur-O2.s
	gcc -O2 -o blur-O2 blur.c
	gcc -O2 -pg -fprofile-arcs -ftest-coverage blur.c -o blur-profile
	gcc -O2 -DBLUR_UNROLL_INNER -o blur-O2-unroll blur.c
	gcc -O2 -DBLUR_UNROLL_INNER -o blur-O2-unroll.s -S blur.c
	gcc -O2 -DBLUR_USE_FUNCTION_CALL -o blur-O2-func blur.c
	gcc -O2 -DBLUR_USE_FUNCTION_CALL -o blur-O2-func.s -S blur.c
	gcc -O2 -DBLUR_USE_SWITCH -o blur-O2-switch blur.c
	gcc -O2 -DBLUR_USE_SWITCH -o blur-O2-switch.s -S blur.c
	gcc -O2 -DBLUR_USE_SWITCH_CALL -o blur-O2-switch-call blur.c
	gcc -O2 -DBLUR_USE_SWITCH_CALL -o blur-O2-switch-call.s -S blur.c
	gcc -O2 -DBLUR_FNPTR_SWITCH -o blur-O2-fnptr-switch blur.c
	gcc -O2 -DBLUR_FNPTR_SWITCH -o blur-O2-fnptr-switch.s -S blur.c
	gcc -O2 -DBLUR_FNPTR_TABLE -o blur-O2-fnptr-table blur.c
	gcc -O2 -DBLUR_FNPTR_TABLE -o blur-O2-fnptr-table.s -S blur.c
	gcc -O2 -DBLUR_USE_SWITCH -o blur-O2-opcode-switch blur-opcode.c
	gcc -O2 -DBLUR_USE_SWITCH -o blur-O2-opcode-switch.s -S blur-opcode.c
	gcc -O2 -DBLUR_USE_FNPTR -o blur-O2-opcode-fnptr blur-opcode.c
	gcc -O2 -DBLUR_USE_FNPTR -o blur-O2-opcode-fnptr.s -S blur-opcode.c
	gcc -O2 -DBLUR_TRANSLATED1 -o blur-O2-opcode-translated1 blur-opcode.c
	gcc -O2 -DBLUR_TRANSLATED1 -o blur-O2-opcode-translated1.s -S blur-opcode.c
	gcc -O2 -DBLUR_TRANSLATED2 -o blur-O2-opcode-translated2 blur-opcode.c
	gcc -O2 -DBLUR_TRANSLATED2 -o blur-O2-opcode-translated2.s -S blur-opcode.c
	gcc -O2 -DBLUR_TRANSLATED3 -o blur-O2-opcode-translated3 blur-opcode.c
	gcc -O2 -DBLUR_TRANSLATED3 -o blur-O2-opcode-translated3.s -S blur-opcode.c

run-all::
	./blur-no-opt
	./blur-O2
	./blur-O2-unroll
	./blur-O2-func
	./blur-O2-switch
	./blur-O2-switch-call
	./blur-O2-fnptr-switch
	./blur-O2-fnptr-table
	./blur-O2-opcode-switch
	./blur-O2-opcode-translated1
	./blur-O2-opcode-translated2
	./blur-O2-opcode-translated3

blur-opcode: blur-opcode.c
	gcc -g blur-opcode.c -o blur-opcode

clean::
	rm -rf *.o *.s blur-no-opt blur-O* blur-profile gmon.out blur.out *.bb *.bbg *.da *.gcov

# blur-no-opt: 11.98
# blur-O1: 6.56
# blur-O2: 6.31
# blur-O3: 8.29
# blur-O4: 8.28
# blur-O5: 8.23
# blur-O6: 8.07

# now break up the task into subfunctions and call them separately
# so that I can measure the functin call overhead.
