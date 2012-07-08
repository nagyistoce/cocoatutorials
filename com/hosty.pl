#!/bin/perl -w

use Socket;

my $host = shift or die "hosty <address>\n" ;

if ( $host =~m/([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)/) {
  $address = inet_aton($host) ;
  $A = gethostbyaddr($address,AF_INET) ;
} else {
  $address = gethostbyname($host) ;
  $A       = inet_ntoa($address)  ;
}

print "$host = $A\n" ;

