#!/usr/bin/perl -w
$version = "1.00" ;


sub DirDump    ; # Forward declaration
sub Println    ; # print args + nl 
sub SetupOS    ; # Setup variables list SEP and so on
sub MakePath   ; # Combine a directory and filename into path
sub Welcome    ; # Show the welcome notice
sub PrintFiles ; # Show the welcome notice

$onehour    = 3600            ;
$oneday     =  24 * $onehour  ;
$oneweek    =   7 * $oneday   ;
$onemonth   =  30 * $oneday   ;
$oneyear    = 365 * $oneday   ;
$twoyear    =   2 * $oneyear  ;
$threeyear  =   3 * $oneyear  ;

my @Oneday   ;
my @Oneweek  ;
my @Onemonth ;
my @Oneyear  ;
my @Twoyear  ;
my @Threeyear;
my @Files    ;
my $files    ;

#
#  Show the welcome message
#
Welcome("Tree","tree dumping utility",$version)  ;

#
#  Get our arguments
#
@args = scalar @ARGV ?  @ARGV :(".") ;

foreach $arg ( @args ) {
  print "AgeSorting " . $arg . "\n" ;
  DirDump $arg ;
  @files = @Oneday    ; PrintFiles "Files younger than 1 day"   ;
  @files = @Oneweek   ; PrintFiles "Files younger than 1 week"  ;
  @files = @Onemonth  ; PrintFiles "Files younger than 1 month" ;
  @files = @Oneyear   ; PrintFiles "Files younger than 1 year"  ;
  @files = @Twoyear   ; PrintFiles "Files younger than 2 years" ;
  @files = @Threeyear ; PrintFiles "Files younger than 3 years" ;
  @files = @Files     ; PrintFiles "Files older   than 3 year"  ;
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
# PrintFiles - print a list of files
# -------------------------------------
sub PrintFiles
{
  return if !scalar @files  ;

  my      $msg   = shift  ;
  my      $file           ;
  print   $msg . "\n"     ;
  foreach $file ( @files ) {
    print "  " . $file . "\n"  ;
  }
  print "\n" ;
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

sub GetTime
{
    my $t = shift ;
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime $t ;
    $year = 1900 + $year ;

    @WDays  = ( "Sunday" , "Monday"  , "Tuesday"  , "Wednesday", "Thursday", "Friday", "Saturday" ) ;
    @Months = ( "January", "February", "March"    , "April"    , "May"     , "June"
              , "July"   , "August"  , "September", "October"  , "November", "December"
              ) ;

    $mon   = $Months[$mon] ;
    $wday  = $WDays[$wday] ;

    $t = sprintf("%s %s/%s/%02d %02d:%02d:%02d",$wday,$mday,$mon,$year,$hour,$min,$sec,) ;
    return $t ;
}

# -------------------------------------
# DirDump
# -------------------------------------
sub DirDump {
  my $dir   = $_[0] ;
  my $total = 0     ;
  my $now   = time  ;

  opendir     DIRECTORY, $dir ;
  my @files = readdir DIRECTORY       ;
              closedir DIRECTORY      ;

  #
  # Report every file
  #
  foreach $file (@files) {
    $file = lc $file ;
    next if ( $file eq '.' || $file eq '..' ) ;

    my $path = MakePath($dir,$file) ;
    
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
           $atime,$mtime,$ctime,$blksize,$blocks)
               = stat($path);

    $total += $size ;

    if ( ! -d $path ) {
      my $age = $now - $mtime ;
      $path = sprintf ("%-40s %s",GetTime($mtime) , $path)  ;

      if ( $age < $oneday ) {
        push @Oneday,$path  ;
      } elsif ( $age < $oneweek ) {
        push @Oneweek,$path ;
      } elsif ( $age < $onemonth ) {
        push @Onemonth,$path ;
      } elsif ( $age < $oneyear ) {
        push @Oneyear,$path ;
      } elsif ( $age < $twoyear ) {
        push @Twoyear,$path ;
      } elsif ( $age < $threeyear ) {
        push @Threeyear,$path ;
      } else {
        push @Files,$path ;
      }
    }
  }

  #
  # Recurse every directory
  #
  foreach $file (@files) {
    next if ( $file eq '.' || $file eq '..' ) ;

    my $path = MakePath($dir,$file) ;
    if ( -d $path ) {
      $total += DirDump($path) ;
    }
  }
  return $total ;
}

