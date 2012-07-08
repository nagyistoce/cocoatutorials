#!/bin/perl -w

use POSIX qw(tmpnam) ;

my $filename = tmpnam() ;
system("guidnew.exe > $filename") ;

open FILE,$filename ;
my @lines ;
my $line  ;
while ( $line = <FILE>) {
  chomp($line) ;
  push @lines,$line ;
}
close FILE ;
unlink $filename ;

my $count = 0 ;
foreach $line ( @lines ) {
  print "$count $line\n" ;
  $count++ ;
}
