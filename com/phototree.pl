#!/usr/bin/perl -w

#
#  Forward declarations
#
sub DirDump   ; # do the business
sub Println   ; # print args + nl 
sub SetupOS   ; # Setup variables list SEP and so on
sub MakePath  ; # Combine a directory and filename into path
sub Syntax    ; # Display the syntax
sub ParseAge  ;

#
#  initialize the globals
#
my $bPrint 		= 1 ; # 1 to print files/paths, 0 to just total
my $bQuote 		= 0 ; # 1 to print "paths"
my $bUnix 		= 0 ;
my $bDos  		= 0 ;
my $bPrintSizes = 1 ;
my $bDepth      = 0 ; # 

#
#  Setup the OS (path separator stuff)
#
SetupOS() ;

#
#  Process the command line arguments
#
my @args = @ARGV ;

my @dirs         ;
my $bAge     = 0 ;
my $bPrefix  = 0 ;
my $sPrefix  = 0 ;
my $bBefore  = 0 ;
my $sBefore  = 0 ;
my $nAge     = 0 ;
my $nDepth       ;
my $nDirDepth=0  ;

foreach $arg ( @args )
{
	my $bProcess = ( substr($arg,0,1) eq '-' || $bAge || $bPrefix || $bBefore || $bDepth ) ? 0 : 1 ;
  	push @dirs,$arg if ( $bProcess ) ;

	$nAge    	 = ParseAge($arg) if $bAge;
	$nDepth    	 = $arg      if $bDepth   ;
	$sPrefix 	 = $arg if $bPrefix       ;
	$sBefore 	 = $arg if $bBefore       ;
  	$bAge    	 = $arg eq '-age'         ;
  	$bPrefix 	 = $arg eq '-prefix'      ;
  	$bBefore 	 = $arg eq '-before'      ;
	$bDepth      = $arg eq '-depth'       ;

  	$bPrintSizes = 0 if $arg eq '-nosize' ;
  	$bPrintSizes = 1 if $arg eq '-size'   ;
  	$bPrintSizes = 0 if $arg eq '-age'    ;
  	$bUnix       = 1 if $arg eq '-unix'   ;
  	$bDos        = 1 if $arg eq '-dos'    ;
  	$bQuote      = 1 if $arg eq '-quote'  ;
  	$bQuote      = 0 if $arg eq '-noquote';
}
push @dirs,"." if scalar @ARGV && ! scalar @dirs ;

#
#  now do the business
#
foreach my $dir ( @dirs )
{
   	DirDump($dir) ;
}   	

#
# syntax error if we didn't do anything!
#
Syntax() if ! scalar @dirs ;


# -------------------------------------
# Syntax - report the syntax of this program
# -------------------------------------
sub ParseAge
{
	my $age  = shift ;
	my $u    = substr($age,length($age)-1) ;
	my $unit ;

	$unit =                1 if $u eq 's' ;
	$unit =               60 if $u eq 'm' ;
	$unit =          60 * 60 if $u eq 'h' ;
	$unit =     24 * 60 * 60 if $u eq 'd' ;
	$unit = 7 * 24 * 60 * 60 if $u eq 'w' ;

	$age = substr($age,0,length($age)-1) if $unit ;
	$unit = 60 if !$unit                          ;

	$age = int($age) * $unit ;

	return $age ;
}


# -------------------------------------
# Syntax - report the syntax of this program
# -------------------------------------
sub Syntax
{
	Println("tree [-option]+ dir ...") ;
	Println("       option: -nosize | -size | -dos | -quote | -unix | -age minutes[s|m|h|d|w] | -prefix string | -before string -depth n") ;
}


# -------------------------------------
# SetupOS - set up PATH_SEP and dotSep
# -------------------------------------
sub SetupOS
{
    my $result = $^O =~ m:^mswin32:i ; # is OSNAME == mswin32 ?
    $PATH_SEP  =$result?  "\\" : "/" ; # PATH_SEP = \ on NT and / on UNIX
	$dotSep = "." . $PATH_SEP ;
    return $result ;
}


# -------------------------------------
# Println - print arguments with space and \n
# -------------------------------------
sub Println
{
  	foreach my $arg ( @_ )
  	{
    	print $arg . " "  ;
  	}
  	print "\n"
}


# -------------------------------------
# MakePath(directory,filename) -> path
# -------------------------------------
sub MakePath
{
    my     $dir      = shift ;
    my     $filename = shift ;
    my     $l        = length($dir) ;
    my     $sep      = $PATH_SEP ;
    # remove trailing separators
    while ( substr($dir,$l-1) eq $sep )
    {
      	$dir = substr($dir,0,$l-1) ;
      	$l   = length($dir) ;
    }
    
    # remove leading separators
    while ( substr($filename,0,1) eq $sep )
    {
      	$filename = substr($filename,1) ;
    }
    
    return $dir . $sep . $filename ;
}

my $nDirs = 0 ;
# -------------------------------------
# DirDump(directory) -> bytes in directory
# -------------------------------------
sub DirDump
{
  	my $dir   = $_[0] ;
  	my $total = 0     ;

    $nDirDepth ++ ;
	return $total if $nDepth && ($nDirDepth > $nDepth) ;

  	# print sBefore on the first occasion
  	Println($sBefore) if $sBefore && !$nDirs ;
  	$nDirs ++ ;
  	
  	return $total if ! -d $dir           ;

  	opendir              DIRECTORY, $dir ;
  	my @files = readdir  DIRECTORY       ;
                closedir DIRECTORY       ;

  	#
  	# Report every file/link in the directory
  	#
  	foreach $file (@files)
  	{
    	next if ( $file eq '.' || $file eq '..' ) ;

    	my $path = MakePath($dir,$file) ;
    
    	my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
        	   $atime,$mtime,$ctime,$blksize,$blocks)
            	   = stat($path);

    	my $link = readlink($path) ;
    	my $post = ""         ; 
    	my $bprint = $bPrint  ;
    
    	if ( $link )
    	{
        	$size = -1 ;
        	$post = " => " . $link ;
    	}
    	else
    	{
      		# ignore directories
      		# ignore .filenames (hidden in UNIX)
      		if ( -d $path ) { # || substr($file,0,1) eq "." ) {
        		$bprint = 0 ;
      		}
    	}  
    	$total += $size if $size  ;
    	my $age = time() - $mtime ;
    
    	$bprint = 0 if ( $nAge && $age > $nAge )  ;
    
    	if ( $bprint )
    	{
      		my $p  = substr($path,0,2) eq $dotSep ? substr($path,2) : $path ;
      		$p     =~ s#\\#/#g        if $bUnix       ;
      		$p     =~ s#/#\\#g        if $bDos        ;
			$p     = '"' . $p . '"'   if $bQuote      ;
      		print ($sPrefix . " "   ) if $sPrefix     ;
      		printf("%9d "   , $size ) if $bPrintSizes ;
      		printf("%s%s\n" , $p, $post) 
    	}  
  	}

  	#
  	# Recurse every directory
  	#
  	foreach $file (@files)
  	{
    	next if ( $file eq '.' || $file eq '..' ) ;

    	my $path = MakePath($dir,$file) ;
    	my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
           $atime,$mtime,$ctime,$blksize,$blocks)
               = stat($path);

    	my    $link = readlink($path) ;
    	if ( !$link && -d $path ) {
      		$total += DirDump($path) ;
    	}
  	}

    $nDirDepth -- ;
  	return $total ;
}
