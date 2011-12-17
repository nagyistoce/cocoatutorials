#!/usr/bin/env perl
# program svndiffer.pl
 
# -------------------------------------
# setupOS - set up PATH_SEP and dotSep
# -------------------------------------
sub setupOS
{
    my $result = $^O =~ m:^mswin32:i ; # is OSNAME == mswin32 ?
    $PATH_SEP  =$result?  "\\" : "/" ; # PATH_SEP = \ on NT and / on UNIX
            $dotSep = "." . $PATH_SEP ;
    return $result ;
}
 
# -------------------------------------
# println
# -------------------------------------
sub println
{
	print shift ;
    print "\n"  ;
}
 
# -------------------------------------
# static data
# -------------------------------------
my @exts = ( 'cpp$' , 'c$' , 'h$' ) ;
my $file = 'svnnorm.txt' ;
my $input ;
 
# -------------------------------------
# main program
# -------------------------------------
setupOS ;
system ("svn diff > $file") ;
open   (FILE       ,$file ) ;
 
while ( ($input = <FILE>) ) {
	if ( $input =~ /(^Index: )(\S+)/ ) {
		my $file = $2 ;
		my $bDone = 0 ;
		foreach my $ext ( @exts ) {
        	if ( $file =~ $ext ) {
				my $cmd = "norm $file" ;
				println($cmd) ;
				system ($cmd) ;
				$bDone ++ ;
			}
		}
		print($file . ' ignored') if !$bDone ;
	}
}
 
