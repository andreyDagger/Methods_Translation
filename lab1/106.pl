#!/usr/bin/perl

use strict;
use warnings;


while (my $line = <>) {
  my $count = () = $line =~ /\b\d+\b/g;
  print $line if $count >= 1;
}
