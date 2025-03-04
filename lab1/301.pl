#!/usr/bin/perl

# use strict;
# use warnings;

# my $res = "";

# while (my $line = <STDIN>) {
#     $line =~ s/^[ ]+//;
#     $line =~ s/[ ]+$//;
#     $line =~ s/[ ]+/ /g;
#     $res = $res . $line
# }
# $res =~ s/^(([ ]*\n)+)//;
# $res =~ s/(([ ]*\n)+)$//;
# $res =~ s/\n([ ]*\n)+/\n\n/g;
# print $res

use strict;
use warnings;

my $res = "";

while (my $line = <STDIN>) {
    $res = $res . $line
}

$res =~ s/[ ]+/ /g;
$res =~ s/^ //;
$res =~ s/\n /\n/g;
$res =~ s/ \n/\n/g;

$res =~ s/^(([ ]*\n)+)//;
$res =~ s/(([ ]*\n)+)$//;
$res =~ s/\n([ ]*\n)+/\n\n/g;
print $res