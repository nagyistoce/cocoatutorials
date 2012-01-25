#!/usr/bin/perl

### script to set up easy links to your apache files.
### first created by john gale, jgale@apple.com
### good for Tiger and Leopard.  Panther too, probably, or anything running apache 1.x or 2.x
### version 

### usage: $0 [username]
### sets up apache symlinks.  /apache/docs will point to /Users/username/Sites
### if no username, then it uses current username.

use Getopt::Long;
GetOptions(
	"help",			\$help,
	"verbose+",		\$verbose,
	"user=s",		\$workingUserName,
	"docs=s",		\$docsLocation,
	"<>",				sub { push @arg, $_[0] },  # remaining arguments
);

### need help?
if ($help) { &printUsage; exit; }

unless ($workingUserName) {  # unless we have a username,
	### we should find our working username
	if ($>) {  # if our UID is not root...
		print "Finding current user id and working name...\n";  # then we should find our username
		open(ID, "id $> |");
		while (<ID>) {
			if ($_ =~ /uid=\d*\((.*?)\)/) {  # we've found a username
				$workingUserName = $1;
			}  # end if
		}  # end while
	} else {  # if we're dealing with root and don't have a username...
		print "I'm trying to symlink /apache/docs to your user's ~/Sites folder.\n";
		print "However, the current working user is root, who doesn't have a ~/Sites folder.\n";
		print "I can set it up in /Library/WebServer/Documents if you want.  Do you want to do this?  [no]/yes: ";
		$setupAsRoot = <>;
		if ($setupAsRoot =~ /y/i) {
			print "Alrighty, I'm setting up in /Library/WebServer/Documents.\n\n";
			$workingUserName = "root";
		} else {
			print "Cancelling request.  Don't run this script as sudo if you want to set up for your user.\n";
			&usage();
			exit(0);
		}  # end if else
	}  # end if else
}  # end unless

print "Setting up for user $workingUserName.\n" if ($verbose);

### this script really does need to be run as root, the abovities are just for niceness.
print "Checking user id to ensure running as root...\n" if ($verbose);
if ($>) {  # if $> (effective user id) is greater than 0, we're not root
	print "This script needs to run as root.  Trying to re-execute as root with arg $workingUserName for username...\n";
	$cmd = "sudo $0 --user $workingUserName" . eval { " --verbose" if $verbose; } . eval { " --docs \"$docsLocation\"" if $docsLocation };
	system "sudo $cmd";
	exit 0;  # then exit, since we've just run it
}  # end if


### we should now be running this script as root.


### find out which system we're running on
print "Trying to find out which major OS version we're on...\n" if ($verbose);
open(VERS, "sw_vers -productVersion |");
while (<VERS>) {
	if ($_ =~ /^10\.(\d*)/) { # we've found a product version
		$osxMajorVersion = $1;
	}  # end if
}  # end while
close(VERS);

### create our /apache/docs location.
unless ($docsLocation) {
	if ($workingUserName eq "root") {
		$docsLocation = "/Library/WebServer/Documents";
	} else {
		$homeDir = &expandTwiddle($workingUserName);
		$docsLocation = "$homeDir/Sites";
	}  # end if else
}  # end unless

### create our symlinks
if ($osxMajorVersion == 5) {  # if Leopard
	print "... it seems that we're running Leopard\n" if $verbose;
	$confLocation = "/etc/apache2";
	$logsLocation = "/var/log/apache2";
} else {  # otherwise I'm just using apache 1.3 values.
	print "... it seems that we're not running Leopard\n" if $verbose;
	$confLocation = "/etc/httpd";
	$logsLocation = "/var/log/httpd";
}  # end if else

### doing the work
unless (-e "/apache") {
	print "... creating dir /apache\n" if $verbose;
	system "mkdir /apache";
}  # end unless
print "... creating symlinks\n" if $verbose;
system "rm /apache/conf" if (-l "/apache/conf");  # removing symlink if it exists
system "ln -s $confLocation /apache/conf";
system "rm /apache/docs" if (-l "/apache/docs");  # removing symlink if it exists
system "ln -s $docsLocation /apache/docs";
system "rm /apache/logs" if (-l "/apache/logs");  # removing symlink if it exists
system "ln -s $logsLocation /apache/logs";

### now to edit the conf file...
print "Trying to open and edit our /apache/conf/httpd.conf...\n";
open(CONF, ">> /apache/conf/httpd.conf");

print CONF "\n\n### the below text has been added through an apache setup script.\n";

print "Changing DocumentRoot to /apache/docs/...\n" if ($verbose);
print "Adding configuration for /apache/docs/...\n" if ($verbose);

print CONF <<ENDPRINTING;
DocumentRoot "/apache/docs"
<Directory "/apache/docs/">
        Options Indexes MultiViews ExecCGI FollowSymlinks
        AddHandler cgi-script cgi pl
        AllowOverride None
        Order allow,deny
        Allow from all
</Directory>
ENDPRINTING

close(CONF);

print "Now attempting to restart apache...\n";
system("apachectl restart");

print "All done!\n\n";

exit(0);


sub printUsage {
	print "\n";
	print "Usage: $0 [--help] [--verbose] [--user username] [--docs \"path\"]\n";
	print "\tCreates /apache and sets up symlinks within to various locations for docs, logs, and conf.\n";
	print "\n";
	print "\t--verbose increase verbosity\n";
	print "\t--help print usage and quit\n";
	print "\n";
	print "\tusername is optional.  If excluded, it sets up for the current username.\n";
	print "\tThis is used for /apache/docs, which points to /Users/username/Sites\n";
	print "\tYou can also run as sudo to create /apache/docs as /Library/WebServer/Documents\n";
	print "\n";
	print "\ton Leopard:\n";
	print "\t/apache/conf -> /etc/apache2\n";
	print "\t/apache/docs -> " . &expandTwiddle($workingUserName) . "/Sites\n";
	print "\t\tor specify --docs \"path\" to use that instead\n";
	print "\t/apache/logs -> /var/log/apache2\n";
	print "\n";
	print "\ton Tiger:\n";
	print "\t/apache/conf -> /etc/httpd\n";
	print "\t/apache/docs -> " . &expandTwiddle($workingUserName) . "/Sites\n";
	print "\t\tor specify --docs \"path\" to use that instead\n";
	print "\t/apache/logs -> /var/log/httpd\n";
	print "\n";
}  # end usage


### this sub should expand ~ in the shell to become the user's home path
sub expandTwiddle() {
	my ($workingUserName) = @_;
   chomp(my $home = `echo ~$workingUserName`);
   return $home;
}  # end sub
