@rem = '--*-PERL-*--';
@rem = '
@echo off
perl.exe -x -S "%0" %*
goto endofperl
@rem = ';
#!/bin/perl -w
# A perl script to build A particular CSPI world that
# has been mirrored from San Jose.
# 
# Assumes 'setworld.bat' has already been called to 
# set all required environment variables
#

sub print2 # print to a log file, an error file, and stdout
{
  my($logfil)  = $_[0];
  my($errfil)  = $_[1];
  my($line)    = $_[2];
  # filter the output going to err/stdout, removing echos of command lines etc.
  if(!( ($line =~ "[ \t]+(bscmake|cl|nmake|link|perl|echo|genmake|del|copy|mkdir|rmdir|ntrename|rename|lib|C:\\|move)"|type) ||
        ($line =~ ".* : warning [CLD]")                 ||
        ($line =~ /^[ \t]*Creating/)                    ||
        ($line =~ "^Elapsed")                           ||
        ($line =~ 'file\(s\) copied\.')                 ||
        ($line =~ /^x:\\usr\\2ps\\int\\/)               ||
        ($line =~ /^x:\\disk1\\worlds\\/)               ||
        ($line =~ /^y:\\projects\\/)                    ||
        ($line =~ /^will rename/)                       ||
        ($line =~ /^Warning: expanding/)                ||
        ($line =~ /^\([xXyY]:\//)                       ||
        ($line =~ /^\(.*\)$/)                           ||
        ($line =~ /^Could Not Find /)                   ||
        ($line =~ /Microsoft/)                          ||
        ($line =~ /^Loaded/)                            ||
        ($line =~ /^Local/)                             ||
        ($line =~ /^Global/)                            ||
        ($line =~ /^level/)                             ||
        ($line =~ /^Shared/)                            ||
        ($line =~ /dict used/)                          ||
        ($line =~ /^Saving/)                            ||
        ($line =~ /^Exiting PS$/)                       ||
        ($line =~ /^Command line warning/)              ||
        ($line =~ /lib \/Out:/)                         ||
        ($line =~ /^\.\\.*\.obj /)                      ||
        ($line =~ /^cpsi\.lib $/)                       ||
        ($line =~ /^The system cannot find the file specified\.$/) ||
        ($line =~ /^C:\\apps\\msdev\\vc\\lib\\binmode\.obj /)  ||
        ($line =~ /^Warning: Macro variable/)           ||
        ($line =~ "^[^\.]*\.c\$")                        ))
  {
    print STDOUT $line;
    print $errfil $line;
  }
  print $logfil $line;
}

sub system2 # make a system call, printing the results to stdout and a log file
{
  my($logfil)  = $_[0];
  my($errfil)  = $_[1];
  my($cmd)     = $_[2];

  &print2($logfil,$errfil,"running cmd - $cmd\n");
  open(SYSTEM,"$cmd 2>&1 |");
  while(<SYSTEM>)
  {
    &print2($logfil,$errfil,$_);
  }
  close(SYSTEM);
}

sub is_winnt_config()
{
  my($makefile) = $_[0];
  if(!open(MAKEFILE,$makefile))
  {
    return 0;
  }
  while(<MAKEFILE>)
  {
    if(m/^\s*DEVELOP_ENV\s*= \s*(.*)$/o)
    {
      return (($1 eq "win32vc4") || ($1 eq "win32vc5"));
    }
  }
  return 0;
}

# make a list of all packages of interest
sub allpackages() # takes two args, name of world and pointer to list
{
  $w = "$_[0]/packages";
  local(*dirs) = $_[1];

  opendir(DIRH,$w);
  @pkgs = grep(!/^\.\.?$/,readdir(DIRH));
  closedir(DIRH);
  foreach $pkg (@pkgs)
  {
    if(-d "$w/$pkg")
    {
      opendir(DIRH,"$w/$pkg");
      @ix = grep(/^ix86win32/,readdir(DIRH));
      closedir(DIRH);
      foreach $ix (@ix)
      {
        if(&is_winnt_config("$w/$pkg/$ix/DEVELOP/makefile"))
        {
          push(@dirs,"$w/$pkg/$ix/DEVELOP");
        }
        if(&is_winnt_config("$w/$pkg/$ix/EXPORT/makefile"))
        {
          push(@dirs,"$w/$pkg/$ix/EXPORT");
        }
      }
    }
  }
}

# make a list of all products of interest
sub allproducts()
{
  $w           = $_[0];
  local(*dirs) = $_[1];

  foreach $prod ('buildvm', 'standardVM', 'installvm', 'cpsi', 'supranorm',
    'webprint', 'trapping', 'jtppi', 'jtppidemo', 'buildvm', 'supranorm', 
    'supratools', 'suprarip3', 'suprarip3old', 'suprarip3010', 'supraassem', 
    'supra', 'supratestbed', 'pdf')
  {
     $d = "$w/products/adobe/$prod";
     if(-d $d)
     {
       if($prod eq 'standardVM')
       {
         push(@dirs,"$d/leVM");
       }
       else
       {
         opendir(DIRH,$d);
         @prods = grep(!/^\.\.?$/,readdir(DIRH));
         closedir(DIRH);
         foreach $p (@prods)
         {
           $try = "$d/$p";
           if((-d $try) && ($try =~ /.*32.*/) && 
             &is_winnt_config("$try/makefile"))
           {
             push(@dirs,"$try");
           }
         }
       }
     }
  }
}

sub mkworld # make a specified world, takes two arguments
{
  my($worldname)  = $_[0]; # name of the world
  my($tobuild)    = $_[1]; # build both packages & products ?

  # open two files to record build file info
  # one for all the o/p, and one just for the summary stuff
  open(LOGFILE,"> cpsibuild.log");
  open(ERRFILE,"> cpsibuild.err");
  select((select(LOGFILE),$|=1)[0]); # make LOGFILE autoflush
  select((select(ERRFILE),$|=1)[0]); # make ERRFILE autoflush

  &print2(LOGFILE,ERRFILE," Building $worldname $bothflag\n");
  &system2(LOGFILE,ERRFILE,"setworld");
  if($tobuild eq "products")
  {
      &print2(LOGFILE,ERRFILE,"Package build supressed for $worldname\n");
  }
  else
  {
      &print2(LOGFILE,ERRFILE,"Building all packages for $worldname\n");

      &allpackages("$worldname",*alldirs);
      foreach(@alldirs)
      {
        $dir = $_;
        $subdir = $dir; $subdir =~ s!.*$worldname\/packages\/!!;
        &print2(LOGFILE,ERRFILE," Building in packagedir $subdir\n");
        chdir($dir);
        if(!-e 'native.mak')
        {
          &print2(LOGFILE,ERRFILE,"Cannot build in $dir: no native.mak\n");
        }
        else
        {
          &system2(LOGFILE,ERRFILE,"nmake /nologo /f native.mak package");
        }
      }
  }
  if($tobuild eq "packages")
  {
      &print2(LOGFILE,ERRFILE,"Product build supressed for $worldname\n");
  }
  else
  {
    &print2(LOGFILE,ERRFILE,"Building all products for $worldname\n");
    @alldirs = ();
    &allproducts("$worldname",*alldirs);
    foreach(@alldirs)
    {
      $dir = $_;
      $subdir = $dir; $subdir =~ s!.*$worldname\/products\/!!;
      &print2(LOGFILE,ERRFILE," Building in productdir $subdir\n");
      chdir($dir);
      if(!-e 'native.mak')
      {
        &print2(LOGFILE,ERRFILE,"Cannot build in $dir: no native.mak\n");
      }
      else
      {
        &system2(LOGFILE,ERRFILE,"nmake /nologo /f native.mak target");
      }
    }
  }
  &print2(LOGFILE,ERRFILE,"Building of products/packages complete\n");
  close(LOGFILE);
  close(ERRFILE);
}

&mkworld("w:/worlds/3011.105","packages");

__END__
:endofperl
