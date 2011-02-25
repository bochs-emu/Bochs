#!/usr/bin/perl
#####################################################################
# $Id$
#####################################################################
#
# Batch build tool for multiple configurations
#
# switches:
# - show output vs. send it all to nohup.  --nohup
# - serial or parallel.  --parallel
#
# no args: serial, display output
# --nohup: serial, output to nohup.out.  (Need summary.)
# --nohup --parallel: parallel, output to nohup.out
# --parallel: parallel, spawn xterm for each
#

sub usage {
  print <<EOF;
Usage: $0 [--nohup] [--parallel]
--nohup causes the output of all compiles to go into nohup.out
--parallel causes all compiles to be run in parallel

Usage: $0 --clean
--clean erases the build directories and recreates them from scratch

Combinations of nohup and parallel:
no args: serial compile, output goes to stdout
--nohup:  serial compile, output goes into individual nohup.out files
--nohup --parallel: parallel compile, output goes to individual nohup.out files
--parallel: parallel compile, spawn an xterm for each configuration
EOF
}

$DEBUG=0;

$TEST_STANDARD = 1;
$TEST_GUIS = 1;
$TEST_CPU = 1;
$TEST_SMP = 1;
$TEST_IODEV = 1;
$TEST_PCI = 1;
$TEST_X86_64 = 1;
$TEST_SSE = 1;
$TEST_PLUGINS = 1;
$TEST_DEVICES = 1;

$pwd = `pwd`;
chop $pwd;

# create all the configurations that we should test.  The first argument
# is the configuration name, which must be a valid directory name.  To be
# safe, don't put spaces, slashes, ".", or ".." in here.
if ($TEST_STANDARD) {
  add_configuration ('normal', 
    '');
  add_configuration ('dbg',
    '--enable-debugger');
}

if ($TEST_PLUGINS) {
add_configuration ('plug',
  '--enable-plugins');
add_configuration ('plug-d',
  '--enable-plugins --enable-debugger');
add_configuration ('plug-allgui',
  '--enable-plugins --with-all-libs');
add_configuration ('plug-allgui-d',
  '--enable-plugins --with-all-libs --enable-debugger');
add_configuration ('plug-smp',
  '--enable-plugins --enable-smp');
add_configuration ('plug-smp-d',
  '--enable-plugins --enable-smp --enable-debugger');
add_configuration ('plug-x86-64',
  '--enable-plugins --enable-x86-64');
add_configuration ('plug-wx',
  '--enable-plugins --with-wx');
}

if ($TEST_DEVICES) {
add_configuration ('alldev',
  '--enable-ne2000 --enable-pci --enable-port-e9-hack --enable-cdrom --enable-iodebug');
add_configuration ('ne2000',
  '--enable-ne2000');
add_configuration ('pci',
  '--enable-pci');
add_configuration ('port-e9-hack',
  '--enable-port-e9-hack');
add_configuration ('cdrom',
  '--enable-cdrom');
add_configuration ('gdbstub',
  '--enable-gdbstub');
add_configuration ('iodebug',
  '--enable-iodebug');
}

if ($TEST_GUIS) {
  # test with various gui options
  add_configuration ('wx',
    '--with-wx');
  add_configuration ('wx-d',
    '--with-wx --enable-debugger');
  add_configuration ('sdl',
    '--with-sdl');
  add_configuration ('sdl-d',
    '--with-sdl --enable-debugger');
  add_configuration ('term',
    '--with-term');
  add_configuration ('term-d',
    '--with-term --enable-debugger');
  add_configuration ('rfb',
    '--with-rfb');
  add_configuration ('rfb-d',
    '--with-rfb --enable-debugger');
  add_configuration ('nogui',
    '--with-nogui');
  add_configuration ('nogui-d',
    '--with-nogui --enable-debugger');
}

if ($TEST_CPU) {
# test with various cpu options
add_configuration ('i386',
  '--enable-cpu-level=3 --disable-mmx');
add_configuration ('i486',
  '--enable-cpu-level=4 --disable-mmx');
add_configuration ('i586',
  '--enable-cpu-level=5');
add_configuration ('i686',
  '--enable-cpu-level=6');
add_configuration ('large-pages',
  '--enable-large-pages');
add_configuration ('pae',
  '--enable-pae');
add_configuration ('repeat',
  '--enable-repeat-speedups');
add_configuration ('globalpg',
  '--enable-global-pages');
add_configuration ('icache',
  '--enable-icache');
add_configuration ('cpuall',
  '--enable-large-pages --enable-pae --enable-global-pages --enable-all-optimizations');
}

if ($TEST_SMP) {
# smp
add_configuration ('smp',
  '--enable-smp');
add_configuration ('smp-d',
  '--enable-smp --enable-debugger');
add_configuration ('smp-wx',
  '--enable-smp --with-wx');
add_configuration ('smp-wx-d',
  '--enable-smp --with-wx --enable-debugger');
}

if ($TEST_X86_64) {
# test x86-64
add_configuration ('64bit',
  '--enable-x86-64');
add_configuration ('64bit-d',
  '--enable-x86-64 --enable-debugger');
add_configuration ('64bit-wx',
  '--enable-x86-64 --with-wx');
add_configuration ('64bit-wx-d',
  '--enable-x86-64 --with-wx --enable-debugger');
}

if ($TEST_SSE) {
# test SSE configurations
add_configuration ('sse1',
  '--enable-sse=1');
add_configuration ('sse2',
  '--enable-sse=2');
add_configuration ('sse3',
  '--enable-sse=3');
add_configuration ('sse4',
  '--enable-sse=4');
add_configuration ('sse2-dbg',
  '--enable-sse=2 --enable-debugger');
add_configuration ('sse2-x86-64-wx-d',
  '--enable-sse=2 --enable-x86-64 --with-wx --enable-debugger');
}

my $nohup = 0;
my $parallel = 0;
my $clean = 0;
foreach my $arg (@ARGV) {
  if ($arg eq '--clean') {
    $clean = 1;
  } elsif ($arg eq '--nohup') {
    $nohup = 1;
  } elsif ($arg eq '--parallel') {
    $parallel = 1;
  } else {
    usage(); exit 1;
  }
}

# this script may be run from various directories, and this affects 
# the path to the configure command.  Try to figure out where the configure
# command is found, and set up the build.sh scripts accordingly.  If it
# can't be found, spit out an error now instead of later.
my @configurepath_tries = ("configure", "../configure", "../../configure");
my $configurepath;
foreach my $trypath (@configurepath_tries) {
  if (-x $trypath) {
    print "Found configure at $configurepath.\n" if $DEBUG;
    $configurepath = $trypath;
  }
}
if (!defined $configurepath) {
  print <<EOF;
ERROR I could not locate the configure script.  This script is intended
to be run from the bochs main directory or a subdirectory of it.  Examples:
 1) cd $BOCHS; ./build/batch-build.perl
 2) cd $BOCHS/build; ./batch-build.perl

Here are the places that I tried to find the configure script:
EOF
  foreach (@configurepath_tries) {
    print "  $_\n";
  }
  exit 1;
}

$x = 50; $y = 50;
$xinc = 30;
$yinc = 30;

for (my $i=0; $i <= $#config_names; $i++) {
  my $name = "build-$config_names[$i]";
  my $options = $config_opts[$i];
  die if (!defined $name || !defined $options);
  print "Compiling '$name' with opts '$options'\n" if $DEBUG;
  if ($clean) {
    my $rmcmd = "rm -rf $name";
    print "Removing directory $name\n";
    system $rmcmd;
    next;
  }
  if (! -d $name) { mkdir $name, 0755; }
  $maybe_nohup = $nohup? "nohup" : "";
  open (BUILD, ">$name/build.sh");
  print BUILD <<BUILD_EOF;
#!/bin/bash
echo Running the configure script
export CFLAGS='-g -O2 -Wall'
export CXXFLAGS='-g -O2 -Wall'
$maybe_nohup ../$configurepath $options
if test $? != 0; then
  echo Configure failed.
  exit 1
fi
echo Running make
$maybe_nohup make
if test $? != 0; then
  echo Make failed.
  exit 1
fi
BUILD_EOF
  close BUILD;
  chmod 0755, "$name/build.sh";
  $gotodir = "cd $name";
  $startcmd = "nice $maybe_nohup ./build.sh";
  $header = <<HEADER_EOF;
====================================================================
Configuration name: $name
Directory: $pwd/$name
Config Options: $options
====================================================================
HEADER_EOF
  print $header;

  if ($parallel && !$nohup) {
    # special case for parallel without nohup.  If you're not careful,
    # all output from all compiles will go into the window at once, which
    # is impossible to read.  Also very hard to kill them until they have
    # run their course.  Instead, start each compile in a different xterm!
    # What's even more useful is that after the compile stops it goes into
    # a bash shell so that you can fix things, run the make again, etc.
    #
    # To do this, put the start command in a little shell script called
    # xterm-init.sh.  Start the xterm with "-e xterm-init.sh" so that it
    # runs the script as it starts.

    open (XTI, ">$name/xterm-init.sh");
    print XTI <<XTI_EOF;
#!/bin/bash
cat <<EOF
$header
EOF
$startcmd
bash
exit 0
XTI_EOF
    close XTI;
    chmod 0755, "$name/xterm-init.sh";
    $geometry = "-geom +$x+$y";
    $x+=$xinc; 
    $y+=$yinc;
    $startcmd = "xterm -title $name -name $name $geometry -e ./xterm-init.sh";
  }

  $cmd = "$gotodir && $startcmd";
  $cmd .= "&" if $parallel;
  print "Executing '$cmd'\n" if $DEBUG;
  system $cmd;
}

print "\n"x2;
print "batch-build script is done.\n";
exit 0;

sub add_configuration {
  my ($name, $opts) = @_;
  push @config_names, $name;
  push @config_opts, $opts;
}

