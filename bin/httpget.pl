#!/usr/bin/perl

use LWP::Simple;

# Specification
# httpget url [filename]

sub println
{
	my $x = shift ;
	print($x . "\n") ;
}

sub syntax
{
	println("usage httpget.pl url [filename]\n") ;
	exit(1) ;
}

my $url      = shift or syntax() ;
my $filename = shift or 0        ;

$content = get $url;
if ( $filename ) {
	println("httpget.pl $url") ;
	open OUTPUT ,">$filename" ;
	print OUTPUT $content     ;
	close OUTPUT              ;
} else {
	print($content) ;
}



