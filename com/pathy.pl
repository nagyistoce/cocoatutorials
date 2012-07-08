#!/usr/bin/perl -w
$version = "1.03" ;
# Added "F" output (to detect .jar's in classpath)
print "Pathy " . $version . " F=File,D=Duplicate,X=Doesn't exist" . "\n\n" ;

@args = @ARGV ? @ARGV : ("PATH") ;

foreach $arg ( @args ) {
  if ( @args > 1 ) {
    print "--- $arg ---\n" ;
  }

  $path = $ENV{uc $arg} ;

  my $SEP = ';' ; # if $^O =~ m/MSWin32/ 
  $SEP    = ':'     if $^O =~ m/cygwin/i ;  # respect Cygwin
  $SEP    = ':'     if $^O =~ m/msys/i   ;  # respect MinGW 

  if ( $path ) {
    @dirs = split($SEP,$path) ;

    foreach $dir (@dirs) {
      next if !length $dir ;

      $dir = lc $dir ;
      $dup = -1      ;
      foreach $d ( @dirs) {
        next if !length $d ;
        $d = lc $d ;
        if ( $d eq $dir ) {
          $dup++ ;
        }
      }
      $E    = -d $dir ? " "
            : -e $dir ? "F"
	    :           "X"
	    ;
      $e    = " " 
            . ($dup       ? "D" : " " )
            . $E # ((-d $dir ) ? " " : "X" )
            . " "  ;
      print $e . $dir . "\n"   ;
    }
  } else {
    print "Environment string $arg in not defined\n" ;
  }
}
# Modified  By     Reason
# 25.Sep.11 rmills Modified to work in MinGW environment
# 17.Dec.09 rmills Modified to work in cygwin environment
# 24.Jul.99 rmills Added message for undefined environment strings

# That's all Folks!
##
