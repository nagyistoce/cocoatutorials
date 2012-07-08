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
#!/usr/bin/perl -w
#line 14

use Win32::File ;

&SetupOS() ;

#
#  Get our arguments
#
@args = scalar @ARGV ?  @ARGV :(".") ;
my $total = 0 ;
foreach $arg ( @args ) {
  $total += DirDump($arg) ;
}
print "$total objects reset\n" if $total != 0 ;


# -------------------------------------
# Println - print arguments with space and \n
# -------------------------------------
sub Println
{
  my      $arg     ;
  my      $i = $_  ;
  foreach $arg ( @_ ) {
    $i -- ;
    print $arg . ( $i ? " " : "\n" ) ;
  }
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
# Unreadonly
# -------------------------------------
sub Unreadonly
{
    my $path   = shift ;
    my $result = 0     ;
    if ( -e $path ) {
      use Win32::File  ;
      my $attributes = 0              ;
      Win32::File::GetAttributes($path,$attributes) ;
      my $readonly  = $attributes & READONLY  ;

      if ( $readonly ) {
        $result ++              ;
        $attributes -= READONLY ;
        Win32::File::SetAttributes($path,$attributes) ;
      }
    }
    return $result ;
}

# -------------------------------------
# MakePath
# -------------------------------------
sub MakePath
{
    return $_[0] . $PATH_SEP . $_[1] ;
}

# -------------------------------------
# DirDump
# -------------------------------------
sub DirDump {

  my $dir    = $_[0] ;
  my $result = Unreadonly($dir)  ;

  opendir     DIRECTORY, $dir ;
  my @files = readdir DIRECTORY       ;
              closedir DIRECTORY      ;


  #
  # Report every file
  #
  foreach $file (@files) {
    $file = lc $file ;
    next if ( $file eq '.' || $file eq '..' ) ;

    my $path = MakePath($dir,$file) ;
    $result += Unreadonly($path)      ;
  }

  #
  # Recurse every directory
  #
  foreach $file (@files) {
    next if ( $file eq '.' || $file eq '..' ) ;

    my $path = MakePath($dir,$file) ;
    if ( -d $path ) {
      $result += DirDump($path) ;
    }
  }

  return $result ;
}

__END__
:endofperl
