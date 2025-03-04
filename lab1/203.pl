#!/usr/bin/perl
 
use strict;
use warnings;

 
while (my $line = <STDIN>) {
  $line =~ s/\b(a+)\b/argh/i;
  print $line;
}