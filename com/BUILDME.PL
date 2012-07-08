#!/bin/perl -w

sub Syntax
{
   my $PRIVATE_WORLD ;
   my $CONFIG        ;
   my $STAGE         ;
   my $EDIT          ;
   my $CD            ;

   $PRIVATE_WORLD    = $ENV{"PRIVATE_WORLD"} or $PRIVATE_WORLD = "" ;
   $CONFIG           = $ENV{"CONFIG"}        or $CONFIG        = "" ;
   $STAGE            = $ENV{"STAGE"}         or $STAGE         = "" ;
   $EDIT             = $ENV{"EDIT"}          or $EDIT          = "" ;
   $CD               = $ENV{"CD"}            or $CD            = "" ;
   
   die "Useage $0 <target>\ntarget=library or product or 'packages'\n" 
     . "Environment PRIVATE_WORLD = $PRIVATE_WORLD\n" 
     . "Environment CONFIG        = $CONFIG\n" 
     . "Environment STAGE         = $STAGE\n" 
     . "Environment EDIT          = $EDIT\n" 
     . "Environment CD            = $CD\n"
     ;
}

#
# Prototypes
#
sub Dots           ;
sub IsFileName     ;
sub SetupOS        ;
sub MakePath       ;
sub BuildPackage   ;
sub BuildProduct   ;
sub BuildPackages  ;
sub GetDirectories ;
sub GetEnv         ;

#
# find the batch filename
#
use File::Basename                                  ;
my @temp  = fileparse($0)                           ; 
my $temp  = MakePath($temp[1],"buildme_helper.bat") ;
open (FILE,">$temp") ;
close FILE ;

#
# find the arguments
#
my $target = shift or Syntax ;

my $PrivateWorld = GetEnv("PRIVATE_WORLD","PRIVATE_WORLD not specified - use setworld") ;
my $CD           = GetEnv("CD"           ,"CD not specified in environment"    ) ;
my $CONFIG       = GetEnv("CONFIG"       ,"CONFIG not specified in environment") ;
my $STAGE        = GetEnv("STAGE"        ,"STAGE  not specified in environment") ;
my $bWritten     = 0 ;


#
# Open the batch file and figure out what the target is
#
open(FILE,">$temp") ;

if ( $target =~ /packages/i ) {
  BuildPackages() ;
} else {
  BuildPackage($PrivateWorld,"packages",$target) ;
  BuildProduct($PrivateWorld,"products","adobe",$target) ;
}
close FILE ;

print "Cannot find target $target\n" unless $bWritten ;

# =====================================================

# ------------------------------------
# GetEnv - get something from the environment or die
# ------------------------------------
sub GetEnv
{
    my $var = shift             ; # Environment variable name
    my $msg = shift             ; # death message
    my $result = $ENV{$var}     ;
    die "$msg\n" unless $result ;
    return $result              ;
}

# ------------------------------------
#  BuildPackages - build all packages
# ------------------------------------
sub BuildPackages
{
    my $cmd = "foreachpack $PrivateWorld" 
            . ' "' . $CONFIG                      . '"'
            . ' "' . "cd <CONFIG>\\$STAGE && mk"  . '"'
            ;
    print FILE $cmd . "\n" ;
    print      $cmd . "\n" ;
    $bWritten ++ ;
}

# ------------------------------------
#  BuildPackage - build a package
# ------------------------------------
sub BuildPackage
{
    my $directory = MakePath(@_) ;

    my @configs  = GetDirectories($directory) ;
    foreach $config ( @configs ) {
      if ( $config =~ /$CONFIG/i ) {
        my $stage = MakePath($directory,$config,$STAGE) ;

        if ( -d $stage ) {
          print FILE "$CD $stage && mk\n" ;
          print    "\n$CD $stage && mk\n\n" ;
          $bWritten ++ ;
        }
      }
    }
}

# ------------------------------------
#  BuildProduct - build a product
# ------------------------------------
sub BuildProduct
{
    my $directory = MakePath(@_) ;

    my @configs  = GetDirectories($directory) ;
    my $Config   = "^" . substr($CONFIG,0,4) . ".*" . $STAGE ;

    foreach $config ( @configs ) {
      if ( $config =~ /$Config/i ) {
        my $stage = MakePath($directory,$config) ;

        if ( -d $stage ) {
          print FILE "$CD $stage && mk\n" ;
          print    "\n$CD $stage && mk\n\n" ;
          $bWritten ++ ;
        }
      }
    }
}


# ------------------------------------
# Dots - does argument start with .
# ------------------------------------
sub Dots { return  (shift) =~ /^\./ ; }


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

# ------------------------------------
# GetDirectories - get the directories in a directory
# ------------------------------------
sub GetDirectories
{
    my $path = MakePath(@_)       ;
    opendir DIRECTORY,$path       ;
    my @files = readdir DIRECTORY ;
    closedir DIRECTORY            ;

    my @result = ()               ;
    my $file                      ;
    foreach $file (@files ) {
      my $fullpath = MakePath($path,$file) ;
      if ( -d $fullpath && !Dots($file) ) {
        push @result, $file ;
      }
    }
    return @result ;
}

