#!/bin/perl -w

sub Time
{
    my $seconds = int(shift) ;
    my $hour    = int ($seconds / 3600) ; $seconds -= $hour * 3600 ;
    my $min     = int ($seconds / 60 )  ; $seconds -= $min  * 60   ;
    my $result  ;

    if ( !defined($hour) || !$hour || $hour == 0 ) {
      $result = sprintf("%02d %02d",$min,$seconds)  ;
    } else {
      $result = sprintf("%d %02d %02d",$hour,$min,$seconds) ;
    }

    return $result ;
}

$count = 0 ;
print "         --Mile--   --Mara--   -- Lap--      -Half-       --10k--\n" ;
for ( $seconds = 5*60 ; $seconds <= 21*60 ; $seconds += 10 ) {
   printf("%6.2f ",3600/$seconds) ; 
   print  "  " . Time($seconds)
   . "       " . Time($seconds*26.2)
   . "    "    . Time($seconds*0.25)
   . "       " . Time($seconds*13.1)
   . "       " . Time($seconds*6.2)
   . "\n"
   ;
   if ( !($seconds % 60) && $count ) {
     print "         --Mile--   --Mara--   -- Lap--      -Half-       --10k--\n" ;
     print "         --------------------------------------------------------\n" ;
   }
   $count ++ ;

}