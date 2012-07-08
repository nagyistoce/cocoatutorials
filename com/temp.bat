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

my $thisfile = TruncateThis() ;
#
# find the arguments
#

my $CD           = $ENV{"CD"} ;
   $CD  = "cd /d" if ( !$CD ) ;
my $TEMP = $ENV{"TEMP"}       ;


#
# Open the batch file and figure out what the target is
#
open (FILE,">>$thisfile") ;
print FILE   "$CD $TEMP\n" ;
print FILE   "color 2e\n" ;
print FILE   "title Temp\n" ;
close FILE                ;

# =====================================================

# ------------------------------------
# TruncateThis - find the bottom of the batch file and chop of the rest
# ------------------------------------
sub TruncateThis
{
    open THIS,"+<$0" ;
    my $line  = ""   ;
    my $count        ;
    while ( $line ne ":endofperl" && ($line = <THIS>) ) {
      chop($line)    ;
      $count ++      ;
    }
    my $now = tell THIS ;
    truncate THIS,$now  ;
    close    THIS ;
    return $0 ;
}

__END__
:endofperl
cd /d c:\temp
color 2e
title Temp
