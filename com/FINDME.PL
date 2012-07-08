#!/bin/perl -w

# ----------------------------------------------------
# This script is called from findme.bat on your path
# findme.bat                  setworld.bat
# @echo off                   @echo off
# perl -S findme.pl %*        set PRIVATE_WORLD=%1
# call findme_helper.bat      set PUBLIC_WORLD=%1
#                             set | grep -i world
# ----------------------------------------------------
#
# find the batch filename
#
use File::Basename                                 ;
my @temp  = fileparse($0)                          ; 
my $temp  = MakePath($temp[1],"findme_helper.bat") ;
open(FILE,">$temp") ;
close FILE ;

sub Syntax
{
   my $PRIVATE_WORLD ;
   my $EDIT          ;
   my $CD            ;
   my $STAGE         ;

   $PRIVATE_WORLD    = $ENV{"PRIVATE_WORLD"} or $PRIVATE_WORLD = "" ;
   $EDIT             = $ENV{"EDIT"}          or $EDIT          = "" ;
   $CD               = $ENV{"CD"}            or $CD            = "" ;
   $STAGE            = $ENV{"STAGE"}         or $STAGE         = "" ;
   
   die "Useage $0 <target>\ntarget=file or package or product\n" 
     . "Environment PRIVATE_WORLD = $PRIVATE_WORLD\n" 
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
sub GetDirectories ;
sub FindFile       ;
sub FindDir        ;
sub FindDirOrFile  ;
sub GetEnv         ;

#
# find the arguments
#
my $target   = shift or Syntax ;
my $argument = shift or 1      ;

my $PrivateWorld = GetEnv("PRIVATE_WORLD","PRIVATE_WORLD not specified - use setworld") ;
my $Edit         = GetEnv("EDIT"         ,"EDIT not specified in environment") ;
my $CD           = GetEnv("CD"           ,"CD not specified in environment") ;
my $STAGE        = GetEnv("STAGE"        ,"STAGE  not specified in environment") ;

my $bWritten     = 0 ;


#
# Open the batch file and figure out what the target is
#
open(FILE,">$temp") ;
FindDirOrFile($PrivateWorld,"packages"        ) ;
FindDirOrFile($PrivateWorld,"products","adobe") ;
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

    print "$msg\n" unless $result ;
    Syntax() unless $result       ;
    
    return $result              ;
}


# ------------------------------------
# IsFileName - name contain a dot?
# ------------------------------------
sub IsFileName { return  (shift) =~ /\./ ; }

# ------------------------------------
#  FindDirOrFile - is $target in this directory
# ------------------------------------
sub FindDirOrFile
{
    my $dir = MakePath(@_) ;

    FindDir ($dir) if !$bWritten ;
    FindFile($dir) if !$bWritten ;
}


# ------------------------------------
#  FindDir - is $target a directory of this directory?
# ------------------------------------
sub FindDir
{
    my      $directory = MakePath(@_,$target) ;
    if ( -d $directory ) {
      if ( $argument && -d MakePath($directory,$argument) ) {
        $directory = MakePath($directory,$argument) ;
      }
      print FILE "$CD $directory\n" ;
      $bWritten ++ ;
    }
}

# ------------------------------------
#  FindFile - is $target in this directory or sources
# ------------------------------------
sub FindFile
{
    my $directory = MakePath(@_) ;
    my @products  = GetDirectories($directory) ;
    foreach $product ( @products ) {

      # try findme filename.c (in sources)
      $dir  = MakePath($directory,$product,"sources") ;
      $file = MakePath($dir,$target) ;
      if ( -e $file ) {
        print FILE   "$CD $dir\n"      ;
        print FILE   "$Edit $file\n" ;
        print      "\n$Edit $file\n\n" ;
        $bWritten ++ ;
      }

      # try findme filename.h (not in sources)
      $dir  = MakePath($directory,$product) ;
      $file = MakePath($dir,$target) ;
      if ( -e $file ) {
        print FILE   "$CD $dir\n"      ;
        print FILE   "$Edit $file\n"   ;
        print      "\n$Edit $file\n\n" ;
        $bWritten ++ ;
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

