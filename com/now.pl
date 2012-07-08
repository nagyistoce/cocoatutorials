#!/usr/bin/perl -w

sub println
{
	print shift ; print "\n" ;
}

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime time ;

@WDays  = ( "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat") ;
@Months = ( "Jan", "Feb", "Mar", "Apr", "May", "Jun"
          , "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
          ) ;

$Mon   = $Months[$mon] ;
$Wday  = $WDays[$wday] ;
$year += 1900 if $year < 1900 ;
$mon++ ;

print "Time = $hour:$min:$sec  Date = $Wday $mday/$Mon/$year \n" ;
println(sprintf("%d-%02d-%02d %02d:%02d:%02d",$year,$mon,$mday,$hour,$min,$sec)) ;
