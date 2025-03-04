#!/usr/bin/perl
 
use strict;
use warnings;

 
while (my $line = <STDIN>) {
  $line =~ s/\bhuman\b/computer/g;
  print $line;
}