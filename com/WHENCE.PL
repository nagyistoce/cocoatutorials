#!/usr/bin/perl -w
# (c) Robin Mills Software Consultancy Camberley England 1999
# ----------------------------------------------------------------- 
# * File   : whence.pl
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
# Modified By Reason
# 
# -----------------------------------------------------------------
$version = "1.00" ;

use Cwd ;
# -------------------------------------
# main program
# -------------------------------------

    if ( ! scalar @ARGV ) {
      print "whence version " . $version  ;
      print "\nUseage: whence [[name] ...]" ;
      exit 0 ;
    }

    my $bNT = SetupOS() ;
    $path    = lc cwd() . ";" . $ENV{"path"} ;
    $pathext = lc ";"  . $ENV{"pathext"}     ;

    @dirs    = split(";",$path)              ;
    @exts    = split(";",$pathext)           ;

    # go over every program in the argument vector
    foreach $program ( @ARGV ) {
      foreach $dir (@dirs) {
        next if !length $dir ;
    
        # try every extension
        foreach $ext (@exts) {
          $filename = $dir . $PATH_SEP . $program . $ext ;
          if ( -e $filename ) {
            print $filename ;
            exit 0 ;
          } 
        }
      }
      print $program . " not on path\n" ;
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
