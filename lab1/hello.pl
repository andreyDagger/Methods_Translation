#!/usr/bin/perl

use strict;
use warnings;


while (my $line = <>) {
  my $count = () = $line =~ /cat/g;
  print $line if $count >= 2;
}
