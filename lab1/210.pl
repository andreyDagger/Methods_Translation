#!/usr/bin/perl

use strict;
use warnings;

while (my $line = <STDIN>) {
    $line =~ s/a(.*?)aa(.*?)aa(.*?)a/bad/g;
    print $line;
}
