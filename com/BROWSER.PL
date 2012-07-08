#!/bin/perl -w

# ----------------------------------------------------
# This script is called from browser.bat on your path
# ----------------------------------------------------
#
# find the batch filename
#
use File::Basename                                     ;
my     @temp = fileparse($0)                           ; 
my     $temp = MakePath($temp[1],"browser_helper.bat") ;
unlink $temp ;

#
# find the arguments
#
my $site ;
   $site = shift or $site = "ntrmills" ;

#
# Open the batch file and figure out what the target is
#
open(FILE,">$temp") ;
print FILE "start http://" . $site . "\n" ;
close FILE ;

# ------------------------------------
# SetupOS - 
# ------------------------------------
sub SetupOS
{
    my $result = $^O =~ m:^mswin32:i ; # is OSNAME == mswin32 ?
    $PATH_SEP  =$result?  "\\" : "/" ; # PATH_SEP = \ on NT and / on UNIX
    return $result ;
}

# ------------------------------------
# MakePath - combine elements into a pathname
# ------------------------------------
sub MakePath
{
    SetupOS if !$PATH_SEP         ;
    my $next           ;
    my $result = shift ;
    while ( $next = shift ) {
      $result = $result . $PATH_SEP . $next ;
    }
    return $result ;
}

