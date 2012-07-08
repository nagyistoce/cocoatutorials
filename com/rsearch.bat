@rem = '--*-PERL-*--';
@rem = '
@echo off
rem setlocal
set LOC=-S %0.bat
if exist %0 set LOC=%0
if exist %0.bat set LOC=%0.bat
set ARGS=
:loop
if %1.==. goto endloop
set ARGS=%ARGS% %1
shift
goto loop
:endloop
perl.exe %LOC% %ARGS%
goto endofperl
@rem ';
#--------------------------------------------------------------
($cmd = $0) =~ s|.*\\||;
$debug = 0;
$quiet = 0;
$fileList = 0;
$usage = "Usage: $cmd [-quiet] [-fileList] <searchPattern> <filePattern>\n";
while($_ = shift) {
    if (/-d/) { $debug = 1; next;}
    if (/-q/) {$quiet = 1; next;}
    if (/-f/) {$fileList = 1; next;}
    push(@pat, $_);
}


($searchPattern, $filePattern) = @pat;
die $usage unless ($searchPattern);
$filePattern ||= '\.[cChH]';


$| = 1;
$CR = "\015";



if (! $quiet) {
   print "###### Searching for lines matching $searchPattern #####\n";
   print "###### in files matching $filePattern ####\n";
}


&Traverse(\&Process, ".");


sub Traverse
{
    my($func, $d) = @_;
    my($file, @files);


    if (-d $d) {
        if (opendir(DIR, $d)) {
            @files = readdir(DIR);
            closedir(DIR);
            foreach $file (@files) {
                &Traverse($func, "$d/$file") unless ($file =~ /^\.{1,2}$/);
            }
        }
        else { print "Could not read directory: $d\n";}
    }
    else {
        &$func($d);
    }
}


sub Process
{
    my($file) = $_[0];


    if ($file =~ /$filePattern/) {
        print STDERR "File: $file matches filePattern, /$filePattern/.\n" if $debug;
        @matches = ();
        unless(open(FILE, "<$file")) {
            warn "$cmd: Cannot open $file\n";
            return;
        }
        $lineno = 0;
        while(<FILE>) {
            $lineno++;
            if (/$searchPattern/) {
                if ($fileList) {
                   print "### $file ###\n";
                   last;
                }
                push @matches, sprintf("%4d: ", $lineno), $_;
                last if ($firstOnly);
            }
        }
        close(FILE);
        if (@matches) {
            print "<<< $file >>>";
            print "\n", @matches, "\n";
        }
    }
}



#--------------------------------------------------------------
__END__
:endofperl 
