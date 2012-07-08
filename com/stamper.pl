use Cwd;
my $dir = cwd;
use File::Basename;

($name,$path,$suffix) = fileparse($dir) ;

print "name   = $name\n" ;
print "path   = $path\n" ;
print "suffix = $suffix\n" ;


chop($path) ;
($name1,$path1,$suffix1) = fileparse($path) ;

print "name1   = $name1\n" ;
print "path1   = $path1\n" ;
print "suffix1 = $suffix1\n" ;

my $ rip2_source = (( 'source' eq lc $name ) && ( 'rip2' eq lc $name1 )) ;

die "you must be in rip2\\source to use this script" if !$rip2_source ;
$source  = "..\\public\\source"  ; die "I can't find $dir" if ( ! -d $source  ) ;
$include = "..\\public\\include" ; die "I can't find $dir" if ( ! -d $include ) ;


die "Syntax:  CPRstamper Objectname Interface\n" 
  . "Example: CPRstamer  FrontEnd   ICPRFrontEnd\n" if (scalar @ARGV) != 2 ;
my $Component   = shift       ;
my $Interface   = shift       ;

my $Component_C = $Component . "_i.c" ;
my $Component_H = $Component . ".h"   ;
my $Interface_C = $Interface . "_i.c" ;
my $Interface_H = $Interface . ".h"   ;

$I_H = "..\\public\\include\\$Interface_H" ;
$C_H = "..\\public\\include\\$Component_H" ;
$I_C = "..\\public\\source\\$Interface_C"  ;
$C_C = "..\\public\\source\\$Component_C"  ;

die "directory $Component already exists\n" if -d $Component ;
die "file $I_H already exists\n" if -e $I_H                  ;
die "file $C_H already exists\n" if -e $C_H                  ;
die "file $I_C already exists\n" if -e $I_C                  ;
die "file $C_C already exists\n" if -e $C_C                  ;

system("mkdir $Component") ;
chdir  $Component          ;
system("xcopy/s ..\\Component\\*.* .") ;






