package Collage; # use package to declare a module

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
	
	my $path  =  $file . '.inc';
	open (OUTPUT, ">$path");
	
	# output the map
	#
	println("<img class=\"home\" src=\"${file}.jpg\" usemap=\"#${file}\">") ;
	println("<map id=\"$file\" name=\"$file\">") ;
	foreach $line (reverse(@map)) { println($line) } ;
	println("</map>") ;
	
	close(OUTPUT);
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

###############################
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

###############################
# process a char (string) event
sub ch
{
	my ($expat, $el, %atts) = @_;
	if ( $src ) {
		$p = $el ;
		$src = 0 ;
	}
}

###############################
# process an end-of-element event
sub eh {
	my ($expat, $el) = @_;
	if ( $el eq 'node' ) {
		# print "calling poly W,H,x,y = $W,$H,$x,$y\n";
		poly($W,$H,$x,$y,$w,$h,$t,$p,$s) ;
	}
}

###############################
# utilities
sub println 
{
	my $x = shift ;
	print OUTPUT $x . "\n" ;
}

sub error
{
	my $x = shift	;
	println($x) ;
	return(1)		;
}

sub atoi
{
	my $t;
	foreach my $d (split(//, shift())) {
		$t = $t * 10 + $d;
	}
	return $t;
}

##
# always last
1 ;
# That's all Folks!
##
