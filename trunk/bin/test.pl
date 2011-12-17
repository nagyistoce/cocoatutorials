#!/usr/bin/perl
use Cwd;

my $dir   ;
my $stuff ;

$dir = getcwd;

print "-------------------------------------\n" ;
print "your working directory = " . $dir . "\n" ;

print   @ENV ;
foreach $env ( keys %ENV ) {
  print $env . ' -> ' . $ENV{$env} . "\n" ;
}


chdir("~") ; # /Users/rmills   ;
$dir = getcwd ;

print "PWD = " . $ENV{'PWD'} . "\n" ;
$dir = $ENV{'PWD'} ;

print "your working directory = " . $dir . "\n" ;

# system("ls -alt " . $dir ) ;
print "-------------------------------------\n" ;
