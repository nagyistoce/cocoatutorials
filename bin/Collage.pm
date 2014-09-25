package Collage;

# use strict;
# use warnings;

use XML::Parser::Expat;
use File::Basename;
use Math::Trig;
use Cwd;
use Cwd 'abs_path';

##
# forward declarations
sub OK; # ($filename)
sub read; # ($filename,$W,$H)
sub atoi;
sub ch;
sub eh;
sub sh;
sub println; 
sub error;
sub getHead;
sub getShtml;
sub getTail;
sub poly;

##
# global variables
my $x;
my $y;
my $w;
my $h;
my $t;
my $p;
my $s; # position and photo name
my $src = 0				; # boolean - are we in <src>....</src>?
my @map					;
my $pi = 4.0*atan(1.0)	; # simple simon
my $W ;
my $H ;
my $filename;

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
	
	$p =~ s#\\#/#g;                     # DOS to UNIX / conversion
	$p =~ s#My\ Pictures#Pictures#g;    # Picasa Windows 
	$p =~ s#My\ Documents#Documents#g;  # 
	$p =~ s#\$.*/Photos/#/#g;           # $My Documents $Pictures .... /Photos/ -> /
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
	my $x = shift;
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

sub OK # ($filename,$force)
{
	my $filename = shift;
	my $force    = shift;
	
	my($file, $dirs, $ext) = fileparse($filename,qr/\.[^.]*/);
	
	##
	# Test for illegal current directory
	my $pwd = abs_path(getcwd);
	die("do not run in the directory with the collages") if $pwd eq abs_path($dirs) ;
	$force |= $pwd =~ m/Homepages/;
	die ("you are not in Homepages") if ! $force;


	##
	# Test for cxf and jpg file exist
	$cxf       = $dirs . $file . '.cxf' ;
	$jpg       = $dirs . $file . '.jpg' ;
	die ("$cxf does not exist") if ! -e $cxf;
	die ("$jpg does not exist") if ! -e $jpg;
}
	
sub read # ($filename,$W,$H)
{
	$filename = shift or die "syntax error" ;
	$W		  = shift or die "syntax error" ;
	$H		  = shift or die "syntax error" ;
	
	OK($filename);
	
	# perl at its most powerful and horrible
	#
	my($file, $dirs, $ext) = fileparse($filename,qr/\.[^.]*/);
	
	$W = atoi($W) ;
	$H = atoi($H) ;
	return 1 if $W == 0 || $H == 0 ;
	
	# open and parse the CXF file
	#
	my  $cxf = $dirs . $file . '.cxf';
	open(CXF, $cxf) or error("Couldn't open file $cxf") ; 
	$parser = new XML::Parser::Expat;
	$parser->setHandlers('Start' => \&sh,'End' => \&eh,'Char'  => \&ch) ;
	$parser->parse(*CXF);
	close(CXF);
	
	# output result
	my   @result;
	push(@result,"<map id=\"$file\" name=\"$file\">") ;
	foreach $line (reverse(@map)) {
		push(@result,$line);
	}
	push(@result,"</map>") ;

	push(@result,'<table><tr><td class="story_head">__TITLE__<br>');  
	push(@result,"<img class=\"home\" src=\"${file}.jpg\" width=\"$W\" usemap=\"#${file}\">") ;
	
	##
	# copy the story (if it exists)
	my $story  = $dirs . $file . ".txt";
	if ( -e $story ) {
		open(STORY,$story); 
		push(@result,'</td></tr><tr><td>');
		foreach $line (<STORY>) {
			chomp($line); 
			push(@result,$line);
		} ;
		close(STORY);
	}

	push(@result,'</td></tr></table>');
	return @result;
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
          <td><img src="/unionjack.gif" style="border:none;" height="50"><br>
          <!-- <img src="/3dflagsdotcom_usa_2fawm.gif" style="border:none;"><br>-->
          <!-- <img src="/lionflag.gif" style="border:none;"><br>-->
          <img src="/england-flag.gif" style="border:none;" height="40"><br>
          <img src="/scotflag.gif" style="border:none;"  height="40"><br>
          <img src="/3dflagsdotcom_usa_2fawm.gif" style="border:none;" height="40">
        </td><td>

<div id="c_a1aa8a4e127ee4f9910570c1d8f1136e" class="normal"><h2 style="color: #ffffff; margin: 0 0 3px; padding: 2px; font: bold 13px/1.2 Tahoma; text-align: center; width=100%"><a href="http://www.forecast.co.uk/camberley.html" style="color: #ffffff; text-decoration: none; font: bold 13px/1.2 Tahoma;">Weather in Camberley</a></h2><div id="w_a1aa8a4e127ee4f9910570c1d8f1136e" class="normal" style="height:100%"></div></div><script type="text/javascript" src="http://www.forecast.co.uk/widget/loader/a1aa8a4e127ee4f9910570c1d8f1136e"></script>
        <!--
        <script type="text/javascript" src="http://voap.weather.com/weather/oap/USGA0028?template=HOMEH&par=3000000007&unit=1&key=twciweatherwidget"></script>
		<span style="display: block !important; width: 320px; text-align: center; font-family: sans-serif; font-size: 12px;"><a href="http://www.wunderground.com/cgi-bin/findweather/getForecast?query=zmw:94114.1.99999&bannertypeclick=wu_clean2day" title="San Francisco, California Weather Forecast" target="_blank"><img src="http://weathersticker.wunderground.com/weathersticker/cgi-bin/banner/ban/wxBanner?bannertype=wu_clean2day_metric_cond&airportcode=KSFO&ForcedCity=San Francisco&ForcedState=CA&zip=94114&language=EN" alt="Find more about Weather in San Francisco, CA" width="300" /></a><br><a href="http://www.wunderground.com/cgi-bin/findweather/getForecast?query=zmw:94114.1.99999&bannertypeclick=wu_clean2day" title="Get latest Weather Forecast updates" style="font-family: sans-serif; font-size: 12px" target="_blank"></a></span>
		-->
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
    href="mailto:webmaster\@clanmills.com">webmaster\@clanmills.com</a>
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

# http://juerd.nl/site.plp/perlpodtut

=head1 NAME

Collage - A module for processing .cxf files

=head1 SYNOPSIS

    use Collage
    Collage.read(path-to-cxf,width,height);

=head1 DESCRIPTION

This module processes .cxf files

=head2 Methods

println
read

=over 12

=item C<new>

=item C<as_string>

=back

=head1 LICENSE

This is released under the Artistic License. See L<perlartistic>.

=head1 AUTHOR

Robin Mills robin@clanmills.com - L<http://clanmills.com/>

=head1 SEE ALSO

L<perlpod>, L<perlpodspec>

=cut


##
# always last
1 ;
# That's all Folks!
##
