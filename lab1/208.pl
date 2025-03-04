#!/usr/bin/perl

use strict;
use warnings;

while (my $line = <STDIN>) {
    $line =~ s/(\d)0(?!\d)/$1/g;
    print $line;
}
