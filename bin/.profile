##
# shell variables

export HOST=`(hostname|cut -d. -f 1)`
declare -x PS1='\! ${HOST} ${PWD} \$ '

# export PS1="\! $PWD $ "
# PS1='\! $PWD> '
#

##
# increase the number of open files
# 516 /Users/rmills/clanmills $ ulimit -a
# core file size          (blocks, -c) 0
# data seg size           (kbytes, -d) unlimited
# file size               (blocks, -f) unlimited
# max locked memory       (kbytes, -l) unlimited
# max memory size         (kbytes, -m) unlimited
# open files                      (-n) 256
# pipe size               (512 bytes, -p) 1
# stack size              (kbytes, -s) 8192
# cpu time                (seconds, -t) unlimited
# max user processes              (-u) 709
# virtual memory          (kbytes, -v) unlimited
# 517 /Users/rmills/clanmills $ 

ulimit -n 1024

##
# environment strings
# export DYLD_LIBRARY_PATH=/opt/local/lib/
#

if [ -e /tmp/robin/bin ]; then
	BIN=/tmp/robin/bin
else
	BIN=${HOME}/bin
fi
export PATH=".:${BIN}:/usr/local/cbl/bin:/usr/local/bin:/usr/X11R6/bin:/Developer/Tools:${PATH}:$MAGICK_HOME/bin:/System/Library/Frameworks/Python.framework/Versions/2.7/bin/:/Developer/usr/bin:/usr/libexec:"
export MANPATH="/opt/local/share/man:/usr/share/man:/usr/share/man/man1:/usr/share/man/man2:/usr/share/man/man3:/usr/local/man:/usr/local/share/man/:/usr/X11R6/man:/opt/subversion/man"
export DISPLAY=:0.0
export CLASSPATH=".:${HOME}/classpath:${HOME}/classpath/Multivalent20060102.jar:${HOME}/classpath/DVI20060102.jar"
export DYLD_LIBRARY_PATH=/Users/rmills/boost_1_44_0/stage/lib:/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages
  
export P4USER=rmills
export P4PORT=home:1666
export P4CLIENT=imac

export PINGS="bocksbeutel tenth demi housecat"

# used by cpan
export "FTP_PASSIVE=1"
  
# export PYTHONPATH=~/scripts/:${HOME}/bin/:.:/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages
# export PYTHONPATH=/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages

export VERSIONER_PERL_PREFER_32_BIT=no
export VERSIONER_PYTHON_PREFER_32_BIT=no
export PERLPATH=/Users/rmills/gnu/ImageMagick
export PYTHONPATH="/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages:$PYTHONPATH"  
export REMOTEHOST=clanmills.homedns.org
export GPGKEY=CF1649DA
export PHOTOS=/Users/rmills/Documents/Dropbox/Photos
export RUBYOPT=rubygems

# export ARCHFLAGS="-arch x86_64" # "-arch i386 -arch ppc"
# export CFLAGS="$ARCHFLAGS"      # "-arch i386 -arch ppc"
# export LDFLAGS="$ARCHFLAGS"     # "-arch i386 -arch ppc"
export L="-exec ls -dalt {} ;"
export X="-exec rm -rf {} ;"
export C="-exec $CE {} ;"
export I="-exec lipo -info {} ;"
export O="-exec otool -L {} ;"
export Z="-exec open {} ;"
export P="/Users/rmills/Documents/Dropbox/Photos/"
export Q="/Users/rmills/Documents/NotDropbox/Photos/"
export G="-exec grep"
export __="{} ;"

##


##
# Install image magick
# http://mirror1.smudge-it.co.uk/imagemagick/www/binary-releases.html#macosx
# export MAGICK_HOME="/Users/rmills/gnu/ImageMagick"
# export PATH="$MAGICK_HOME:$PATH"
# export DYLD_LIBRARY_PATH="$MAGICK_HOME/lib:$DYLD_LIBRARY_PATH"





##
# functions
dirXX() {
	# this if is here to remind me of the syntax !
	if [ -z "$22" ]; then 
	   echo usage: dir directory
	   return
	fi
	ls -alt "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9"
}

# alias dir="ls -dlt"

dir() { # it's impossible to like bash - the syntax is pure shit
	list='ls -dlthp'
	if [ ${#*} -eq 0 ]; then
		$list *
	elif [ ${#*} -eq 1 ]; then
		if [ -d $1 ]; then
			$list $1/*
		else
			$list $*
		fi
	else
		$list $*
	fi
}

history() { 
	builtin history | tail -30
}

free() {
  df -m .
  # df -k . | tail -1 | cut -f4 -d ' '
}

dirs() {
	ls -ltp $1 | grep "^d"
}

links() {
	ls -altp $1 | grep "^l"
}

files() {
	ls -altp $1 | grep "^-"
}

socks() {
	ls -altp $1 | grep "^s"
}

diros() {
	ls -ltG $* | sort --key=5 --numeric-sort
#	ls -ltGp $* | tigersort +4 -n
}

diron() {
	ls -ltG $* | sort --key=9 --ignore-case
#	ls -ltGp $* | tigersort +8 -i
}

hidden() {
	ls -altp $1 | grep " \."
}


# set CE = editor of choice
if [ `uname` == 'Darwin' ]; then
	export CE=`which bbedit 2>/dev/null`
	if [ -z "$CE" ]; then
		export CE=`which edit 2>/dev/null`
	fi
else # probably linux, could be cygwin
	export CE=`which kate 2>/dev/null`
	if [ ! -z "$CE" ]; then
		export CE="kate --use"
	fi		
fi
# catch all - use good old vi!
if [ -z "$CE" ]; then
	export CE=vi
fi			

ce() {
	if [ $CE == "kate --use" ]; then
		kate --use "$@" 2>/dev/null >/dev/null &
	else
		$CE "$@"
	fi
}

#

##
# aliases
# dos like things
alias rename=mv
alias move=mv
alias del='sudo rm -rf'
alias dirod='ls -altGr'
alias xcopy='ditto'
alias finder='find . -depth -name'
alias shellx=open
alias start=open

# hieroglyphics
alias .='ls -alt'
alias ..='cd ..'
alias ...=sudo
alias ~='cd ~'

# favorite directories
alias photos="cd ~/Pictures/Photos/2008"
alias heather='cd ~/heather/httpdocs'
alias clanmills.com='ssh clanmil1@clanmills.com'
alias clanmills='cd ~/clanmills'
alias collages='cd ~/Pictures/Picasa/Collages'
alias tuts='cd ~/Projects/cocoatutorials'
alias QtSee='cd ~/Projects/cocoatutorials/QtSee'
alias robin='cd ~/Projects/cocoatutorials/robin'
#

##
# keep QtCreator happy
mkdir -p /tmp/qt-stuff-6474/source/qt-everywhere-opensource-src-4.7.0/lib

# That's all Folks!
##

##
# Your previous /Users/rmills/.profile file was backed up as /Users/rmills/.profile.macports-saved_2011-12-06_at_12:29:07
##

## rmills - I've put /opt/local/bin at the end of the path to prevent Macports from putting his commands in front of the system's
# MacPorts Installer addition on 2011-12-06_at_12:29:07: adding an appropriate PATH variable for use with MacPorts.
export PATH=$PATH:/opt/local/bin:/opt/local/sbin
# Finished adapting your PATH environment variable for use with MacPorts.

# That's all Folks
##

