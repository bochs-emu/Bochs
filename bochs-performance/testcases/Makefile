all: blur


blur: blur.c
	gcc -S blur.c -o blur.s
	gcc -o blur-no-opt blur.c
	gcc -O2 -S blur.c -o blur-O2.s
	gcc -O2 -o blur-O2 blur.c

clean::
	rm -rf *.o blur-no-opt blur-O*

# blur-no-opt: 11.98
# blur-O1: 6.56
# blur-O2: 6.31
# blur-O3: 8.29
# blur-O4: 8.28
# blur-O5: 8.23
# blur-O6: 8.07

# now break up the task into subfunctions and call them separately
# so that I can measure the functin call overhead.
