#!/usr/bin/perl
#
# $Id: makesym.perl,v 1.3 2009-12-30 20:28:23 sshwarts Exp $
#
# Read output file from as86 (e.g. rombios.txt) and write out a symbol
# table suitable for the Bochs debugger.
#

use strict;
use warnings;

my $WHERE_BEFORE_SYM_TABLE = 0;
my $WHERE_IN_SYM_TABLE = 1;
my $WHERE_AFTER_SYM_TABLE = 2;

my $where = $WHERE_BEFORE_SYM_TABLE;
while (<STDIN>) {
  chop;
  if ($where == $WHERE_BEFORE_SYM_TABLE && /^Symbols:/) {
    $where = $WHERE_IN_SYM_TABLE;
  } elsif ($where == $WHERE_IN_SYM_TABLE && /^$/) {
    $where = $WHERE_AFTER_SYM_TABLE;
  }
  if ($where == $WHERE_IN_SYM_TABLE) {
    my $name;
    my $junk;
    foreach my $f (split(/\s+/)) {
      if ($f =~ /^[[:xdigit:]]{4,}$/) {
        if (defined($name)) {
	  print '000f', lc($f), ' ', $name, "\n";
	  undef($name);
	  undef($junk);
	  next;
	}
      }
      $name = $junk
        if (defined($junk));
      $junk = $f;
    }
  }
}

exit(0);
