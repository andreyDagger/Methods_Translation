#!/usr/bin/perl
 
use strict;
use warnings;
use List::MoreUtils 'uniq';
 
# scheme://host:port/path?query#fragment
my $pattern = qr/<a(.*)href="(.+?:\/\/)?(?<need>\w+.*?)[\"\/\:].*>/;
my @links = ();
 
while (my $line = <>) {
  while ($line =~ /$pattern/g) {
    push @links, $+{need};
  }
}
 
my @sorted_links = sort @links;
my @unique = uniq @sorted_links;
print join("\n", @unique), "\n";