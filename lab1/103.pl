#!/usr/bin/perl

use strict;
use warnings;


while (my $line = <>) {
  my $count = () = $line =~ /cat/i;
  print $line if $count >= 1;
}
