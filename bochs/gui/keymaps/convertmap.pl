#!/usr/bin/perl
# little utility script that I used to convert key map files from
# the pre-March 11 format to the post-March 11 format.  It doesn't
# do anything smart with the ascii equivalents and modifiers, so ATM those must
# be added by hand.

while (<STDIN>)
{
  chop;
  s/^ *//;
  if (/^#/ || /^ *$/) { print "$_\n"; next;}
  ($key, $equals, $xksym) = split (/ +/);
  printf ("%-45s %-10s %s\n", $key, 'none', "XK_$xksym");
}
