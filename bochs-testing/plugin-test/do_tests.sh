#!/bin/sh
OUT=do_tests.out

cat<<EOF
This script will run configure, make, and make test for you, and collect
all the output in a file called $OUT.  It is placed in one file so that
you can easily mail it to a developer who is working on plugin support
for your platform.  If you want to watch while the tests are building
and running, do "tail -f $OUT" to see it.

EOF

echo "*** Cleaning up from any previous tests"
make dist-clean >$OUT 2>&1
# erase that
rm -f $OUT
echo "*** Starting configure, results to $OUT" 
echo "do_tests output file" >> $OUT
echo date: `date` >> $OUT
echo host: `hostname` >> $OUT
echo "" >> $OUT
echo "*** Starting configure" >> $OUT
./configure >> $OUT 2>&1
echo "*** Copying config.log and config.status into the output file"
echo "*** This is config.log" >> $OUT
cat config.log >> $OUT
echo "*** This is config.status" >> $OUT
cat config.status >> $OUT
echo "*** Starting make, results to $OUT" 
echo "*** Starting make" >> $OUT
make >> $OUT 2>&1
echo "*** Starting make test, results to $OUT" 
echo "*** Starting make test" >> $OUT
make test >> $OUT 2>&1

cat<<EOF

The tests are complete!  Run "make test" or look at the end of $OUT
to see the test results.  To clean up any files that were generated
during this script (except for $OUT), do "make dist-clean".

$0 is exiting.
EOF
