@rem = '--*-Perl-*--
@echo off

: -------------------------------
: Execute Perl on both log files
: -------------------------------
:perly
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %*
goto endofperl
:WinNT
perl -x -S "%0" %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
goto endofperl


: -------------------------------
: The Perl Script
: -------------------------------
@rem ';
#!/usr/bin/perl -w
#line 23

die "Keepy pattern ...\n" if (scalar @ARGV) <= 0 ;

my %keep      = GetFiles(@ARGV) ; # keep these files
my %directory = GetFiles("*.*") ; # all files in directory

#
# subtract the keep files from the directory to get dead
#
my $file ;
my %dead ;
foreach $file ( keys %directory ) {
  if ( !exists $keep{$file} ) {
     $dead{$file} = $file  ;
  }
} 

foreach $file ( keys %dead ) {
  unlink $file ;
}

#
# GetFiles - find files which match the input pattern
#
sub GetFiles
{
    my      %result       ;
    my      $pattern      ;
    foreach $pattern ( @_ ) {
      my $file  ;
      my @files = glob($pattern) ;
      foreach $file (@files) {
        if ( -e $file && !-d $file ) {
          $result{$file} = $file ;
	}
      }
    }
    return %result ;
}

__END__
:endofperl
