#!/usr/bin/perl

use Imager;

my $image = Imager->new;

my $source = shift; # from the command-line
$image->read(file=>$source)
    or die "Cannot load $image_source: ", $image->errstr;
    
print ("width = " . $image->getwidth . " height = " . $image->getheight . "\n");

# That's all Folks!
##
