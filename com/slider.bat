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

$title = shift || die "Syntax: slider <title>\n" ;

sub getFileName
{
   my $line   = shift ;
   my $l      = length($line) ;
   my $finish = 0 ;
   my $start  = 0 ;
   my $i          ;

   for ( $i = 0 ; $i < $l ; $i++ ) {
      my $s = substr($line,$i,12) ;
      if ( $s  eq 'src="Thumbs/') {
        $start = $i +12 ;
      }
   }

   for ( $i = $start ; $i < $l ; $i++ ) {
      my $s = substr($line,$i,4) ;
      if ( $s eq '.jpg') {
        $finish = $i+4 ;
      }
   }

   # printf("start, finish = %d %d\n",$start,$finish) ;

   my $result = $finish > $start ? substr($line,$start,$finish-$start) : "--unknown--" ;

   $result =~ s/'/\\'/ ;
   return $result ;
}

sub Process
{
    my $filename = "default.shtml" ;
    open(FILE,$filename) ;
    @lines = <FILE> ;
    close(FILE) ;

    open(FILE,">slides.shtml") ;

    print FILE '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">' ."\n" ;
    print FILE '<HTML>' . "\n" ;
    print FILE '<HEAD>' . "\n" ;
    print FILE '<meta http-equiv="Content-Type" content="text/html; charset=windows-1252">' . "\n" ;
    print FILE '<meta http-equiv="Content-Language" content="en-us">' . "\n" ;
    print FILE '<LINK REL="stylesheet" TYPE="text/css" HREF="/page.css">' . "\n" ;

    print FILE '<!--#include file="/slides.js" -->' . "\n" ;

    print FILE '<TITLE>' . $title . '</TITLE>' . "\n" ;
    print FILE '</HEAD>' . "\n" ;
    print FILE '<BODY' . "\n" ;
    print FILE '  onLoad   ="' . "\n" ;


    for $line ( @lines ) {
      if ( $line =~ /valign=bottom/ ) {
        $name = getFileName($line) ;
        # print $name . "\n\n" ;
        print FILE '  AddImage(\'Images/' . $name . "') ;\n" ;
      }
    }

    print FILE '  window_onload();"' . "\n" ;
    print FILE '>' . "\n" ;

    print FILE '<!--' . "\n" ;
    print FILE '  onresize ="window_onresize();"' . "\n" ;
    print FILE '-->' . "\n" ;
    print FILE '' . "\n" ;
    print FILE '<!--#include file="/menu.inc" -->' . "\n" ;
    print FILE '<div id = "Content">' . "\n" ;
    print FILE '<H1>' . $title . '</h1>' . "\n" ;
    print FILE '<!--#include file="/slides.inc" -->' . "\n" ;
    print FILE '</div>' . "\n" ;
    print FILE '' . "\n" ;
    print FILE '</BODY></HTML>' . "\n" ;
}

Process() ;




__END__
:endofperl
