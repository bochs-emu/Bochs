#!/usr/bin/perl
#
# tested with linux 2.2.14
# reads <asm/unistd.h>, outputs syscalls-linux.h

$date = `date`;
print <<EOF;
//////////////////////////////////////////////////////////////////////////////
// Linux system call table
//////////////////////////////////////////////////////////////////////////////
//
// Format for each entry:
//   DEF_LINUX_SYSCALL(syscall_number, "syscall_name")
// This file can be regenerated with the following command:
//
//    ./make-syscalls-linux.pl < /usr/include/asm/unistd.h > syscalls-linux.h
//
EOF

$max = 0;
while (<STDIN>) {
  $line = $_;
  next unless /#define __NR_[a-z]/;
  s/.*NR_//;
  undef $number;
  ($name, $number) = split (/[\s]+/);
  if ((length $number) < 1) {
    die "bad line: $line";
  }
  if ($number > $max) { $max = $number; }
  print "DEF_SYSCALL($number, \"$name\")\n";
}

print "#define N_SYSCALLS $max\n";
