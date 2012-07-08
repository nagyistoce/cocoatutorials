#!/usr/bin/perl -w
$version = "1.00" ;


sub DirDump  ; # Forward declaration
sub Println  ; # print args + nl 
sub SetupOS  ; # Setup variables list SEP and so on
sub MakePath ; # Combine a directory and filename into path
sub Welcome  ; # Show the welcome notice

#
#  Show the welcome message
#

Welcome("Tree","tree dumping utility",$version)  ;

#
#  Get our arguments
#
@args = scalar @ARGV ?  @ARGV :(".") ;

printf "Release,Folder Level,FileName,,,,,\n" ;
printf "//,,DETAILED CONTENTS,,,,,\n"          ;
$depth = 1 ;


foreach $arg ( @args ) {
#   print "Dumping " . $arg . "\n" ;
#   Println("Total = ", DirDump($arg),"\n") ;
    
    printf "*,1," . $arg . "(folder)\n"  ;
    DirDump($arg) ;
}

# -------------------------------------
# Welcome - 
# -------------------------------------
sub Welcome
{
  my ( $name,$desc,$version ) = @_ ;

  my $bNT = SetupOS() ;
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

  $depth ++ ;

  opendir     DIRECTORY, $dir ;
  my @files = readdir DIRECTORY       ;
              closedir DIRECTORY      ;

  #
  # Report every file
  #
  foreach $file (@files) {
    # $file = lc $file ;
    next if ( $file eq '.' || $file eq '..' ) ;

    my $path = MakePath($dir,$file) ;
    
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
           $atime,$mtime,$ctime,$blksize,$blocks)
               = stat($path);

    $total += $size ;

    if ( ! -d $path ) {
      # printf "%9d %s\n" , $size , $path  ;
      printf "*,%d,",$depth                ;
      for ( $i = 1 ; $i < $depth ; $i++ ) { printf '"",' ; }
      printf '"%s"' . "\n" , $file  ;
    }
  }

  #
  # Recurse every directory
  #
  foreach $file (@files) {
    next if ( $file eq '.' || $file eq '..' ) ;

    my $path = MakePath($dir,$file) ;
    if ( -d $path ) {
      printf "*,%d,",$depth  ;
      for ( $i = 1 ; $i < $depth ; $i++ ) { printf '"",' ; }
      printf '"%s(folder)"' . "\n" , $file  ;
      $total += DirDump($path) ;
    }
  }
  $depth -- ;
  return $total ;
}

