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

die "webdir pattern ...\n" if (scalar @ARGV) <= 0 ;

#
# Find where we are on Drive C:
#
use Cwd ;
$dir = cwd() ;
die "You must be on drive C: to use this script\n" if $dir !~ /^[Cc]:/ ;

#
# Create the Web address
#
$dir = (substr $dir,2) . "\\"                    ;
$url = "ftp://rmills-dell" . $dir               ;

# deal with Janeway publications
# http://janeway.corp.adobe.com/PTG-Core/Copernicus/Documents/Architecture/Overall/Arch.pdf
if ( $dir =~ m/Copernicus/ ) {
  $d    = $dir                               ;
  $d    =~ s.Copernicus/current.Copernicus.  ;
  $url  =  "http://janeway.corp.adobe.com/PTG-Core" . $d    ;
  $url  =  "http://janeway/PTG-Core"                . $d    ;
}

#
# Get all the files from the command line and build the web addresses
#
my %files   = GetFiles(@ARGV) ;
my $file    ;
foreach $file ( keys %files ) {
    $webpath = $url . $file  ; # create the name
    $webpath =~ s.\\./.g     ; # convert \ to /
        $path    = $dir . $file  ; # 
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
           $atime,$mtime,$ctime,$blksize,$blocks)
               = stat($path);

    print "<$webpath> ($size bytes)\n"    ; # print it
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
