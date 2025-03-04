#!/usr/bin/perl

use strict;
use warnings;

while (my $line = <STDIN>) {
    $line =~ s/([a-zA-Z])\1/$1/g;
    print $line;
}
