#!/usr/bin/perl -w


sub DirDump  ; # Forward declaration
sub Println  ; # print args + nl 
sub SetupOS  ; # Setup variables list SEP and so on
sub MakePath ; # Combine a directory and filename into path
sub Welcome  ; # Show the welcome notice

#
#  Show the welcome message
#
Welcome("hExtract","header extract utility")  ;

#
#  Get our arguments
#
@args = scalar @ARGV ?  @ARGV :(".") ;

foreach $arg ( @args ) {
  opendir     PACKAGES, $arg ;
  
  my @packages ;
  @packages = readdir PACKAGES ;


  foreach $package (@packages) {
     next if ( $package eq '.' || $package eq '..' ) ;
     DirDump( MakePath($arg,$package)) ;
  }
  closedir PACKAGES ;
}

# -------------------------------------
# Welcome - 
# -------------------------------------
sub Welcome
{
  my ( $name,$desc ) = @_ ;

  my $bNT = SetupOS() ;
  if ( $bNT ) {
    print "ษออออออออออหอออออออออออออออออออออออออออออออออออออออออออออออหออออออออออออออออออป\n" ;
    printf "บ %-8s บ %-45s บ Version: 1.00    บ\n",$name,$desc ;
    print "ฬออออออออออสอออออออออออออออออออออออออออออออออออออออออออออออสออออออออออออออออออน\n" ;
    print "บ Copyright (c) 1998 Robin Mills, Camberley, England.    All rights reserved. บ\n" ;
    print "ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ\n" ;
  } else {
    print  "+---------------------------------------------------------------------------+\n"   ;
    printf "| %-8s :    %-44s : Version 1.00    |%s \n",$name,$desc             ;
    print  "|---------------------------------------------------------------------------|\n"   ;
    print  "| Copyright (c) 1998 Robin Mills, Camberley, England. All rights reserved.  |\n" ;
    print  "+---------------------------------------------------------------------------+\n"   ;
  }
}

# -------------------------------------
# Println - print arguments with space and \n
# -------------------------------------
sub Println
{
  my      $arg     ;
  my      $i = $_  ;
  foreach $arg ( @_ ) {
    $i -- ;
    print $arg . ( $i ? " " : "\n" ) ;
  }
}

# -------------------------------------
# SetupOS - 
# -------------------------------------
sub SetupOS
{
    my $result = $^O =~ m:^mswin32:i ; # is OSNAME == mswin32 ?
    $PATH_SEP  =$result?  "\\" : "/" ; # PATH_SEP = \ on NT and / on UNIX
    return $result ;
}

# -------------------------------------
# MakePath
# -------------------------------------
sub MakePath
{
    return $_[0] . $PATH_SEP . $_[1] ;
}

# -------------------------------------
# DirDump
# -------------------------------------
sub DirDump {
  my $dir   = $_[0] ;
  my $total = 0     ;

  opendir     DIRECTORY, $dir ;
#  my @files = glob("$dir/*.h") ;
  my @files = readdir DIRECTORY ;

  #
  # Report every file
  #
  foreach $file (@files) {
    if ( $file =~ /\.h$/i ) {
      print MakePath($dir,$file) . "\n" ;
    }
  }
  closedir DIRECTORY ;
}

