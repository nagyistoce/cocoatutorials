#!/usr/bin/perl

##
# functions
sub println
{
	my $argc = scalar(@_)-1; # scalar = length of the argument array @_
	for ( $i = 0 ; $i <= $argc ; $i++ ) {
		my $x = shift ;
		print $x;
		print ' ' if $i < $argc ;
	}
	print "\n";
}

##
# array and references
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

##
# dispatcher
sub a;
sub b;
sub c;

my %dispatch = 
(	'a' 		=> \&a
,	'b' 		=> \&b
,	'c' 		=> \&c
,   'default_' 	=> \&_default
);

println("arg = $ARGV[0]");

my $x = $dispatch{$ARGV[0]} ? $dispatch{$ARGV[0]} : $dispatch{'default_'};
my  @args = reverse @ARGV;
pop @args;
    @args = reverse @args;
$x->(@args);

sub a
{
	my $count = scalar(@_);
	print ("I am a argc = $count = ");
	println(@_);
}

sub b
{
	my $count = scalar(@_);
	print ("I am b argc = $count = ");
	println(@_);
}

sub c
{
	my $count = scalar(@_);
	print ("I am c argc = $count = ");
	println(@_);
}

sub _default
{
	print ("I am _default ");
	println(@_);
}

# That's all Folks
##
