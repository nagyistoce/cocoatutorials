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
#line 24

sub Process
{
    my $filename = shift ;
    open(FILE,$filename) ;
    @lines = <FILE> ;
    close(FILE) ;

    open(FILE,">$filename") ;

    for $line ( @lines ) {
      $line =~ s|</TABLE></BODY>|</TABLE><p align="right"><img src="../llslogo.gif" align="middle">&nbsp;<img src="../tnt.gif" align="middle"></p></BODY>|     ;

      print FILE $line ;
    }
}


my $old     = ".htm"   ;
my $new     = ".shtml" ;
my $pattern = "*.shtml"  ;


#
# Get all the files from the command line and build the web addresses
#
my %files   = GetFiles($pattern) ;
my $file    ;
foreach $file ( keys %files ) {
    my $name = $file ;

    print $name . "\n" ;

    Process($name) ;
} 

#
# GetFiles - find files which match the input pattern
#
sub GetFiles
{
    my     %result               ;
    my     $pattern = shift      ;
    my @files = glob($pattern) ;
    foreach my $file (@files) {
      if ( -e $file && !-d $file ) {
        $result{$file} = $file ;
      }
    }
    return %result ;
}

__END__
:endofperl
