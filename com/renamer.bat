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

die "Syntax:  renamer old new file-pattern\n" 
  . "Example: renamer TiffDisplay French *.*\n" if (scalar @ARGV) != 3 ;
my $old     = shift ;
my $new     = shift ;
my $pattern = shift ;


#
# Get all the files from the command line and build the web addresses
#
my %files   = GetFiles($pattern) ;
my $file    ;
foreach $file ( keys %files ) {
    my $oldname = $file ;
    my $newname = $file ;
    $newname    =~ s/$old/$new/i  ;

    print "rename $oldname $newname\n"    ; # print it
    rename($oldname,$newname) ;
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
