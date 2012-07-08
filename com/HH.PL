#!/usr/bin/perl -w
# (c) Robin Mills Software Consultancy Camberley England 1999
# ----------------------------------------------------------------- 
# * File   : HelloWorld2.pl
# * Author : Robin Mills
# * Date   : 5.March.1999
# -----------------------------------------------------------------
# 
# * Purpose:
#   Echos command line args and environment
# 
# * Useage :
#   HelloWorld2 args...
# 
# * Example:
#   HelloWorld2 now is the time
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
#$version = "1.00" ;

sub print_header ; # Forward declaration


# ----------------------
# Main program
# ----------------------

print "hello world\n" ;
$argc = scalar @ARGV  ;
for ( $i = 0 ; $i < $argc ; $i++ ) {
  print $ARGV[$i] . "\n" ;
}

print_header "Arguments" ;
foreach $arg ( @ARGV ) {
  print $arg . "\n" ;
}

# ----------------------
# print_header
# ----------------------
sub print_header
{
  my ( $title ) = @_  ;
  my $width     = 30  ;
  my $len       = length($title)     ;
  my $blanks    = $width - $len - 2  ;
  my $blanks1   = int $blanks/2      ;
  my $blanks2   = $blanks - $blanks1 ;
  
  print "-" x $width . "\n" ;
  print "-" . " " x $blanks1 . $title . " " x $blanks2 . "-\n" ;
  print "-" x $width . "\n" ;
}
