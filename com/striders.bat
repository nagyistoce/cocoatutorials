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

my $arg = scalar @ARGV ? shift : "Photos by Robin" ;
print $arg . "\n"  ;

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
           . "<p>${arg}.   Page created: ${now}</p>"
           . "\n</center>"
           . "<!--#include virtual=\"/photos/footer.inc\" -->"
           ;

    $box1  =   "\n  <table><tr><td><div class=boxshadow><div class=boxmain>\n    " ;
    $box2  =   "\n  </div></div></table>\n\n" ;



    for $line ( @lines ) {
      $line =~ s|</h1></td>|</h1td>| ; # a dodge, we'll restore this later!
      $line =~ s| </td>|</ td>|      ; # another dodge
      $line =~ s|<td valign=bottom>|<td valign=bottom>${box1}|          ;
      $line =~ s|</td>|${box2}</td>|                                    ;
      $line =~ s|<BODY>|<body><!--#include virtual="/photos/menu.inc" -->| ;
      $line =~ s|htm|shtml|                                             ;
      $line =~ s|album.css|/photos/album.css|                           ;
      $line =~ s|up.gif|/photos/up.gif|                                 ;
      $line =~ s|next.gif|/photos/next.gif|                             ;
      $line =~ s|prev.gif|/photos/prev.gif|                             ;
      $line =~ s|<TABLE><tr>|<center><table cellpadding=5><tr>|         ;
      $line =~ s|<TABLE>|<center><table width=750>|                     ;
      $line =~ s|</TABLE></BODY></HTML>|</table>${stuff}|               ;
      $line =~ s|<img src="T|<Img class ="thumb" src="T|                ;
      $line =~ s|<td colspan=2><img|<td colspan=2 align=center>${box1}<img class="photo"| ;
      $line =~ s|</h1td>|</h1></td>| ; # restore
      $line =~ s|</ td>|</td>|       ; # 

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
