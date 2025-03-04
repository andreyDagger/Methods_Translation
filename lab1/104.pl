#!/usr/bin/perl

use strict;
use warnings;


while (my $line = <>) {
  my $count = () = $line =~ /z.{3}z/g;
  print $line if $count >= 1;
}
