@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S "%0" %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
call getme_helper.bat
goto endofperl
@rem ';


#!/bin/perl -w
#line 17


use Cwd      ;
use Net::FTP ;


# options
my $bFTP = 0 ; // set to FTP changed files (not recommended)


sub Syntax
{
   die "Useage $0 <target>\ntarget=file or package or product\n" 
     . "\n"
     ;
}

#
# find the bottom of the batch file and chop of the rest
#

# =====================================================

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
        push @result, $fullpath ;
        my @subdirs = GetDirectories($fullpath) ;
        foreach $subdir ( @subdirs ) {
          push @result,$subdir ; # MakePath($fullpath,$subdir) ;
        }
      }
    }
    return @result ;
}


sub Process
{
    my $filename = shift ;
    print $filename  ;
    open(FILE,$filename) ;
    @lines = <FILE> ;
    close(FILE) ;

    my $bChanged = 0 ;

    foreach $line ( @lines ) {
      my $old   = $line ;
      $line     =~ s|1996-2003|1996-2004| ;
      $bChanged += 1 if ( $line ne $old ) ;
    }


    if ( $bChanged ) {
      open(FILE,">$filename") ;
      foreach $line ( @lines ) {
        $line =~ s|1996-2003|1996-2004|  ;
        print FILE $line ;
      }
      close(FILE) ;
    }

    print $bChanged ? " changed\n" : " not changed\n" ;

    return $bChanged ;
}

# ------------------------------------
# MakeURL - combine elements into UNIX pathname
# ------------------------------------
sub MakeURL
{
    my $x = shift ;
    $x    =~ s|\\|/|g       ;
    if ( substr($x,0,1) eq "." ) {
      $x = substr($x,1) ;
    }

    return $x ;
}

print "Hello world\n" ;
$pattern =  "*.shtml" ;
@dirs    = GetDirectories(".") ;
push @dirs,"." ;

$count   = 0 ;
$changes = 0 ;



if ( $bFTP ) {
  $host     = "ftp://ftp.clanmills.com" ;
  $user     = "clanmills.com" ;
  $pass     = "largs1263" ;

  $ftp = Net::FTP->new($host)        or die "Can't connect: $@\n" ;
  $ftp->login($user,$pass)           or die "Can't login: $@\n"   ;
}

foreach $dir ( @dirs ) {

   print "cd " . $dir ."\n" ;

   my $current = getcwd ;
   chdir $dir ;
   my @files = glob($pattern) ;
   chdir $current ;

   foreach my $file ( @files ) {
     $count ++ ;
     my $F = MakePath($dir,$file) ;
     my $R = MakeURL($dir) ;

     if ( $bFTP ) {
       print "put " . $F 
         . " -> " . $R  . "/" . $file
         . "\n"
         ;
       $ftp->cwd($R) ;
       $ftp->delete($file) ;
       $ftp->put($F,$file) ;
     }

     $changes += 1 if Process($F)  ;
     # system("ce \"$F\"") ;
     
     if ( 0 ) {
       $count ++ ;
       if ( $count > 5000 ) {
         $ftp->quit() ;
         exit(0) ;
       }
     }
     # system("pause") ;
  }
}

if ( $bFTP ) {
  $ftp->quit() ;
}

print "processed $changes / $count files\n" ;




# put ( LOCAL_FILE [, REMOTE_FILE ] )

__END__
:endofperl
