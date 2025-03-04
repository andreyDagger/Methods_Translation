#!/usr/bin/perl

use strict;
use warnings;

while (my $line = <STDIN>) {
    $line =~ s/(\()(.*?)(\))/()/g;
    print $line;
}
