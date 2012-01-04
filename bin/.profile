##
# shell variables

export HOST=`(hostname|cut -d. -f 1)`
# If id command returns zero, you’ve root access.
if [ "$USER" == "root" ]; then                # $(id -u) -eq 0 ];
	# you are root, set yellow color prompt
	# http://news.softpedia.com/news/How-to-Customize-the-Shell-Prompt-40033.shtml
  	PS1="\\[\! $(tput setaf 3)\\]\\u@\\h:\\w #\\[$(tput sgr0)\\] "
fi

export PS1='\! $(tput setaf 3)\u@\h:\w$(tput sgr0) $ '
if [ "$USER" == "root" ]; then
#  	PS1="\\[\! $(tput setaf 2)\\]\\u@\\h:\\w $\\[$(tput sgr0)\\] "
  	PS1='\! $(tput setaf 2)\u@\h:\w$(tput sgr0) # '
fi
# http://wiki.bash-hackers.org/scripting/debuggingtips
export PS4='+(${BASH_SOURCE}:${LINENO}): ${FUNCNAME[0]:+${FUNCNAME[0]}(): }'

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
export DYLD_LIBRARY_PATH=/Users/rmills/boost_1_48_0/stage/lib:/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages
  
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
export RUBYOPT=rubygems

##
# deal with the photos
export PHOTOS="$HOME/Documents/Dropbox/Photos"
export QHOTOS="$PHOTOS/../../NotDropbox/Photos"
DropboxPhotos=/Volumes/Data/rmills/Dropbox/Photos
NotDropboxPhotos=/Volumes/Data/rmills/NotDropbox/Photos
if [ -e "$DropboxPhotos" ]; then 
	export PHOTOS="$DropboxPhotos"
fi
if [ -e "$NotDropboxPhotos" ]; then 
	export QHOTOS="$NotDropboxPhotos"
fi

# export ARCHFLAGS="-arch x86_64" # "-arch i386 -arch ppc"
# export CFLAGS="$ARCHFLAGS"      # "-arch i386 -arch ppc"
# export LDFLAGS="$ARCHFLAGS"     # "-arch i386 -arch ppc"
export L="-exec ls -daltF {} ;"
export X="-exec rm -rf {} ;"
export C="-exec $CE {} ;"
export I="-exec lipo -info {} ;"
export O="-exec otool -L {} ;"
export Z="-exec open {} ;"
export P="$PHOTOS"
export Q="$QHOTOS"
export G="-exec grep"
export __="{} ;"

##
# servers
export B=bocksbeutel
export S
S[0]=bocksbeutel
S[1]=housecat
S[2]=tenth
S[3]=demi

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
	ls -altF "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9"
}

dir() {
	list='ls -dlthpF'
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
  	df -h .
# 	df -k . | tail -1 | cut -f4 -d ' '
}

dirs() {
	ls -ltpF $1 | grep "^d"
}

links() {
	ls -altpF $1 | grep "^l"
	# 
	# for x in `(ls -F|grep @)`; do echo "${x::${#x}-1}" ; done
}

files() {
	ls -altpF $1 | grep "^-"
}

socks() {
	ls -altpF $1 | grep "^s"
}

diros() {
	ls -ltGF $* | sort --key=5 --numeric-sort
#	ls -ltGFp $* | tigersort +4 -n
}

diron() {
	ls -ltGF $* | sort --key=9 --ignore-case
#	ls -ltGp $* | tigersort +8 -i
}

hidden() {
	ls -altpF $1 | grep " \."
}

##
# set CE = editor of choice
if [ `uname` == 'Darwin' ]; then
	export CE=`which bbedit 2>/dev/null`
	if [ -z "$CE" ]; then
		export CE=`which edit 2>/dev/null`
	fi
else # probably linux, could be cygwin
	export CE=`which kate`
	if [ ! -z "$CE" ]; then
		export CE="kate --use"
	fi		
fi
# catch all - use good old vi!
if [ -z "$CE" ]; then
	export CE=vi
fi			

ce() {
	if [ "$CE" == "kate --use" ]; then
		kate --use "$@" 2>/dev/null >/dev/null &
	else
		"$CE" "$@"
	fi
}

##
# aliases
# dos like things
alias rename=mv
alias move=mv
alias del='sudo rm -rf'
alias dirod='ls -altGFr'
alias xcopy='ditto'
alias finder='find . -depth -name'
alias shellx=open
alias start=open

# hieroglyphics
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
alias homepages='cd ~/clanmills/Homepages/2012'
#

##
# keep QtCreator happy
mkdir -p /tmp/qt-stuff-6474/source/qt-everywhere-opensource-src-4.7.0/lib

##
# apache-maven (and itext support)
# http://maven.apache.org/download.html
export M2_HOME=/usr/local/apache-maven/apache-maven-3.0.3
export M2=$M2_HOME/bin
export PATH=$M2:$PATH
# export MAVEN_OPTS="-Xms256m -Xmx512m"

# That's all Folks!
##

##
# Your previous /Users/rmills/.profile file was backed up as /Users/rmills/.profile.macports-saved_2011-12-06_at_12:29:07
##

## rmills - I've put /opt/local/bin at the end of the path to prevent Macports from putting his commands in front of the system's
# MacPorts Installer addition on 2011-12-06_at_12:29:07: adding an appropriate PATH variable for use with MacPorts.
export PATH=$PATH:/opt/local/bin:/opt/local/sbin
# Finished adapting your PATH environment variable for use with MacPorts.

