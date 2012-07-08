#!/usr/bin/perl -w
# (c) Robin Mills Software Consultancy Camberley England 1999
# ----------------------------------------------------------------- 
# * File   : ftpput.pl
# * Author : Robin Mills
# * Date   : 18.Feb.2005
# -----------------------------------------------------------------
# 
# * Purpose:
#   FTP put utility
# 
# * Useage :
#   ftpput host remotepath localpath [[user] [password] [-testlength]]
#
#   user/password = anonymous/ftpput.pl if omitted
# 
# * Example:
#   None.
# 
# * Description:
#   None.
# 
# * Restrictions:
#   None.
# 
# -----------------------------------------------------------------
# Modified By Reason
# 08.18.07 RM Added lots more retry code
# 08.17.07 RM Added retry, recursive mkdir, and size detection code
# -----------------------------------------------------------------

use File::Basename;
use Net::FTP      ;
sub open_ftp      ; 

sub usage { die "usage: ftpput <host> <remote> <local> [<user> <password> -testlength]\n" ; } ;

$host      = shift or usage ;
$remote    = shift or usage ;
$local     = shift or usage ;
$user      = shift or $user   = "anonymous" ;
$pass      = shift or $pass   = "ftpput.pl"  ;
$option    = shift or $option = ""  ;
$ftp       = 0 ;

open_ftp() ;

my   $retry = 5      ; # number of retries
my   $in = "xx"      ; # indicator.  -> for transmit, ~~ for skipped, xx for not local
my   $sp = " "       ; # space
my	 $ok = -e $local ;
my   $bTestLength = $option eq "-testlength" ;
if ( $ok  )
{
	#
	# convert \ to /
	#
	$local     =~ s#\\#/#g ;
	$remote    =~ s#\\#/#g ;

    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$localsize,
        	   $atime,$mtime,$ctime,$blksize,$blocks)
            	   = stat($local);

	my $remotesize ;
	$remotesize = $ftp->size($remote) if $bTestLength ;
	$remotesize = -1 if !defined($remotesize) ;

	if ( $localsize == $remotesize ) {
		$in = "~~"    ; # we skipped it
	}

	while ( $localsize != $remotesize && $retry  ) {
		$retry -- ;
		$ok = $ftp->put($local,$remote) ;
		if ( !$ok  ) # recursively create the directory if necessary
		{
			$ftp->mkdir(dirname($remote),1) ;
			$ok = $ftp->put($local,$remote) ;
		}
		if ( $ok  ) {
	   		$in = "->"    ; # we copied it
			$remotesize = $ftp->size($remote) ;
			$remotesize = -1 if !defined($remotesize) ;
		} else {
		  print "RETRY\n" ;
		  $ftp->quit() ;
		  open_ftp()   ;
		}
	}
} 
$ftp->quit() ;

print $in . $sp . $host . $remote . "\n" ;

sub open_ftp
{
	my $retry = 5 ;
	$ftp = 0 ;
	while ( !$ftp && $retry ) {
		$ftp = Net::FTP->new($host)  ;#      or die "Can't connect: $@\n" ;
		$retry -- ;
		if ( $ftp ) {
			$ftp->login($user,$pass) ;#          or die "Can't login: $@\n"   ;
			$ftp->cwd("/")           ;#          or die "Can't cwd\n"         ;
			$ftp->binary()           ;#          no comment
		}
	}
}



