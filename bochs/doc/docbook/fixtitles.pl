#!/bin/sh
exec perl -x $0 $*; echo "Could not exec perl!"; exit 1
# The line above allows perl to be anywhere, as long as it's in your
# PATH environment variable.

#!perl
#
# fix-titles.pl
# $Id: fixtitles.pl,v 1.1.4.1 2002-03-17 08:51:20 bdenney Exp $
#
# The HTML stylesheet likes to print html has the ends of tags on a different
# line, like this:
# <HTML
# ><HEAD
# ><TITLE
# >FreeBSD</TITLE
# >
#
# Glimpse, which is indexing our website, finds this very confusing and
# it cannot pick out the title from this mess.  This script takes a list
# of HTML files on the command line and attempts to make the <TITLE> tag
# look more normal so that glimpse can understand it.
#
# WARNING: This is a hack.  It's made to work on docbook generated html, but
# may do strange things on anything else.

use strict;

foreach my $file (@ARGV) {
  print "Fixing $file\n";
  rename $file, "$file.orig";
  open (IN, "$file.orig") || die "open $file.orig";
  open (OUT, ">$file") || die "open $file for writing";
  while (<IN>) {
    if (/^<HTML$/) {
      print OUT "<HTML>\n";
    } elsif (/^><HEAD$/) {
      print OUT "<HEAD>\n";
    } elsif (/^><TITLE$/) {
      print OUT "<TITLE>";
    } elsif (/^>(.*)<\/TITLE$/) {
      print OUT "$1</TITLE>\n";
      # next line has one extra >, so read it and remove it.
      $_ = <IN>;
      s/^>//;
      print OUT;
    } else {
      print OUT;
    }
  }
  close IN;
  close OUT;
  unlink "$file.orig";
}
