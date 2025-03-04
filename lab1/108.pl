#!/usr/bin/perl

use strict;
use warnings;

while (my $line = <>) {
  if ($line =~ /\([^\(\)]*\b\w+\b[^\(\)]*\)/) {
    print $line;
  }
}