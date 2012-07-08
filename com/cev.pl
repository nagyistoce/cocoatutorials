#!/bin/perl -w

open FILE,"vomit" ;
$filename = <FILE> ;
close FILE ;
chomp($filename) ;


system("ce $filename") ;