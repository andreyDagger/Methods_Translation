#!/usr/bin/perl

use strict;
use warnings;


while (my $line = <>) {
  if ($line !~ /^ /g && $line !~ / $/g) {
    print $line;
  }
}
