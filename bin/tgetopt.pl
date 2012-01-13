#!/usr/bin/perl -w

##
# our module (load from ~/bin)
use lib "$ENV{HOME}/bin";
use Collage;

use Getopt::Long qw/:config auto_help/;

my @files  ;
my $length = 24;
my $verbose= 0;
$result = GetOptions (	"length=i"  => \$length,    	# numeric
   				   		"file=s"    => \@files,      	# string
					   	"verbose"   => \$verbose,  	# flag
);
                               
Collage::println("length  = $length");
Collage::println("verbose = $verbose");
Collage::println("result  = $result");

my $arg;

for $arg (@ARGV) {
	Collage::println("arg = $arg");
}

for my $file (@files) {
	Collage::println("file = $file");
}



=head1 NAME

My::Module - An example module

=head1 SYNOPSIS

    use My::Module;
    my $object = My::Module->new();
    print $object->as_string;

=head1 DESCRIPTION

This module does not really exist, it
was made for the sole purpose of
demonstrating how POD works.

=head2 Methods

=over 12

=item C<new>

Returns a new My::Module object.

=item C<as_string>

Returns a stringified representation of
the object. This is mainly for debugging
purposes.

=back

=head1 LICENSE

This is released under the Artistic 
License. See L<perlartistic>.

=head1 AUTHOR

Juerd - L<http://juerd.nl/>

=head1 SEE ALSO

L<perlpod>, L<perlpodspec>

=cut


# That's all Folks!
##
