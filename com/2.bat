@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S "%0" %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
goto endofperl
@rem ';
#!/bin/perl -w
#line 14

my $thisfile = TruncateThis() ;
#
# find the arguments
#
my $target   = shift or Syntax() ;
my $argument = shift or 1        ;

SetupOS()        ;
my $CD           = "cd /d" ; # GetEnv("CD"           ,"CD not specified in environment") ;


#
# Open the batch file and figure out what the target is
#
my  $bWritten  = 0       ;
open(FILE,">>$thisfile") ;
DirSearch("."          ) ;
close FILE               ;

print "Cannot find target $target\n" unless $bWritten ;

sub Syntax
{
   my $CD            ;
   $CD               = $ENV{"CD"}            or $CD            = "" ;
   
   die "Useage $0 <target>\ntarget=file or package or product\n" 
     . "Environment CD            = $CD\n"
     ;
}


# =====================================================

# ------------------------------------
# TruncateThis - find the bottom of the batch file and chop of the rest
# ------------------------------------
sub TruncateThis
{
    open THIS,"+<$0" ;
    my $line  = ""   ;
    my $count        ;
    while ( $line ne ":endofperl" && ($line = <THIS>) ) {
      chop($line)    ;
      $count ++      ;
    }
    my $now = tell THIS ;
    truncate THIS,$now  ;
    close    THIS ;
    return $0 ;
}

# -------------------------------------
# SetupOS - 
# -------------------------------------
sub SetupOS
{
    my $result = $^O =~ m:^mswin32:i ; # is OSNAME == mswin32 ?
    $PATH_SEP  =$result?  "\\" : "/" ; # PATH_SEP = \ on NT and / on UNIX
    return $result ;
}
# -------------------------------------
# MakePath
# -------------------------------------
sub MakePath
{
    return $_[0] . $PATH_SEP . $_[1] ;
}

# ------------------------------------
# GetEnv - get something from the environment or die
# ------------------------------------
sub GetEnv
{
    my $var = shift             ; # Environment variable name
    my $msg = shift             ; # death message
    my $result = $ENV{$var}     ;

    print "$msg\n" unless $result ;
    Syntax() unless $result       ;

    SetupOS()                   ;
    
    return $result              ;
}

# ------------------------------------
# IsFileName - name contain a dot?
# ------------------------------------
sub IsFileName { return  (shift) =~ /\./ ; }

# -------------------------------------
# DirSearch - search for $target in "shift"
# -------------------------------------
sub DirSearch {

  my $dir    = shift             ;

  opendir     DIRECTORY, $dir ;
  my @files = readdir DIRECTORY       ;
              closedir DIRECTORY      ;


  #
  # Hunt for the target
  #
  foreach $file (@files) {
    $file = lc $file ;
    next if ( $file eq '.' || $file eq '..' ) ;

    if ( (lc $file) eq (lc $target) ) {
      my $path = MakePath($dir,$file) ;

      if ( -d $path ) {
        print        "$CD $path\n"     ;
        print FILE   "$CD $path\n"     ;
        $bWritten ++ ;
      } elsif ( -e $path ) {
        print  "$path\n"  ;
        print        "found $path\n"   ;
        print FILE   "$CD $dir\n"      ;
        $bWritten ++ ;
      }
    }
  }

  #
  # Recurse every directory
  #
  foreach $file (@files) {
    next if ( $bWritten || $file eq '.' || $file eq '..' ) ;

    my $path = MakePath($dir,$file) ;
    if ( -d $path ) {
      DirSearch($path) ;
    }
  }
}

__END__
:endofperl
cd /d .\cim\bin64
