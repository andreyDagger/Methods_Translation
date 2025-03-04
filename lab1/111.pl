#!/usr/bin/perl
 
use strict;
use warnings;

 
while (my $line = <STDIN>) {
  if ($line =~ /^(1(01*0)*1|0)*$/) {
    print $line
  }
}