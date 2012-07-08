#!/usr/bin/perl -w

sub DirDump    ; # Forward declaration
sub Println    ; # print args + nl 
sub SetupOS    ; # Setup variables list SEP and so on
sub MakePath   ; # Combine a directory and filename into path
sub Welcome    ; # Show the welcome notice
sub PrintFiles ; # Show the welcome notice

$onesecond  =    1              ;
$oneminute  =   60 * $onesecond ;
$onehour    =   60 * $oneminute ;
$oneday     =   24 * $onehour   ;
$twoday     =    2 * $oneday    ;
$threeday   =    3 * $oneday    ;
$oneweek    =    7 * $oneday    ;
$twoweek    =    2 * $oneweek   ;
$onemonth   =   30 * $oneday    ;
$threemonth =    3 * $onemonth  ;
$oneyear    =  365 * $oneday    ;
$twoyear    =    2 * $oneyear   ;
$threeyear  =    3 * $oneyear   ;

my @OneHour    ;
my @Oneday     ;
my @Twoday     ;
my @Threeday   ;
my @Oneweek    ;
my @Twoweek    ;
my @Onemonth   ;
my @Threemonth ;
my @Oneyear    ;
my @Twoyear    ;
my @Threeyear  ;
my @Files      ;
my $files      ;

my @FILES =
( \@OneHour 
, \@Oneday   
, \@Twoday   
, \@Threeday 
, \@Oneweek  
, \@Twoweek  
, \@Onemonth 
, \@Threemonth 
, \@Oneyear  
, \@Twoyear  
, \@Threeyear
, \@Files    
) ;

my @TIMES  =
(  $onehour
,  $oneday   
,  $twoday   
,  $threeday 
,  $oneweek  
,  $twoweek  
,  $onemonth 
,  $threemonth 
,  $oneyear  
,  $twoyear  
,  $threeyear
) ;

my @Titles =
(  "Younger than 1 hour"   
,  "Younger than 1 day"   
,  "Younger than 2 days"  
,  "Younger than 3 days"  
,  "Younger than 1 week"  
,  "Younger than 2 weeks" 
,  "Younger than 1 month" 
,  "Younger than 3 months" 
,  "Files younger than 1 year"  
,  "Files younger than 2 years" 
,  "Files younger than 3 years" 
,  "Files older   than 3 years" 
) ;

#
#  Show the welcome message
#
Welcome("urls","tree to URL convertor")  ;

sub Syntax 
{
   print  "Useage: urls directory url\n" ;
   printf "Eg:     urls Y:\projects\PTG-Core\Copernicus\Documents http://janeway.corp.adobe.com/PTG-Core/Copernicus/Documents\n\n" ;
   die ;
}
#
#  Get our arguments
#
# @args = scalar @ARGV ?  @ARGV :(".") ;

$arg        = shift or Syntax ;
$url        = shift or Syntax ; 


print "<HTML><HEAD><TITLE>$url</TITLE></HEAD><BODY bgcolor=#FFFFFF>\n" ;
print "<H2>$url</p>\n<table>" ;
print "<p>Updated: " . GetTime(time) . "</p>\n" ;

DirDump($arg ,$url) ;
for ( $i = 0 ; $i < scalar(@Titles) ; $i++ ) {
  @files  = @{$FILES[$i]}  ;
  PrintFiles $Titles[$i] ;
}
print "</table>\n" ;
print "</BODY></HTML>\n" ;

# -------------------------------------
# Welcome - 
# -------------------------------------
sub Welcome
{
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
  print   "<TR><TD VALIGN=Top><B>$msg</B></TD><TD VALIGN=Top>"     ;
  foreach $file ( @files ) {
    print "<A HREF=$file>$file</A></BR>\n"  ;
  }
  print "</TD></TR>" ;
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
# GetTime - get the time as a string
# -------------------------------------
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
  my $dir   = shift ;
  my $URL   = shift ;
  my $total = 0     ;
  my $now   = time  ;
  my $path ;

  opendir     DIRECTORY, $dir ;
  my @files = readdir DIRECTORY       ;
              closedir DIRECTORY      ;

  #
  # Report every file
  #
  foreach $file (@files) {
    $file = $file    ;
    next if ( $file eq '.' || $file eq '..' ) ;

    my $path = MakePath($dir,$file) ;
    my $url  = $URL . "/" . $file   ;
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
           $atime,$mtime,$ctime,$blksize,$blocks)
               = stat($path);

    $total += $size ;

    if ( ! -d $path ) {
      my $age = $now - $mtime ;
      # $path = sprintf ("%-40s %s",GetTime($mtime) , $path)  ;

      my $bFound = 0 ;
      for ( $i = 0 ; !$bFound && $i < (scalar @TIMES)  ; $i++ ) {
        if ( $age < $TIMES[$i] ) {
          push @{$FILES[$i]},$url  ;
          $bFound ++ ;
        }
      }
      push @{$FILES[$i+1]},$url if !$bFound ;
    }
  }

  #
  # Recurse every directory
  #
  foreach $file (@files) {
    next if ( $file eq '.' || $file eq '..' ) ;

    my $path = MakePath($dir,$file) ;
    my $url  = $URL . "/" . $file   ;
    if ( -d $path ) {
      $total += DirDump($path,$url) ;
    }
  }
  return $total ;
}

