##
# prompt and bash magic

export HOST=`(hostname|cut -d. -f 1)`
# If id command returns zero, you’ve root access.
if [ "$USER" == "root" ]; then                # $(id -u) -eq 0 ];
	# you are root, set yellow color prompt
	# http://news.softpedia.com/news/How-to-Customize-the-Shell-Prompt-40033.shtml
  	PS1="\\[\! $(tput setaf 3)\\]\\u@\\h:\\w #\\[$(tput sgr0)\\] "
fi

export PS1='\! $(tput setaf 3)\u@\h:\w$(tput sgr0) $ '
export PS1='\! \u@\h:\w $ '
if [ "$USER" == "root" ]; then
#  	PS1="\\[\! $(tput setaf 2)\\]\\u@\\h:\\w $\\[$(tput sgr0)\\] "
  	PS1='\! $(tput setaf 2)\u@\h:\w$(tput sgr0) # '
fi

# http://wiki.bash-hackers.org/scripting/debuggingtips
export PS4='+(${BASH_SOURCE}:${LINENO}): ${FUNCNAME[0]:+${FUNCNAME[0]}(): }'

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
if [ -e /tmp/robin/bin ]; then
	BIN=/tmp/robin/bin
else
	BIN=${HOME}/bin
fi

applebin=''
if [ `uname` == Darwin ]; then
	export PLATFORM=macosx
	export "AUSER=robin_mills"
	export "AHOME=/Network/Servers/hawthorne.apple.com/Volumes/holt/$AUSER"
	applebin="$HOME/robin_mills_bin:$AHOME/bin"
elif [ `uname -o` == Cygwin ]; then
	export PLATFORM=cygwin
else
	export PLATFORM=linux
fi

export PATH=".:${BIN}:$BIN/$PLATFORM:$applebin:/usr/local/cbl/bin:/usr/local/bin:/usr/X11R6/bin:/Developer/Tools:${PATH}:$MAGICK_HOME/bin:/System/Library/Frameworks/Python.framework/Versions/2.7/bin/:/Developer/usr/bin:/usr/libexec:"
export MANPATH="/opt/local/share/man:/usr/share/man:/usr/share/man/man1:/usr/share/man/man2:/usr/share/man/man3:/usr/local/man:/usr/local/share/man/:/usr/X11R6/man:/opt/subversion/man"
export DISPLAY=:0.0
export CLASSPATH=".:${HOME}/classpath:${HOME}/classpath/Multivalent20060102.jar:${HOME}/classpath/DVI20060102.jar"
export DYLD_LIBRARY_PATH=/Users/rmills/boost_1_48_0/stage/lib:/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages
# export DYLD_LIBRARY_PATH=/opt/local/lib/
  
## 
# SCM support
export P4USER=rmills
export P4PORT=home:1666
export P4CLIENT=imac

export PINGS="bocksbeutel tenth demi housecat"

## 
# scripting support
# used by cpan
export "FTP_PASSIVE=1"
  
# export PYTHONPATH=~/scripts/:${HOME}/bin/:.:/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages
# export PYTHONPATH=/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages

export VERSIONER_PERL_PREFER_32_BIT=no
export VERSIONER_PYTHON_PREFER_32_BIT=no
export PERLPATH=/Users/rmills/gnu/ImageMagick
export PYTHONPATH="/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages:$PYTHONPATH"  
export RUBYOPT=rubygems
export PERL5LIB="$PERL5LIB:${BIN}"

##
# clanmills support
export REMOTEHOST=clanmills.homedns.org
export GPGKEY=CF1649DA


##
# photos
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

##
# finder helpers
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

crdir() {
	dir="$@"
	if [ $# -eq 1 ]; then
		mkdir "$@"
		cd    "$@"
	elif [ $# -eq 0 ]; then
		echo "*** give me an argument,buddy ***"
	else
		echo "*** too many arguments ***"
	fi 
}
alias crd=crdir

to() {
	# I have to find a simpler way to copy the arguments and set target
	let argc=-1
	for arg in "$@" ; do target="$arg" ; argc=$((argc+1)) ; done
	args=()
	let a=0
	for arg in "$@" ; do
		if [ $a -lt $argc ]; then
			args[$a]="$arg"
		fi
		a=$((a+1))
	done
	
	#if [ $argc -gt  0 ]; then
	#	for arg in $(seq 0 $argc); do echo arg $arg = "${args[$arg]}" ; done
	#fi
	
	# pain over.  find possible targets
	if [ $argc -ge  0 ]; then
		tmp=/tmp/2.tmp
		find . -name "$target" -print > $tmp
		count=$(wc -l $tmp | cut -b1-9)

		if [ $count -eq 1 ]; then
			dir=$(dirname `cat "$tmp"`)
			if [ $argc -gt 0 ]; then
				pushd "$dir"  > /dev/null
				echo $ "${args[@]}" "$target"
				find . -name "$target" -print0 | xargs -0 "${args[@]}"
				popd >/dev/null
			elif [ $argc -eq 0 ]; then
				echo $ cd "$dir" 
				cd "$dir" 
			fi
			rm "$tmp"
		elif [ $count == 0 ]; then
			echo "NONE"
		else
			echo "TOO Many"
		fi
		
		if [ -e "$tmp" ]; then
			cat $tmp
		fi
	fi
}

##
# set CE = editor of choice
if [ `uname` == 'Darwin' ]; then                # Mac
	export CE=`which bbedit 2>/dev/null`
	if [ -z "$CE" ]; then
		export CE=`which edit 2>/dev/null`
	fi
elif [ `uname -o` == 'Cygwin' ]; then            # Cygwin
	CE='/cygdrive/c/Users/rmills/com/ce.exe'
else                                            # probably linux
	export CE=`which kate`
	if [ ! -z "$CE" ]; then
		export CE="kate --use"
	fi		
fi
# catch all - use good old vi!
if [ -z "$CE" ]; then
	export CE=vi
fi
alias 2=to			

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

# one-liners
alias ridLeadingSpace="pbpaste | perl -e 'while(<>){\$_=~s/^\s*//g;print(\$_);}' | pbcopy "

# favorite directories
year=$(date "+%Y")
alias homepages="cd ~/clanmills/Homepages/$year"
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

##
# apache-maven (and itext support)
# http://maven.apache.org/download.html
# export M2_HOME=/usr/local/apache-maven/apache-maven-3.0.3
# export M2=$M2_HOME/bin
# export PATH=$M2:$PATH
# export MAVEN_OPTS="-Xms256m -Xmx512m"

# That's all Folks!
##

##
# One last thing:
##
# Your previous /Users/rmills/.profile file was backed up as /Users/rmills/.profile.macports-saved_2011-12-06_at_12:29:07
##

## rmills - I've put /opt/local/bin at the end of the path to prevent Macports from putting his commands in front of the system's
# MacPorts Installer addition on 2011-12-06_at_12:29:07: adding an appropriate PATH variable for use with MacPorts.
export PATH=$PATH:/opt/local/bin:/opt/local/sbin
# Finished adapting your PATH environment variable for use with MacPorts.

