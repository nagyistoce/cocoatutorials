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

sub TimeNow
{
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime time ;

    @WDays  = ( "Sunday" , "Monday"  , "Tuesday"  , "Wednesday", "Thursday", "Friday"  , "Saturday") ;
    @Months = ( "January", "February", "March"    , "April"    , "May"     , "June"
              , "July"   , "August"  , "September", "October"  , "November", "December"
              ) ;

    $mon   = $Months[$mon] ;
    $wday  = $WDays[$wday] ;
    $year += 1900 if $year < 1900 ;

    $result =  "$wday $mon $mday, $year" ;

    return $result ;
}


sub Process
{
    my $filename = shift ;
    open(FILE,$filename) ;
    @lines = <FILE> ;
    close(FILE) ;

    open(FILE,">$filename") ;

    $now   = TimeNow() ;
      
    $stuff = "\n"
           . '<hr>'
           . '<table><tr><td>' . "\n"
           . '    <p align="center"><a href="/">Home</a> <a>.........</a>' . "\n"
           . '    <a href="/about.shtml">About</a></p>' . "\n"
           . '    <p align="center">Page design &copy; 1996-2004 Robin Mills / <a' . "\n"
           . '    href="mailto:webmaster@clanmills.com">webmaster@clanmills.com</a>' . "\n"
           . '    </p>' . "\n"
           . '    <p align="center">Created: ' . $now . '</p>' . "\n"
           . '    </li>' . "\n"
           . '</td><td>' . "\n"
           . '<p align="right"><img src="/robinali.gif" align="middle"  width=120 border=2width=120 border=0></p>' . "\n"
           . '</td></tr></table>' . "\n"
           ;


    for $line ( @lines ) {
      $line =~ s|<BODY>|<BODY><!--#include file="/menu.inc" --><div id = "Content">|;
      $line =~ s|htm|shtml|           ;
      $line =~ s|</BODY>  |</DIV></BODY>|   ;
      $line =~ s|album.css|/album.css|      ;
      $line =~ s|up.gif|/up.gif|      ;
      $line =~ s|next.gif|/next.gif|      ;
      $line =~ s|prev.gif|/prev.gif|      ;
      $line =~ s|</TABLE></BODY>|</TABLE>${stuff}</BODY>|  ;

      print FILE $line ;
    }
}


my $old     = ".htm"   ;
my $new     = ".shtml" ;
my $pattern = "*.htm"  ;


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

    Process($newname) ;
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
