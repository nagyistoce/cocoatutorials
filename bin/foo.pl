#!/usr/bin/perl

sub println
{
	my                  $argc = @_; # scalar = length of the argument array @_
	for ( $i = 0 ; $i < $argc ; $i++ ) {
		my $x = shift ;
		print($x) ;
	}
	print "\n";
}

my @array = ( 101,102,103,104);
# @array.push('abc');
# @array.push('10');

my $ref = \@array;

for ( my $i = 0 ; $i < scalar(@array) ; $i++ ) {
	println($array[$i], '  ref value ',$$ref[$i]) ;
}
println ('--------------------');
for ( my $i = 0 ; $i < @$ref ; $i++ ) {
	println($array[$i], ' ref value ',$$ref[$i]) ;
}


# That's all Folks
##
