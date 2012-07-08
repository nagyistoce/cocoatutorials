@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S "%0" %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
goto endofperl
@rem ';
#!/bin/perl -w
#line 14


report(1) ; print "\n\n" ;
report(2) ; print "\n\n" ;
report(3) ; print "\n\n" ;


sub report
{

    $method = shift ;
    
    $lockin   = 36.5                   ; # lockin price
    $shares   = 200                    ; # initial number of shares
    $shares1  = (11700 * 1.1 ) * 0.25  ; # how much invested per month
    $options  = 5 * 2500 / 48.0        ; # initial number of options
    $options1 =     2500 / 48.0        ; # options per month
    $spent    = $shares * $lockin      ; 
    $exchange = 1.55                   ;
    
    printf(" Month    Shares Options Spent: ") ;
    for ($price = 50 ; $price < 200 ; $price += 10 ) {
      printf("%6d ",$price) ;
    }
    printf("\n") ;
    
    @M = ( "Dec" , "Jan" , "Feb" , "Mar" , "Apr" , "May" , "Jun" , "Jul" , "Aug" , "Sep" , "Oct" , "Nov" ) ;
    
    
    for ( $month = 5 ; $month <= 36 ; $month ++ ) {
    
      $lockin    = 120 * 0.85 if $month == 6   ;
      $lockin    = 140 * 0.85 if $month == 18  ;
      $shares1  *= 1.07 if $month == 18        ;
      $shares1  *= 1.07 if $month == 30        ;
      $options1 *= 2 if $month == 24           ;
    
      $spent    += $shares1                   ;
      printf("%4s %4d %6d %6d %6d: ",$M[$month % 12 ],2000+int(($month-1)/12),$shares,$options,$spent/$exchange) ;
    
      for ($price = 50 ; $price < 200 ; $price += 10 ) {
        $option = $options * ($price - 71)   ;
        $option = 0  if $option < 0          ;
        $share  = $price * ($shares + ($shares1 / $lockin))  ; # how much are the shares worth
    
    
        $value  = $option           if $method ==1 ;
        $value  = $share            if $method ==2 ;
        $value  = $option + $share  if $method ==3 ;
        $value /= $exchange                        ;
        printf("%6d ",$value)      ; # print it
      }
      print "\n" ;
    
      $shares  += $shares1 / $lockin ; 
      $options += $options1          ;
    }
}

__END__
:endofperl
