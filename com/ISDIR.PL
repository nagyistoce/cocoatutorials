#!/usr/bin/perl -w
# (c) Robin Mills Software Consultancy Camberley England 1999
# ----------------------------------------------------------------- 
# * File   : isdir.pl
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

$argc = 0 ;
foreach $dir ( @ARGV ) { $argc++ ; }

$dir = $argc > 1 or die "usage isdir <dir>...\n" ;

foreach $dir ( @ARGV ) {
  print "$dir " ;
  if ( -d $dir ) {
    print "directory" ;
  }
  elsif ( -f $dir ) {
    print "file" ;
  }
  else {
    print "unknown" ;
  }
  print "\n" ;

}


