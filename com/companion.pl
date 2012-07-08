use strict;
use Win32::OLE qw(in EVENTS);
use Win32::OLE::Variant;
use Time::HiRes qw(gettimeofday tv_interval);
use Data::Dumper;
use Tk;


# get current time;
my $t0=[gettimeofday];
my (@stack,@dstack,@istack,%widgets,$currentDoc,$ie,
    $totalTime,$userTime,$pageSize,$imageSize);


# new MainWindow
$widgets{main}=MainWindow->new;
$widgets{main}->configure(-title => 'Internet Explorer Browser Companion');


# top three labels
$widgets{lurl}  = $widgets{main}->Label(-text => 'URL:', -anchor => 'w');
$widgets{url}   = $widgets{main}->Label(-text => '[none]', -relief => 'sunken', -anchor => 'w');


$widgets{ltime} = $widgets{main}->Label(-text => 'Page Time:', -anchor => 'w');
$widgets{time}  = $widgets{main}->Label(-text => 'na', -relief => 'sunken', -anchor => 'w');


$widgets{lttime} = $widgets{main}->Label(-text => 'Total Page Time:', -anchor => 'w');
$widgets{ttime}  = $widgets{main}->Label(-text => 'na', -relief => 'sunken', -anchor => 'w');


$widgets{lusertime} = $widgets{main}->Label(-text => 'User Time:', -anchor => 'w');
$widgets{usertime}  = $widgets{main}->Label(-text => 'na', -relief => 'sunken', -anchor => 'w');


$widgets{lpagesize} = $widgets{main}->Label(-text => 'Page Size:', -anchor => 'w');
$widgets{pagesize}  = $widgets{main}->Label(-text => 'na', -relief => 'sunken', -anchor => 'w');


$widgets{ltotpagesize} = $widgets{main}->Label(-text => 'Total Page Size:', -anchor => 'w');
$widgets{totpagesize}  = $widgets{main}->Label(-text => 'na', -relief => 'sunken', -anchor => 'w');


$widgets{lisize} = $widgets{main}->Label(-text => 'Images Size:', -anchor => 'w');
$widgets{isize}  = $widgets{main}->Label(-text => 'na', -relief => 'sunken', -anchor => 'w');


$widgets{ltotisize} = $widgets{main}->Label(-text => 'Total Images Size:', -anchor => 'w');
$widgets{totisize}  = $widgets{main}->Label(-text => 'na', -relief => 'sunken', -anchor => 'w');


$widgets{path}  = $widgets{main}->Label(-text => "Item: [none]", -anchor => 'w');
$widgets{lcookies} = $widgets{main}->Label(-text => 'Cookies:', -anchor => 'w');


# two listboxes
$widgets{items}  =$widgets{main}->Scrolled('Listbox');
$widgets{cookies}=$widgets{main}->Scrolled('Listbox');


# scrolled Text for values
$widgets{value}=$widgets{main}->Scrolled('Text');
$widgets{value}->configure(-height=>8);
$widgets{history}=$widgets{main}->Scrolled('Text');
$widgets{history}->configure(-height=>8,-fg=>'white',-bg => 'darkblue');


# exit button
$widgets{bExit}=$widgets{main}->Button(-text => "Exit", -command => \&onExit);
$widgets{bResetTime}=$widgets{main}->Button(-text => "Reset Time", -command => \&onResetTime);
$widgets{bResetBytes}=$widgets{main}->Button(-text => "Reset Bytes", -command => \&onResetBytes);
$widgets{bResetHistory}=$widgets{main}->Button(-text => "Clear History", -command => sub { $widgets{history}->delete('1.0','end'); });


# layout widgets using Tix->frame
$widgets{lurl}->form(-top => '%0', -left =>'%0', -right=>'%10');
$widgets{url}->form(-top => '%0', -left =>$widgets{lurl}, -right=>'%100');


$widgets{ltime}->form(-top => $widgets{lurl}, -left =>'%0', -right=>'%16');
$widgets{time}->form(-top => $widgets{lurl}, -left =>$widgets{ltime}, -right=>'%33');


$widgets{lttime}->form(-top => $widgets{lurl}, -left => $widgets{time}, -right =>'%50');
$widgets{ttime}->form(-top => $widgets{lurl}, -left => $widgets{lttime}, -right=> '%66');


$widgets{lusertime}->form(-top => $widgets{lurl}, -left => $widgets{ttime}, -right =>'%83');
$widgets{usertime}->form(-top => $widgets{lurl}, -left => $widgets{lusertime}, -right=> '%100');


$widgets{lpagesize}->form(-top => $widgets{ltime}, -left =>'%0', -right=>'%16');
$widgets{pagesize}->form(-top => $widgets{ltime}, -left =>$widgets{lpagesize}, -right=>'%33');


$widgets{ltotpagesize}->form(-top => $widgets{ltime}, -left =>$widgets{pagesize}, -right=>'%50');
$widgets{totpagesize}->form(-top => $widgets{ltime}, -left =>$widgets{ltotpagesize}, -right=>'%66');


$widgets{lisize}->form(-top => $widgets{lpagesize}, -left =>'%0', -right=>'%16');
$widgets{isize}->form(-top => $widgets{lpagesize}, -left =>$widgets{lisize}, -right=>'%33');


$widgets{ltotisize}->form(-top => $widgets{lpagesize}, -left =>$widgets{isize}, -right=>'%50');
$widgets{totisize}->form(-top => $widgets{lpagesize}, -left =>$widgets{ltotisize}, -right=>'%66');


$widgets{path}->form(-top => $widgets{lisize}, -left => '%0',-right=>'%50');
$widgets{lcookies}->form(-top => $widgets{lisize}, -left => $widgets{path},-right=>'%100');


$widgets{items}->form(-top => $widgets{path}, -left => '%0', -right=>'%50');
$widgets{cookies}->form(-top => $widgets{path}, -left => $widgets{items}, -right=>'%100');
$widgets{value}->form(-top => $widgets{items},-left => '%0',-right=>'%100');
$widgets{history}->form(-top => $widgets{value},-left => '%0',-right=>'%100');


$widgets{bResetTime}->form(-left => '%0', -right =>'%25', -top =>$widgets{history}, -bottom => '%100');
$widgets{bResetBytes}->form(-left => $widgets{bResetTime},-right => '%50', -top =>$widgets{history}, -bottom => '%100');
$widgets{bResetHistory}->form(-left => $widgets{bResetBytes},-right => '%75', -top =>$widgets{history}, -bottom => '%100');
$widgets{bExit}->form(-left => $widgets{bResetHistory}, -right =>'%100',-top =>$widgets{history}, -bottom => '%100');


# start up ie
$ie = Win32::OLE->new('InternetExplorer.Application');


# attach events
if(1) {
  Win32::OLE->WithEvents($ie, __PACKAGE__, 'DWebBrowserEvents');
  Win32::OLE->WithEvents($ie, __PACKAGE__, 'DWebBrowserEvents2');
}else{
  Win32::OLE->WithEvents($ie, \&Event, 'DWebBrowserEvents');
  Win32::OLE->WithEvents($ie, \&Event, 'DWebBrowserEvents2');
}


# make sure is is visible
$ie->{Visible} = 1;


#enter main loop
MainLoop;


# -----------------------------------------------------------------


sub onBrowse {
  if(defined($currentDoc)) {
    undef(@stack);push(@stack,"/");
    undef(@dstack);push(@dstack,$currentDoc);
    undef(@istack);push(@istack,1);
    $widgets{path}->configure(-text => "item: /");
    populate();
    cookiePopulate();
  }
}


sub onResetTime {
  undef($totalTime);
  undef($userTime);
  $widgets{ttime}->configure(-text => 'na');
  $widgets{usertime}->configure(-text => 'na');
}


sub onResetBytes {
  undef($pageSize);undef($imageSize);
  $widgets{totpagesize}->configure(-text => 'na');
  $widgets{totisize}->configure(-text => 'na');
}


sub onExit {
  $ie->Quit();
  $widgets{main}->destroy();
  Win32::OLE->SpinMessageLoop();
  Win32::Sleep(500);
}



# - Cookies ------------------------------------------------
sub cookiePopulate {
  $widgets{cookies}->delete(0,$widgets{cookies}->size());
  foreach (split(/;\s*/,$currentDoc->{cookie})) {
    $widgets{cookies}->insert('end',$_);
  }
  $widgets{cookies}->bind('<1>' => sub {cookieSelected($widgets{cookies}->get( $widgets{cookies}->nearest($Tk::event->y) ))});
}


sub cookieSelected {
  my($item)=@_;
  setValue($item);
}


# - Items --------------------------------------------------
sub populate {
  my($item)=$stack[$#stack];
  # trim to 40 chars
  if(length($item)>40) {
    $item="...".substr($item,-40,40);
  }
  $widgets{path}->configure(-text => "item: ".$item);
  $widgets{items}->delete(0,$widgets{items}->size());
  $widgets{items}->insert('end',"..") unless ($stack[$#stack] eq "/");
  foreach(sort { uc($a) cmp uc($b)} keys %{$dstack[$#dstack]}) {
    next if (/^all$/); # skip all element
    $widgets{items}->insert('end',$_);
  }
  my $p=$dstack[$#dstack];
  if(defined($p->{length}) && $p->{length} && UNIVERSAL::isa($p,'Win32::OLE')){
    my (@coll,$i);
    @coll=in $p;
    foreach (@coll) {
      $widgets{items}->insert('end',"[".(++$i)."]");
    }
  }
  $widgets{items}->bind('<1>' => sub {selected($widgets{items}->get( $widgets{items}->nearest($Tk::event->y) ),$widgets{items}->nearest($Tk::event->y))});
}


sub selected {
  my($item,$index)=@_;
  my($top)=$dstack[$#dstack];
  if($item eq "..") {
    my($scrollTo);
    setValue("");
    pop(@stack);pop(@dstack);
    populate();
    $widgets{items}->see(pop(@istack));
  } elsif($item =~ /^\[(\d+)\]$/) {
    my $idx=$1-1;
    setValue("");
    push(@istack,$index);
    push(@stack,$stack[$#stack].$item.qq{/});
    my @coll = in $top;
    push(@dstack,$coll[$idx]);
    populate();
  } elsif(ref($top->{$item}) eq "Win32::OLE" || ref($top->{$item}) eq "HASH") {
    #print "Object Type: ",Win32::OLE->QueryObjectType($top->{$item}),"\n"
    #  if(UNIVERSAL::isa($top->{$item},'Win32::OLE'));
    setValue("");
    push(@istack,$index);
    push(@stack,$stack[$#stack].$item.qq{/});
    push(@dstack,$top->{$item});
    populate();
  } else {
    print "Object Type: ",Win32::OLE->QueryObjectType($top->{$item}),"\n"
      if(UNIVERSAL::isa($top->{$item},'Win32::OLE'));
    setValue($top->{$item});
  }
}


# - Value --------------------------------------------------
sub setValue {
  my($str)=@_;
  $widgets{value}->delete('1.0','end');
  $widgets{value}->insert('end',$str);
}


# - Utility functions --------------------------------------
sub timestamp {
  return tv_interval($t0);
}


# - Events -------------------------------------------------
sub BeforeNavigate2 {
  my($obj,@args)=@_;
  my($notice)=qq{\n[}.scalar(localtime(time)).qq{] }.qq{request:\n};
  if(defined($args[4])) {
    my $val=$args[4]->Value();
    if(length($val)) {
      $notice .= "  POST $args[1]\n";
      $notice .= "  data: ".$val."\n";
    } else {
      $notice .= "  GET $args[1]\n";
      }
  } else {
      $notice .= "  GET $args[1]\n";
  }
  if(defined($args[5])) {
    my $val=$args[5]->Value();
    if(length($val)) {
      $notice .= "  Headers: ".$val."\n";
    }
  }
  if(defined($args[2])) {
    my $val=$args[2]->Value();
    if(length($val)) {
      $notice .= "  Flags: ".$val."\n";
    }
  }
  if(defined($args[3])) {
    my $val=$args[3]->Value();
    if(length($val)) {
      $notice .= "  Target: ".$val."\n";
    }
  }
  $widgets{history}->insert('end',$notice);
  $widgets{history}->see('end');


  $t0=[gettimeofday];
  $userTime=[gettimeofday] unless(defined($userTime));
  undef($currentDoc);
}


sub DocumentComplete {
  my($obj,@args)=@_;
  my $stop=timestamp();


  $widgets{time}->configure(-text => sprintf "%7.4f",$stop);


  $totalTime += $stop;
  $widgets{ttime}->configure(-text => sprintf "%7.4f",$totalTime);
  $widgets{usertime}->configure(-text => sprintf "%7.4f",tv_interval($userTime));


  my $url=$args[1]->Value();
  if(length($url)>80) {
    $url=substr($url,0,80)."...";
  }
  $widgets{url}->configure(-text => $url);
  $currentDoc = $ie->{Document};


  if(defined($currentDoc) && defined($currentDoc->{fileSize})) {
    $pageSize += $currentDoc->{fileSize};
    $widgets{pagesize}->configure(-text=> $currentDoc->{fileSize}." bytes");
    $widgets{totpagesize}->configure(-text=> sprintf("%8.2f KB",$pageSize/1024.0));
  } else {
    $widgets{pagesize}->configure(-text=> "na");
  }


  if(defined($currentDoc) && defined($currentDoc->{images})) {
    my (@coll,$sz,%seen);
    @coll=in $currentDoc->{images};
    foreach my $img (@coll) {
      next if($seen{$img->{src}}++);
      $sz += $img->{fileSize};
    }
    $imageSize+=$sz;
    $widgets{isize}->configure(-text=> $sz." bytes");
    $widgets{totisize}->configure(-text=> sprintf("%8.2f KB",$imageSize/1024.0));
  } else {
    $widgets{isize}->configure(-text=> "na");
  }
  onBrowse();
}



sub Event {
  my ($obj,$event,@args) = @_;
  print "\n$event(";
  my($i);
  for my $arg (@args) {
    my $value = $arg;
    $value = sprintf "[%s %s]",
      Win32::OLE->QueryObjectType($value)
          if UNIVERSAL::isa($value, 'Win32::OLE');
    print ", " if($i++);
    print "$value";
  }
  print ")\n";
  if($event eq "OnQuit") {
    Win32::OLE->QuitMessageLoop();
  }
}

