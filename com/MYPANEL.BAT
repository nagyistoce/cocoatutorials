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
#!/usr/bin/perl -w
#line 14

if ( scalar @ARGV ) {
  my     $arg = shift ;
  system("rundll32.exe shell32.dll,Control_RunDLL mypanel.dll @".$arg ) ;
} else {
   print "Syntax: mypanel [ 0 | 1 ]\n" ;
   system("start control")  ;
}

sub Die
{
   my    $arg = shift ;
   print $arg . "\n"  ;
   exit(0) ;
}

__END__
:endofperl

