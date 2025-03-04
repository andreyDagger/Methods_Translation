#!/usr/bin/perl

use strict;
use warnings;


while (my $line = <>) {
  my $count = () = $line =~ /[xyz].{5,17}[xyz]/g;
  print $line if $count >= 1;
}
