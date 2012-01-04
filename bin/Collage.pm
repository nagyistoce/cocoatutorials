package Collage;

# use strict;
# use warnings;

use XML::Parser::Expat;
use File::Basename;
use Math::Trig;

##
# forward declarations
sub read;
sub println;
sub eh;
sub error;
sub atoi;

##
# global variables
my $x,$y,$w,$h,$t,$p,$s ; # position and photo name
my $src = 0				; # boolean - are we in <src>....</src>?
my @map					;
my $pi = 4.0*atan(1.0)	; # simple simon
my $W ;
my $H ;
my $filename;
	
sub read # ($filename,$W,$H)
{
	# the main program
	#
	$filename = shift or syntax() ;
	$W		 = shift or syntax() ;
	$H		 = shift or syntax() ;

	# perl at its most powerful and horrible
	#
	my($file, $dirs, $ext) = fileparse($filename,qr/\.[^.]*/);
	
	$W = atoi($W) ;
	$H = atoi($H) ;
	return 1 if $W == 0 || $H == 0 ;
	
	# open and parse the CXF file
	#
	$ext = '.cxf' ;
	$filename .= $ext if ( ! -e $filename && -e $filename . $ext ) ;
	# print("opening the file $filename \n");
	open(FOO, $filename) or error("Couldn't open file $filename") ; 
	$parser = new XML::Parser::Expat;
	$parser->setHandlers('Start' => \&sh,'End' => \&eh,'Char'  => \&ch) ;
	$parser->parse(*FOO);
	close(FOO);
	
	# output result
	my @result;
	push(@result,"<img class=\"home\" src=\"${file}.jpg\" width=\"$W\" usemap=\"#${file}\">") ;
	push(@result,"<map id=\"$file\" name=\"$file\">") ;
	foreach $line (reverse(@map)) { push(@result,$line) } ;
	push(@result,"</map>") ;
	
	return @result;
}


###############################
# push a <poly> into the map
sub poly
{
	my $W = shift ;
	my $H = shift ;
	my $x = shift ;
	my $y = shift ;
	my $w = shift ;
	my $h = shift ;
	my $t = shift ;
	my $p = shift ;
	my $s = shift ;

	$t    = $pi*2.0 - $t ;
	
	# print "poly has W,H,x,y,w,h,t,p,s = $W, $H, $x ,$y ,$w ,$h ,$t ,$p , $s \n";

	# adjust x,y for theta and width !
#	$x    += $w * cos($t) - $h * sin($t) ;
#	$y    += $w * sin($t) + $h * cos($t) ;

#	println("<-- eh x,y = $x $y -->\n") ;
	
	my $x1 = int($W * $x) ;
	my $y1 = int($H * $y) ;
	
	# print "x1,y2 = $x1,$y1\n";
	
	my $x2 = int ($x1 + ($W * $w * cos($t))) ;
	my $y2 = int $y1 - $W * $w * sin($t) ;
	
	my $x3 = int $x2 + $H * $h * sin($t) ;
	my $y3 = int $y2 + $H * $h * cos($t) ;
	
	my $x4 = int $x1 + $H * $h * sin($t) ;
	my $y4 = int $y1 + $H * $h * cos($t) ;
	
	$p =~ s#\$Pictures/Photos##g ;
	$p =~ s#jpg$#shtml#g		 ;
	$p =~ s#bmp$#shtml#g		 ;
	
	# print "poly has x1,x2,x3,x4 = $x1, $x2, $x3 $x4\n";
	
	push(@map,"<area shape=\"poly\" coords = \"$x1,$y1,$x2,$y2,$x3,$y3,$x4,$y4,$x1,$y1\" href= \"$p\"/>") ;
}

##
# process an start-of-element event
sub sh
{
	my ($expat, $el, %atts) = @_;
	if ( $el eq 'node' ) {
		# print('node = ') ;
		$x = $atts{'x'} ;
		$y = $atts{'y'} ;
		$w = $atts{'w'} ;
		$h = $atts{'h'} ;
		$t = $atts{'theta'} ;
		$s = $atts{'scale'} ;
		# print "sh read $x,$y,$w,$h,$t,$s\n";
	}
	$src = $el eq 'src' ? 1 : 0 ;
}

##
# process a char (string) event
sub ch
{
	my ($expat, $el, %atts) = @_;
	if ( $src ) {
		$p = $el ;
		$src = 0 ;
	}
}

##
# process an end-of-element event
sub eh {
	my ($expat, $el) = @_;
	if ( $el eq 'node' ) {
		# print "calling poly W,H,x,y = $W,$H,$x,$y\n";
		poly($W,$H,$x,$y,$w,$h,$t,$p,$s) ;
	}
}

##
# utilities
sub println 
{
	my $x = shift ;
	print $x . "\n" ;
}

sub error
{
	my $x = shift	;
	println($x) ;
	exit(1)		;
}

sub atoi
{
	my $t;
	foreach my $d (split(//, shift())) {
		$t = $t * 10 + $d;
	}
	return $t;
}

#############################################
# page templates

##
# page template
sub getShtml
{
return <<ENDOFFILE;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html><head>
<meta http-equiv="content-type" content="text/html"; charset="utf-8">
<meta http-equiv="content-language" content="en-us">
<link rel="stylesheet" type="text/css" href="/page.css">
<link rel="alternate" type="application/rss+xml" title="clanmills rss feed" href="http://www.clanmills.com/1.xml">
<title>Mills Family Website</title></head>
<body>

<!--#include virtual="/menu.inc" -->
<!--#include virtual="/Homepages/__YEAR__/__NAME__.inc" -->

</body></html>
ENDOFFILE
}

##
# page template
sub getHead
{
return <<ENDOFFILE;
<div id = "Content">
<div class=boxshadow><div class=boxmain style="width:__WIDTH__px;">
  <table width="__WIDTH__"><tr><td width="50%">
    <h1>Mills Family<br>WebSite</h1>
    <h4>__DATE__</h4>
    <table class="Content">
      <tr>
       <td valign=bottom><a href="/Homepages/2009/0330.shtml"><img class="button" src="/prev.gif"></a><br></td>
       <td valign=bottom><a href="/defaults.shtml"><img class="button" src="/up.gif"></a><br></td>
       <td valign=bottom><a href="/default-2004-01.shtml"><img class="button" src="/next.gif"></a><br></td>
       <td valign=bottom><a href="/default.shtml"><img class="button" src="/home.gif"></a><br></td>
      </tr>
    </table>
    </td><td width="50%" align="right">
      <table><tr>
          <td><img src="/3dflagsdotcom_usa_2fawm.gif" style="border:none;"><img src="/lionflag.gif" style="border:none;"><img src="/scotflag.gif" style="border:none;"></td>
      </tr><tr>
        <td><table><tr><td style="color:yellow">Current<br>Weather<br>San Jose<br>California</td><td align="center">
			<a href="http://www.wunderground.com/US/CA/San_Jose.html?bannertypeclick=miniTarget2">
			<img src="http://banners.wunderground.com/weathersticker/miniTarget2_both/language/www/US/CA/San_Jose.gif" border=0
			alt="Click for San Jose, California Forecast" height=50 width=150></a>
	  </td></tr></table>
    </td></tr></table>
  </td></tr></table>
ENDOFFILE
}

##
# page template
sub getTail
{
return <<ENDOFFILE;
<hr>
<table><tr><td width="__WIDth__">
    <p align="center"><a href="/">Home</a> <a>.........</a>
    <a href="/about.shtml">About</a></p>
    <p align="center">Page design &copy; 1996-__YEAR__ Robin Mills / <a
    href="mailto:webmaster@clanmills.com">webmaster@clanmills.com</a>
    </p>
    <p align="center">Page created: __DAY__ __DATE__</p>
    </p>
</td><td width="__width__">
<p align="right"><img src="/robinali.gif" align="middle" width=120 border=2 border=0></p>
</td></tr></table>

</div>
ENDOFFILE
}
#
#############################################


##
# always last
1 ;
# That's all Folks!
##
