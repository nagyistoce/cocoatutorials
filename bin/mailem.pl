#!/usr/bin/perl -w

	# ---------------------------------------
	# declarations
	use Net::SMTP ;
 	use Mail::Sender;

	sub Syntax    ;
	sub errorCode ;

	# ---------------------------------------
	# static data
	my $smtp	=   'mail.earthlink.net'  ;
	my $from 	# 	'robin@clanmills.com' ;
				=	'board@stevenscreekstriders.org' ;



	my $address   ;
	my @addresses ;
	my $message   ;

	# ---------------------------------------
	# read and test the command line
	my $addressfile  = shift or Syntax() ;
	my $messagefile  = shift or Syntax() ;
	my $subject      = shift or Syntax() ;
	my $file         = shift or 0        ;

	if ( !-e $addressfile ) {
		die "adressfile $addressfile does not exist!" ;
	}
	if ( !-e $messagefile ) {
		die "messagefile $messagefile does not exist!" ;
	}

#	print "files = $addressfile  $messagefile\n" ;

	# ---------------------------------------
	# read the address file
	open(FILE,$addressfile) ;
	while ( $address = readline(FILE) ) {
		chomp $address ;
		push @addresses,$address if $address !~ /#.*$/ && length($address)  ;
	}
	close FILE ;

	# ---------------------------------------
	# read the message file
	open (FILE,$messagefile) ;
	read  FILE,$message,100000  ;
	close FILE ;

#	foreach $address ( @addresses ) {
#		print "address = $address \n" ;
#	}
#	print "message = " . $message . "\n" ;

	# ---------------------------------------
	# send the emails one at a time
	foreach $address ( @addresses )
	{
	 	my $sender = new Mail::Sender
  			{ smtp => $smtp, from => $from };
  		#	{ smtp => 'mail.earthlink.net', from => 'robinwmills@earthlink.net' };

	#	print "subject = $subject " ;
	#	print "from    = $from "    ;
		my $options = 	{ to      	=> $address
			  			, subject 	=> $subject
			  			, msg  		=> $message
			  			, from 		=> $from
			  			} ; # this is reference to a hash

		# extend the options
		my $s ;
		if ( $file ) {
			${$options}{'file'} = $file ;
		#	print "file    = $file "    ;
	 		$s = $sender->MailFile($options) ;
		} else {
			$s = $sender->MailMsg($options) ;
		}
		
		my $ec = errorCode($s) ;
        print("error code = $ec            ") ;
		print($address . "            ") ;
        print("\n") ;
		$sender = 0 ;
	}

	sub errorCode
	{
		my $s = shift ;
		return "smtphost unknown" 						if $s == -1 ;
		return "socket() failed"  						if $s == -2 ;
		return "connect() failed"  						if $s == -3 ;
		return "service not available"  				if $s == -4 ;
		return "unspecified communication error"  		if $s == -5 ;
		return "local user $to unknown on host $smtp"  	if $s == -6 ;
		return "transmission of message failed"  		if $s == -7 ;
		return "argument $to empty"  					if $s == -8 ;
		return "no message specified in call to MailMsg or MailFile"  	if $s == -9 ;
		return "no file name specified in call to SendFile or MailFile"	if $s == -10 ;
		return "file not found"  						if $s == -11 ;
		return "not available in singlepart mode"  		if $s == -12 ;
    	return "success" ;
	}

	sub Syntax
	{
		print("syntax: mailem <addressfile> <messagefile> subject [filename]\n") ;
		exit ;
	}
