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

    $menu  = ""
    . "\n" . '<div id="Menu" style=";z-index:0;">'
    . "\n" . '       <img src="/Spring2003/tnt.gif"><br>'
    . "\n" . '       <a href ="/Spring2003/index.shtml"      >Home - This week</a><br>'
    . "\n" . '       <a href ="/Spring2003/buddywalks.shtml" >Buddy Walks</a><br>'
    . "\n" . '       <a href ="/Spring2003/calendar.shtml"   >Calendar</a>&nbsp;/&nbsp;<a href ="/Spring2003/prevweeks.shtml"   >Previous&nbsp;Weeks</a><br>'
    . "\n" . '       <a href ="/Spring2003/coach.shtml"      >Coach</a>&nbsp;/&nbsp;<a href ="/Spring2003/manager.shtml">Manager</a><br>'
    . "\n" . ''
    . "\n" . '       <a href ="/Spring2003/fundraising.shtml">Fundraising Ideas</a><br>'
    . "\n" . '       <a href ="/Spring2003/honorees.shtml"   >Honorees</a><br>'
    . "\n" . '       <a href ="/Spring2003/maps.shtml"       >Maps - Directions</a><br>'
    . "\n" . '       <a href ="/Spring2003/groups.shtml"     >Mentor Groups</a><br>'
    . "\n" . '       <a href ="/Spring2003/photos.shtml"     >Photos</a>&nbsp;/&nbsp;<a href ="/Spring2003/links.shtml"      >Links</a><br>'
    . "\n" . '       <a href ="/Spring2003/poems.shtml"      >Poems</a>&nbsp;<br>'
    . "\n" . '       <a href ="/Spring2003/pr.shtml"         >Public Relations</a><br>'
    . "\n" . '       <a href ="/Spring2003/support.shtml"    >Support - Contacts</a><br>'
    . "\n" . '       <a href ="/Spring2003/why.shtml"        >Why I joined stories</a><br>'
    . "\n" . '       <div style="position:relative; top=-5px;z-index:2;">'
    . "\n" . '         <img src="/Spring2003/runners2.gif">'
    . "\n" . '       </div>'
    . "\n" . '       <div style="position:relative; top=-30px;z-index:0;">'
    . "\n" . '         Web<br> Captain:<br>'
    . "\n" . '         <a href="/Spring2003/robin.shtml">Robin Mills</a><br>'
    . "\n" . '         <a href="mailto:webcaptain@earthlink.net"><i>webcaptain@earthlink.net</i></a><br>'
    . "\n" . '         <a href="http://www.clanmills.com">www.clanmills.com</a><br>'
    . "\n" . '       </div>'
    . "\n" . '       <h3>GO <span style="color:purple">TEAM!</span></h3>'
    . "\n" . '</div>'
    ;

    for $line ( @lines ) {
      $line =~ s|<!--#include file="menu.inc" -->|$menu|;
      print FILE $line ;
    }
}

sub Action
{
    my $pattern = shift ;

    #
    # Get all the files from the command line and build the web addresses
    #
    my %files   = GetFiles($pattern) ;
    my $file    ;
    foreach $file ( keys %files ) {
        my $filename = $file ;
        print $filename . "\n" ;
        Process($filename) ;
    }
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

Action("*/*.shtml")  ;
Action("*.shtml")    ;



__END__
:endofperl
