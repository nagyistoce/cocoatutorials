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
my $target   = shift or &Syntax() ;
my $argument = shift or 1         ;

my $PrivateWorld = GetEnv("PRIVATE_WORLD","PRIVATE_WORLD not specified - use setworld") ;
my $bWritten     = 0 ;


#
# Open the batch file and figure out what the target is
#
open(FILE,">>$thisfile")      ;
FindDirOrFile($PrivateWorld,"packages"        ) ;
FindDirOrFile($PrivateWorld,"products","adobe") ;
close FILE ;

print "Cannot find target $target\n" unless $bWritten ;


sub Syntax
{
   my $PRIVATE_WORLD ;

   $PRIVATE_WORLD    = $ENV{"PRIVATE_WORLD"} or $PRIVATE_WORLD = "" ;
   die "Useage $0 <target>\ntarget=file or package or product\n" 
     . "Environment PRIVATE_WORLD = $PRIVATE_WORLD\n" 
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
    
    return $result              ;
}


# ------------------------------------
# IsFileName - name contain a dot?
# ------------------------------------
sub IsFileName { return  (shift) =~ /\./ ; }

# ------------------------------------
#  FindDirOrFile - is $target in this directory
# ------------------------------------
sub FindDirOrFile
{
    my $dir = MakePath(@_) ;

    FindFile($dir) if !$bWritten ;
}

# ------------------------------------
#  FindFile - is $target in this directory or sources
# ------------------------------------
sub FindFile
{
    my $directory = MakePath(@_) ;
    my @products  = GetDirectories($directory) ;
    foreach $product ( @products ) {

      # try findme filename.c (in sources)
      $dir  = MakePath($directory,$product,"sources") ;
      $file = MakePath($dir,$target) ;
      if ( -e $file ) {
        print        "copy $file\n" ;
        print FILE   "copy $file\n" ;
        $bWritten ++ ;
      }

      # try findme filename.h (not in sources)
      $dir  = MakePath($directory,$product) ;
      $file = MakePath($dir,$target) ;
      if ( -e $file ) {
        print        "copy $file\n" ;
        print FILE   "copy $file\n" ;
        $bWritten ++ ;
      }
    }
}

# ------------------------------------
# Dots - does argument start with .
# ------------------------------------
sub Dots { return  (shift) =~ /^\./ ; }


# ------------------------------------
# SetupOS - 
# ------------------------------------
sub SetupOS
{
    my $result = $^O =~ m:^mswin32:i ; # is OSNAME == mswin32 ?
    $PATH_SEP  =$result?  "\\" : "/" ; # PATH_SEP = \ on NT and / on UNIX
    return $result ;
}

# ------------------------------------
# MakePath - combine elements into a pathname
# ------------------------------------
sub MakePath
{
    SetupOS if !$PATH_SEP         ;
    my $next           ;
    my $result = shift ;
    while ( $next = shift ) {
      $result = $result . $PATH_SEP . $next ;
    }
    return $result ;
}

# ------------------------------------
# GetDirectories - get the directories in a directory
# ------------------------------------
sub GetDirectories
{
    my $path = MakePath(@_)       ;
    opendir DIRECTORY,$path       ;
    my @files = readdir DIRECTORY ;
    closedir DIRECTORY            ;

    my @result = ()               ;
    my $file                      ;
    foreach $file (@files ) {
      my $fullpath = MakePath($path,$file) ;
      if ( -d $fullpath && !Dots($file) ) {
        push @result, $file ;
      }
    }
    return @result ;
}

__END__
:endofperl
