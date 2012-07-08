#!/usr/bin/perl

use Win32 ;
use Win32::File ;

my $file    = shift or die "syntax: fileSummary.pl filename";
my $returns = 0;

my $Prop      = Win32::File::Summary->new($file);
my $iscorOS   = $Prop->IsWin2000OrNT();
print "This OS is the correct one\n";
my $isStgfile = $Prop->IsStgFile();

print "The file contains a storage object.\n";
my $result = $Prop->Read();
if(ref($result) eq "SCALAR")
{
      my $err = $Prop->GetError();
      print "The Error: " . $$err  . "\n";
      $returns = 1;
}


my %hash = %{ $result };
foreach my $key (keys %hash)
{
      print "$key=" . $hash{$key} . "\n";
}

exit($returns)

  