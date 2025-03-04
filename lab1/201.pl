#!/usr/bin/perl
 
use strict;
use warnings;

 
while (my $line = <STDIN>) {
  $line =~ s/human/computer/g;
  print $line;
}