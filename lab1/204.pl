#!/usr/bin/perl

use strict;
use warnings;

while (my $line = <STDIN>) {
    $line =~ s/(\w+)(\W+)(\w+)/$3$2$1/;
    print $line;
}
