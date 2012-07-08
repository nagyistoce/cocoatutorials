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
#!/usr/bin/perl
#line 14

my $arg = shift or die "Syntax: $0 directory\n"   ;
$result = "is a directory" if             -d $arg ;
$result = "is a file"      if !$result && -e $arg ;
$result = "does not exist" if !$result            ;

print "$arg $result\n" ;

__END__
:endofperl
