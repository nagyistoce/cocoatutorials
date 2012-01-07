#!/usr/bin/perl

use Imager;

sub println
{
	my $s = '';
	for my $a (@_) {
		$s .= $a . ' ';
	}
	print($s,"\n");
}


my $image = Imager->new;
for my $source (@ARGV) {
	$image->read(file=>$source) or die "Cannot load $image_source: ", $image->errstr;
	println("source = $source", "width =", $image->getwidth , "height =" , $image->getheight);
}

1;
# That's all Folks!
##
