#!/usr/bin/perl -w

my $pattern = shift or die "Syntax: $0 <filepattern>\n" ;

sub MB
{
  my $result = shift ;
  my $mb = 1024 * 1024 ;
  if ( $result > $mb ) {
    $result   /= $mb ;
    $result    = sprintf "%10.2fmb",$result ;
  }
  return $result ;
}

while ( 1 ) {
  opendir DIR,"."      ;
  @files = readdir DIR ;
  closedir DIR         ;

  my $bWritten ;
  foreach $file ( @files ) {
    if ( $file =~ /$pattern/i ) {
      @StatStuff = stat $file ;
      $size = $StatStuff[7]   ;
      if ( $size > 0 ) {
        $size = MB($size)   ;
        print "$file = $size "  ;
        $bWritten++             ;
      }
    }
  }
  print "\n" if $bWritten ;
  sleep 5 ;
}
