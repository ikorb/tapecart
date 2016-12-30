#!/usr/bin/env perl

# Trivial little script to convert a binary into C-array-compatible
# hex values for use as an include file. No copyright claimed.

use warnings;
use strict;
use feature ':5.10';

if (scalar(@ARGV) != 2) {
    say "Usage: $0 input.bin output.h";
    exit 1;
}

my $buffer;

open IN, "<", $ARGV[0] or die "Can't open $ARGV[0]: $!";
my $len = read(IN, $buffer, -s $ARGV[0]);
if (!defined($len) || $len != -s $ARGV[0]) {
    say "Error reading from $ARGV[0]: $!";
    exit 2;
}
close IN;

open OUT, ">", $ARGV[1] or die "Can't open $ARGV[1]: $!";
say OUT "/* auto-generated from $ARGV[0] on ",scalar(localtime(time)), " */";
my $i;
for ($i = 0; $i < $len; $i++) {
    print  OUT " " if ($i % 8) == 0;
    printf OUT " 0x%02x,", ord(substr($buffer, $i, 1));
    print  OUT "\n" if ($i % 8) == 7;
}
print OUT "\n" unless ($i % 8) == 0;
close OUT;
