sub Syntax
{
    die "Syntax: pace time distance\n" ;
}

sub NoDistance
{
    my $distance = shift ;
    die   "Unknown distance " . $distance . "\n"
        . "Choices: 10k half marathon\n"
        ;
}

my $time     = shift or Syntax() ;
my $distance = shift or Syntax() ;


my $d = 0 ;
   $d = 6.2  if ( $distance eq "10k"      ) ;
   $d = 13.1 if ( $distance eq "half"     ) ;
   $d = 26.2 if ( $distance eq "marathon" ) ;
   NoDistance($distance) if ( !$d )         ;

$t   = int $time  ;
$sec = $time - $t ;


$T  = $t + ($sec*100)/60 ;
printf("t,d,T = %f %f %f\n",$t, $T,$d) ;

$pace = $T / $d   ;
$p    = int $pace ;
$sec  = int (($pace - int $pace   ) * 60.0) ;
$s    = sprintf("%d",$sec) ;
$s    = ($sec < 10 ? "0" : "") + $s   ;

printf("pace = %d.%02d",$p,$sec) ;

