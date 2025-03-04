#!/usr/bin/perl

use strict;
use warnings;

while (my $line = <STDIN>) {
    if ($line =~ /\b(\w+?)\1\b/) {
        print $line
    }
}

# +: 1 и более симчолов
# ?: делаем это не жадно, т. к. жадно мы съедим всю строку в (\w+?), хотя должны только половину