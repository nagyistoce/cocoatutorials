#!/usr/bin/perl -w

	# ---------------------------------------
	# declarations
 	use Mail::Sender;

 	$sender = new Mail::Sender
  			{ smtp => 'mail.earthlink.net', from => 'robinwmills@earthlink.net' };

	$options = 	{ to      => "robin\@clanmills.com"
			  	, subject => "Striders Newsletter"
			  	, msg  	=> "I attach the Striders Newsletter for October.  Enjoy.\nRobin"
			  	, from 	=> "robin\@clanmills.com"
			  	} ; # this is reference to a hash

	# extend the options
	${$options}{'file'} = 'x.pdf' ;
	

	my $key ;
	foreach $key ( keys( %{$options}) ) {  # @{$aref}
		print("key = " . $key . " => " . ${$options}{$key} .  "\n") ;
	}

 	my $s = $sender->MailFile($options) ;
	print("s = $s\n") ;
