#!/usr/bin/perl

use strict;
use warnings;


while (my $line = <>) {
  my $count = () = $line =~ /\\/g;
  print $line if $count >= 1;
}
