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


: =====================================
: The Perl Script
: =====================================
@rem ';
#!/usr/bin/perl -w
#line 24

# Modified By     Reason
# 12.11.00 rmills Added ComponentData to the protected names
# 09.07.00 rmills Run this from the ripN\source directory.  Check environment first.
#                 Check for ripN (not rip2).  Don't move component_i.c to public directories.
#                 Check that directory source\Component exists (the template directory)
#                 Look for guidnew.exe in same directory as CPRstamper.bat

die "Syntax:  CPRstamper Objectname Interface\n" 
  . "Example: CPRstamer  FrontEnd   ICPRFrontEnd\n" if (scalar @ARGV) != 2 ;
my $Component   = shift       ;
my $Interface   = shift       ;


# -------------------------------
# Check that this is a sane thing to do, then copy the Component template directory
# -------------------------------
$this_filename = $0 ;
use Cwd;
my $dir = cwd;
use File::Basename  ;

#
# are we in ...\rip3\source ?
# is there a ..\public\source and ..\public\include directory?
# does the directory Component exist (the template)?
#
($name,$path) = fileparse($dir) ;
chop($path) ;
($name1) = fileparse($path) ;

my $ rip3        = "rip[\\d]" ;
my $ rip3_source = (( $name =~ /source/i ) && ( $name1 =~ /$rip3/i )) ;

die "you must be in $rip3\\source to use this script" if !$rip3_source             ;
$source  = "..\\public\\source"  ; die "I can't find $dir" if ( ! -d $source     ) ;
$include = "..\\public\\include" ; die "I can't find $dir" if ( ! -d $include    ) ;
die "I can't find directory the directory \"Component\""   if ( ! -d "Component" ) ;

#
# is there a directory called $Component?
# are there already files in the ..\public\source and ..\public\include directories?
#
my $Component_C = $Component . "_i.c"      ;
my $Component_H = $Component . ".h"        ;
my $Interface_C = $Interface . "_i.c"      ;
my $Interface_H = $Interface . ".h"        ;

$I_H = "..\\public\\include\\$Interface_H" ;
$C_H = "..\\public\\include\\$Component_H" ;
$I_C = "..\\public\\source\\$Interface_C"  ;
$C_C = "..\\public\\source\\$Component_C"  ;

die "directory $Component already exists\n" if -d $Component ;
die "file $I_H already exists\n" if -e $I_H                  ;
die "file $C_H already exists\n" if -e $C_H                  ;
die "file $I_C already exists\n" if -e $I_C                  ;
die "file $C_C already exists\n" if -e $C_C                  ;

#
# make the directory and copy in the files. Remove read-only attributes.
#
system("mkdir $Component")             ;
chdir  $Component                      ;
system("xcopy/s ..\\Component\\*.* .") ;
system("attrib/s -r *.*")              ;


# -------------------------------
# generate the GUIDs (sounds like something from Stonehenge)
# -------------------------------
my @guid_c      = &GetGuid ;
my @guid_i      = &GetGuid ;

my $c_guid      = $guid_c[17] ;
my $C_guid      = $guid_c[14] ;

my $i_guid      = $guid_i[17] ;
my $I_guid      = $guid_i[14] ;

print "------------------------------------------------\n" ;
print "Component GUID = $c_guid\n" ;
print "Interface GUID = $i_guid\n" ;
print "------------------------------------------------\n" ;

# -------------------------------
# Make a list of files to be processed
# -------------------------------
my %files1      = GetFiles("*.*") ;
my %testfiles   = GetFiles("test\\*.*") ;
my %files       = (%files1,%testfiles) ;

# -------------------------------
# go over the files and edit them
# -------------------------------
my $file    ;
foreach $file ( keys %files ) {
    my $oldname = $file ;
    my $newname = $file ;
    $newname    =~ s/Component/$Component/  ;
    $newname    =~ s/Interface/$Interface/  ;

    # print "copy $oldname $newname\n"    ; # print it
    # rename($oldname,$newname) ;
    EditFile($oldname,$newname) ;
} 

# -------------------------------
# move the files to their final resting place
# -------------------------------
unlink("..\\..\\public\\include\\$Interface_H") ;
unlink("..\\..\\public\\source\\$Interface_C") ;

system("move $Interface_H ..\\..\\public\\include") ;
system("move $Interface_C ..\\..\\public\\source") ;

# unlink("..\\..\\public\\include\\$Component_H") ;
# unlink("..\\..\\public\\source\\$Component_C") ;
# system("move $Component_H ..\\..\\public\\include") ;
# system("move $Component_C ..\\..\\public\\source") ;

print "Still to do:\n" ;
print " 1.  Make sure you have CPRLibrary.dll on your path\n" ;
print " 2.  Start MSDEV and Build $Component\n"               ;
print " 3.  Build and run test.exe\n"                         ;


# =====================================
# GetFiles - find files which match the input pattern
# =====================================
sub GetFiles
{
    my     %result               ;
    my     $pattern = shift      ;
    my @files = glob($pattern) ;

    foreach my $file (@files) {
      if ( -e $file && !-d $file ) {
        $result{$file} = $file ;
      }
    }

    $file = "resource.h" ;
    if ( -e $file && !-d $file ) {
      $result{$file} = $file ;
    }

    return %result ;
}

# =====================================
# EditFile - Edit files and change their name
# =====================================
sub EditFile
{
    my $oldname = shift         ;
    my $newname = shift         ;
    my $scratch = "scratch.tmp" ;

    my   $line                  ;
    open INPUT ,$oldname        ;
    open OUTPUT,"> $scratch"    ;

    while ( $line = <INPUT> ) {
      # hide some strings with words like Interface and Component
      $line    =~ s/QueryInterface/QUERYINTERFACE/g ;
      $line    =~ s/ComponentDef/COMPONENTDEF/g     ;
      $line    =~ s/ComponentData/COMPONENTDATA/g   ;
      $line    =~ s/InstanceDef/INSTANCEDEF/g       ;
      $line    =~ s/ComponentRef/COMPONENTREF/g     ;
      $line    =~ s/InterfaceDef/INTERFACEDEF/g     ;
      $line    =~ s/CPRInterface/CPRINTERFACE/g     ;

      # change Component and Interface to their true values
      $line    =~ s/Component/$Component/g          ;
      $line    =~ s/Interface/$Interface/g          ;

      # restore strings with words like Interface and Component
      $line    =~ s/INTERFACEDEF/InterfaceDef/g     ;
      $line    =~ s/COMPONENTREF/ComponentRef/g     ;
      $line    =~ s/COMPONENTDEF/ComponentDef/g     ;
      $line    =~ s/COMPONENTDATA/ComponentData/g   ;
      $line    =~ s/INSTANCEDEF/InstanceDef/g       ;
      $line    =~ s/QUERYINTERFACE/QueryInterface/g ;
      $line    =~ s/CPRINTERFACE/CPRInterface/g     ;
      
      # replace the GUID markers
      $line    =~ s/cccccccccc/$c_guid/g            ;
      $line    =~ s/CCCCCCCCCC/$C_guid/g            ;
      $line    =~ s/iiiiiiiiii/$i_guid/g            ;
      $line    =~ s/IIIIIIIIII/$I_guid/g            ;
      print OUTPUT $line ;
    }

    close INPUT  ;
    close OUTPUT ;

    unlink $oldname            ;
    rename ($scratch,$newname) ;
    unlink  $scratch           ;
}

# =====================================
# FindOnPath - find an executable 
#            - searches the directory in which this script runs
#            + as well as the current directory and PATH
# =====================================
sub FindOnPath
{
    my $program  = shift                          ;
    my ($thisname,$thispath) = fileparse($this_filename) ;

    # get the path.  Add the current directoy, and the directory from which this script runs
    my $path    = $thispath . ";" . lc cwd() . ";" . $ENV{"path"} ;
    my $pathext = ".exe;"                                         ; # $ENV{"pathext"}

    @dirs    = split(";",$path)              ;
    @exts    = split(";",$pathext)           ;

    # hunt the for the filename
    my $bFound    ;
    my $filename  ;

    foreach $dir (@dirs) {
      next if !length $dir ;
    
      # try every extension
      if ( !$bFound ) {
        foreach $ext (@exts) {
          $filename = $dir . $program . $ext ;
          if ( !$bFound && -e $filename && !-d $filename ) {
            # print $filename . "\n" ;
            $bFound ++ ;
          } 
        }
        # print $program . " not on path\n" if ! $bFound ;
      }
    }

    return $bFound ? $filename : $program ;
}

# =====================================
# GetGuid - returns an array of the output of guidnew.exe
# =====================================
sub GetGuid
{
    use POSIX qw(tmpnam) ;

    my $filename = tmpnam()              ;
    my $guidnew  = FindOnPath("guidnew") ;
    system("$guidnew > $filename") ;

    open FILE,$filename ;
    my @lines ;
    my $line  ;
    while ( $line = <FILE>) {
      chop($line) ;
      chop($line) ;
      push @lines,$line ;
    }
    close FILE ;
    unlink $filename ;

    return @lines ;
}

__END__
:endofperl
