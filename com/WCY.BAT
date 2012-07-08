@rem = '--*-Perl-*--
@echo off

: -------------------------------
: Execute Perl on both log files
: -------------------------------
:perly
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %*
goto endofperl
:WinNT
perl -x -S "%0" %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
goto endofperl


: -------------------------------
: The Perl Script
: -------------------------------
@rem ';
#!/usr/bin/perl -w
#line 24

die "wcy pattern ...\n" if (scalar @ARGV) <= 0 ;

my %files      = GetFiles(@ARGV) ; # keep these files
my $file        ;
my $total       ;
foreach $file ( sort keys %files ) 
{
    my $count = 0 ;
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
           $atime,$mtime,$ctime,$blksize,$blocks)
               = stat($file);

    open FILE,$file ;
    while ( <FILE> ) { $count ++ } ;
    close FILE      ;
    printf "%6d %6d %s\n",$count,$size,$file ;
    $total += $count ;
}
print "total = $total\n" if $total ;

#
# GetFiles - find files which match the input pattern
#
sub GetFiles
{
    my      %result       ;
    my      $pattern      ;
    foreach $pattern ( @_ ) {
      my $file  = $pattern ;
      #
      # if filename start @something, read the file
      #
      if ( $file =~ m/^\@/ ) {
        $file =~ s/^\@//   ;         # rip off the @
        if ( -e $file && !-d $file ) {
          print "open file $file\n" ;
          open FILE,$file ;
          my        $line ;
          while ( ($line=<FILE>) ) {
            chomp($line)    ;
            if ( $line !~ m/^[\@\:\#]/ ) { # ignore lines which begin @ or # or :
              $line =~ s/^\s//g ;    # remove leading space
              $line =~ s/\s$//g ;    # remove training space
              $result{$line} = $line ;
              # print "$line\n" ;
            }
          }
          close FILE ;
        }
      }
      #
      else # use glob to match the filenames
      #
      {
        my @files = glob($pattern) ;
        foreach $file (@files) {
          if ( -e $file && !-d $file ) {
            $result{$file} = $file ;
          }
        }
      }
    }
    return %result ;
}

__END__
:endofperl
