#!/usr/bin/perl

while (<STDIN>)
{
  chop;
  s/^ *//;
  if (/^#/ || /^ *$/) { print "$_\n"; next;}
  ($key, $equals, $xksym) = split (/ +/);
  printf ("%-45s %-10s %s\n", $key, 'none', "XK_$xksym");
}
