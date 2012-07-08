#!/usr/bin/perl -w
# (c) Robin Mills Software Consultancy Camberley England 1999
# ----------------------------------------------------------------- 
# * File   : whereis.pl
# * Author : Robin Mills
# * Date   : 5.March.1999
# -----------------------------------------------------------------
# 
# * Purpose:
#   Seach for executable on path
# 
# * Useage :
#   None.
# 
# * Example:
#   whence whence
# 
# * Description:
#   None.
# 
# * Restrictions:
#   None.
# 
# -----------------------------------------------------------------
# Modified By     Reason
# 09.17.00 rmills Added support for -environment string
# -----------------------------------------------------------------
$version = "1.02" ;

use Cwd ;
# -------------------------------------
# main program
# -------------------------------------

    my $ARGC = scalar @ARGV ;

    my $path    ;
    my $pathext ;
    my $bNT = SetupOS() ;

    $pathext = lc ";"  . $ENV{"pathext"} ;
    $path    = "path"                    ;

    if ( $ARGC && $ARGV[$ARGC-1] =~ /^-.*/ ) {
      $ARGC -- ;
      $path = $ARGV[$ARGC] ;
      substr($path,0,1) = "" ; # delete the first char
      $pathext = ""          ;
    } 
    $path    = lc cwd() . ";" . $ENV{$path} ;
    
    if ( !$ARGC ) {
      print "whereis version " . $version  ;
      print "\nUseage: whereis [[name] ...] [-environment string]" ;
      print "\nE.g.  : whereis cl" ;
      print "\n      : whereis abc.h CPRLibrary.h -include" ;
      exit 0 ;
    }


    @dirs    = split(";",$path)              ;
    @exts    = split(";",$pathext)           ;

    # go over every program in the argument vector
    my $p         ;

    for ( $p = 0 ; $p < $ARGC ; $p++ ) {
      my $bFound   ;
      my $program  = $ARGV[$p] ;
      foreach $dir (@dirs) {
        next if !length $dir ;
    
        # try every extension
        if ( scalar @exts ) {
          foreach $ext (@exts) {
            my $filename = $dir . $PATH_SEP . $program . $ext ;
            if ( !$bFound && -e $filename && !-d $filename ) {
              print $filename . "\n" ;
              $bFound ++ ;
            } 
          }
        } else {
          my $filename = $dir . $PATH_SEP . $program  ;
          if ( !$bFound && -e $filename && !-d $filename ) {
            print $filename . "\n" ;
            $bFound ++ ;
          }
        }
      }

      print $program . " not found\n"  if !$bFound ;
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
