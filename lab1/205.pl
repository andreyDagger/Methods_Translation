#!/usr/bin/perl

use strict;
use warnings;

while (my $line = <STDIN>) {
    $line =~ s/(\w)(\w)(\w*)/$2$1$3/g;
    print $line;
}
