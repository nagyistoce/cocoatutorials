##
# prompt and bash magic

export HOST=`(hostname|cut -d. -f 1)`
# If id command returns zero, you’ve root access.
if [ "$USER" == "rootXXX" ]; then                # $(id -u) -eq 0 ];
    # you are root, set yellow color prompt
    # http://news.softpedia.com/news/How-to-Customize-the-Shell-Prompt-40033.shtml
    PS1="\\[\! $(tput setaf 3)\\]\\u@\\h:\\w #\\[$(tput sgr0)\\] "
fi

# export PS1='\! $(tput setaf 3)\u@\h:\w$(tput sgr0) $ '
if [ "$USER" == "rootYYY" ]; then
#   PS1="\\[\! $(tput setaf 2)\\]\\u@\\h:\\w $\\[$(tput sgr0)\\] "
    PS1='\! $(tput setaf 2)\u@\h:\w$(tput sgr0) # '
fi

# http://wiki.bash-hackers.org/scripting/debuggingtips
export PS1='\! \u@\h:\w \$ '
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
BIN=${HOME}/bin

export PLATFORM=''
if [ `uname` == Darwin  ]; then
    PLATFORM=macosx
elif [ `uname -o` == Cygwin ]; then
    PLATFORM=cygwin
else
    PLATFORM=linux
fi

export FACTORY=$(hostname)
if [ $((echo $FACTORY| grep novariant 2>&1 >/dev/null);echo $?) == 0 ]; then
    FACTORY=novariant
fi

if [[ "$FACTORY" == "rmillsmbp.corp.adobe.com" ]]; then
	FACTORY=adobe
fi

# export EBSREVISION=Dev
export CBL_STONEHENGE=1
export PATH=".:${BIN}:$BIN/$PLATFORM:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:/usr/X11R6/bin:/System/Library/Frameworks/Python.framework/Versions/2.7/bin/"
export MANPATH="/usr/local/share/man:/opt/local/share/man:/usr/share/man:/usr/share/man/man1:/usr/share/man/man2:/usr/share/man/man3:/usr/local/man:/usr/local/share/man/:/usr/X11R6/man:/opt/subversion/man"
export DISPLAY=:0.0
export CLASSPATH=".:${HOME}/classpath:${HOME}/classpath/Multivalent20060102.jar:${HOME}/classpath/DVI20060102.jar"
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/usr/lib/pkgconfig:/opt/local/lib/pkgconfig


## 
# SCM support
export P4USER=rmills
export P4PORT=home:1666
export P4CLIENT=imac

export  FLEET="clanmills.com"
export  PINGS="$FLEET"

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
if [ ! -z $PERL5LIB ]; then
    export PERL5LIB="$PERL5LIB:${BIN}"
else
    export PERL5LIB="${BIN}"
fi

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
export L="-exec ls -dalt {} ;"
export X="-exec rm -rf {} ;"
export C="-exec ce {} ;"
export I="-exec lipo -info {} ;"
export O="-exec otool -L {} ;"
export Z="-exec open {} ;"
export P="-exec p4 edit {} ;"
export R="-exec p4 revert {} ;"
export G="-exec grep"
export __=";"
export ___="{} ;"

##
# Install image magick
# http://mirror1.smudge-it.co.uk/imagemagick/www/binary-releases.html#macosx
# export MAGICK_HOME="/Users/rmills/gnu/ImageMagick"
# export PATH="$MAGICK_HOME:$PATH"
# export DYLD_LIBRARY_PATH="$MAGICK_HOME/lib:$DYLD_LIBRARY_PATH"

##
# functions
dir() {
    list='ls -dlthpF'
    if [ ${#*} -eq 0 ]; then
        $list *
    elif [ ${#*} -eq 1 ]; then
        if [ -d "$1" ]; then
#           (pushd "$1">/dev/null;$list *)
            if [ "$1" == '.' ]; then
                $list
            else 
                $list $(dirname "$1")/$(basename "$1")/*
            fi
        elif [ "$1" = /od ]; then
            shift
            eval dirod $*
        elif [ "$1" = /os ]; then
            shift
            eval diros $*
        else
            $list "$@"
        fi
    else
        $list "$@"
    fi
}

export HISTTIMEFORMAT="%T "
history() { 
    builtin history | tail -30
}

free() {
    df -h .
#   df -k . | tail -1 | cut -f4 -d ' '
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
    ls -lt $* | sort --key=5 --numeric-sort
#   ls -ltFp $* | tigersort +4 -n
}

diron() {
    ls -lt  $* | sort --key=9 --ignore-case
#   ls -ltp $* | tigersort +8 -i
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
    ##
    # parse the arguments
    target=""
    error=0
    cd=0
    help=0
    verbose=0
    parse=1
    args=()
    for arg in "$@" ; do
        target="$arg"
        if [ $parse == 1 ]; then
            # to lower case if more than 2 characters
            if [ "${#arg}" -gt 2 ]; then
                arg="$(echo $arg|perl -e 'print lc <>;')"
            fi
            case "$arg" in
              -n|--nocd)     cd=0       ;;
              -c|--cd)       cd=1       ;;
              -v|--verbose)  verbose=1  ;;
             -\?|--help|-h)  help=1     ;;
              --)            parse=0    ;;
              *)             parse=0    ; args+=("$target") ;;
            esac
        else
            args+=("$target")
        fi
    done
    
    ##
    # report or bail
    if [ ${#args} == 0 ]; then help=1; fi
    if [ $help == 1 ]; then
        echo "to [--[no]cd --help --verbose --] [command ...] filename"
        error=1
    fi
    if [ $error == 1 ]; then return; fi
    
    unset args[${#args[@]}-1]       
    argc=${#args}
    if [ $verbose == 1 ]; then
        echo args   = ${args[@]} "(argc = $argc)"
        echo target = "$target" ;
        echo cd     =  $cd
    fi
    
    ##
    # pain over.  find possible targets
    tmp=/tmp/2.tmp
    find . -name "$target" -print > $tmp
    # count=$(wc -l $tmp | rev | sed -e "s/.* //" | rev)
    declare -i count=0
    for i in $(cat $tmp); do : $((count++));done

    if [ "$count" == "1" ]; then
        dir=$(cat "$tmp")
        dir=$(dirname "$dir")
        if [ $argc -gt 0 ]; then
            if [ $cd == 1 ]; then
                pushd "$dir"  >/dev/null
                echo "$" cd "$dir" ";" "${args[@]}" "$target"
                find . -name "$target" -print0 | xargs -0 "${args[@]}"
                popd >/dev/null
            else
                echo ">" "${args[@]}" "$dir/$target"
                find . -name "$target" -print0 | xargs -0 "${args[@]}"
            fi
        elif [ $argc -eq 0 ]; then
            if [ -d "$dir/$target" ]; then
                dir="$dir/$target"
            fi
            echo $ cd "$dir" 
            cd "$dir" 
        fi
    elif [ "$count" == "0" ]; then
        echo "*** NO file found ***"
    else
        echo "*** TOO Many files found ***"
        cat $tmp
    fi
}
alias 2=to

path() {
    if [ -d $1 ]; then
    	(cd $1 ; pwd)
    else
    	(cd $(dirname $1) ;echo $(pwd)/$(basename $1))
    fi
}


##
# set CE = editor of choice (obsolete)
#if [ -z "$SSH_CLIENT" ]; then
#   if   [ "$PLATFORM" == 'macosx' ]; then
#       export CE=`which bbedit &>/dev/null`
#       if [ -z "$CE" ]; then
#           export CE=`which edit &>/dev/null`
#       fi
#   elif [ "$PLATFORM" == 'cygwin' ]; then
#       CE='/cygdrive/c/Users/rmills/com/ce.exe'
#   elif [ "$PLATFORM" == 'linux' ]; then
#       export CE=`which kate`
#       if [ ! -z "$CE" ]; then
#           export CE="kate --use"
#           export CE2="&>/dev/null &"
#       fi      
#   fi
#fi
#
#if [ "$PLATFORM" == "macosx" ]; then
#   export CE=bbedit
#fi
#
#ce()
#{
#   $CE "$@" 2>/dev/null >/dev/null &
#}

# catch all - use good old vi!
#if [ -z "$CE" ]; then
#   export CE=vi
#fi

##
# aliases
# dos like things
alias rename=mv
alias move=mv
alias del='sudo rm -rf'
alias dirod='ls -alt'
alias xcopy='ditto'
alias finder='find . -depth -iname'
alias shellx=open
alias start=open
alias get=git

if [ $PLATFORM == linux ]; then 
    alias open=xdg-open
    alias shellx=open
    alias start=open
#   export PATH=$PATH:/usr/lib/java/jre1.7.0/bin/
    export JAVA_HOME=/usr/local/java/jdk1.7.0_21
    export PATH=$PATH:$HOME/bin:$JAVA_HOME/bin
    JRE_HOME=/usr/local/java/jre1.7.0_21
    PATH=$PATH:$HOME/bin:$JRE_HOME/bin
fi

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
alias clanmills='cd ~/clanmills'
alias collages='cd ~/Pictures/Picasa/Collages'
alias tuts='cd ~/Projects/cocoatutorials'
alias QtSee='cd ~/Projects/cocoatutorials/QtSee'
alias robin='cd ~/Projects/cocoatutorials/robin'
#
# Favorite servers
alias clanmills.com='ssh clanmil1@clanmills.com'

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


## Warren's remind/remember magic
# Function "remind" 
remind() {
  # remindme - Searches a data file for matching lines, or shows the entire contents
  # of the data file if no argument is specified.
  rememberFile="${HOME}/bin/.remember"
  if [ $# -eq 0 ] ; then
    if [ -e "$rememberFile" ]; then
      more "$rememberFile"
    else
      echo "$rememberFile" does not exist
    fi
  else
    grep -i "$@" "$rememberFile" | ${PAGER:-more}
  fi
}
# Function "remember" 
remember() {
  # remember - An easy command-line-based memory pad.
  rememberFile="${HOME}/bin/.remember"
  if [ $# -eq 0 ] ; then
          echo "Enter note, end with ^D: "
          cat - >> "$rememberFile"
  else
          echo "$@" >> "$rememberFile"
  fi
}

##
# run the local kdiff3 type tool
differ () {
	declare -a args
	for arg in "$@"; do
		if [ -e $arg ]; then
			args+=("$arg")
		fi
		shift
	done
	cmd=kdiff3
	if [ "$PLATFORM" == "macosx" ]; then
		cmd=opendiff
	fi
	echo $cmd ${args[*]}
	     $cmd ${args[*]} &
}

##
# platform adjustments
if [ "$PLATFORM" == "cygwin" ]; then
    d=$(find /c/boost -maxdepth 1 -type d -name "boo*" | sort | tail -1)
    if [ -d "$d" ]; then
        export "BOOST_ROOT=$d"
    fi
fi

if [ "$PLATFORM" == "macosx" ]; then
    : export DYLD_LIBRARY_PATH=/Users/rmills/boost_1_48_0/stage/lib:/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages
    # export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH}:/opt/local/lib/"
fi

if [ "$FACTORY" == "rmills-ubuntu" ]; then
    export PHOTOS=/Windows/Users/rmills/Documents/Dropbox/Photos
    export QHOTOS=/Windows/Users/rmills/Documents/Dropbox/Photos
    unset dir
fi

export EXIV2_AWSUBUNTU_USERNAME=ubuntu
export EXIV2_AWSUBUNTU_PASSWORD=p%40ssw0rd
export EXIV2_AWSUBUNTU_HOST=54.251.248.216

##
# last minute adjustments for different machines
if [ "$FACTORY" == "novariant" ]; then
    export DISPLAY=:4.0
    export "PATH=~/local/bin/:$PATH"
    alias ll=dir
    export UUT=192.168.116
fi

if [ "$FACTORY" == "rmills-imac" ]; then
    export "PATH=/opt/subversion/bin:$PATH"
fi

if [ "$FACTORY" == "rmills-w7" -a "$PLATFORM" == "cygwin" ]; then
	FACTORY="adobe"
fi

if [ "$FACTORY" == "adobe" ]; then
    P4CLIENT=rmills-mbp-X5d2
    P4PORT=ssl:scm003.corp.adobe.com:3070
    if [ $PLATFORM == "cygwin" ]; then 	P4CLIENT="rmills-w7" ; fi
	
	if [ $PLATFORM == "macosx" ]; then
		 docs () {
			cd /Langley/1740/rmills-mbp/PE/12X/Main/Tools/ReaderNextBuild/Documents
			export P4CLIENT=rmills-mbp
		}
	fi
fi

if [ "$PLATFORM" == "macosx" ]; then
  	alias xce='open -a /Applications/Xcode.app'
  	if [ -e /opt/local/bin/svn ]; then
  		alias svn='/opt/local/bin/svn'
  	fi

	pushd / >/dev/null
		# turn off local backup in time machine
		# https://discussions.apple.com/message/21716617#21716617
		# https://discussions.apple.com/thread/3233868
		if [ -e .MobileBackups.trash ]; then
  			sudo chmod -Rf 777 .MobileBackups.trash
  			sudo rm -rf .MobileBackups.trash
  		fi
  		# tmutil = Time Machine Util ; pmset = Power Management Settings ;
  		sudo tmutil disablelocal
  	popd >/dev/null
  	if [ $FACTORY == rmills-mbp.local -o $FACTORY == rmillsmbp13.local ]; then
    	P4CLIENT=rmills-mpb13-7Xd2
    	P4PORT=ssl:scm003.corp.adobe.com:3070
    fi
fi

if [ "$PLATFORM" == "linux" ]; then
  	alias svn='/usr/local/bin/svn'
fi

if [ "$PLATFORM" == "cygwin" -a "$FACTORY" == "rmills-vista" ]; then
    cp /cygdrive/c/Users/rmills/AppData/Local/Microsoft/BingDesktop/themes/*.jpg /cygdrive/z/Users/rmills/clanmills/files/themes/
fi

cd ~/
# That's all Folks!
##

##
# One last thing:
##

## rmills - I've put /opt/local/bin at the end of the path to prevent Macports from putting his commands in front of the system's
# MacPorts Installer addition on 2011-12-06_at_12:29:07: adding an appropriate PATH variable for use with MacPorts.
export PATH=$PATH:/opt/local/bin:/opt/local/sbin:/opt/pkgconfig/bin
# Finished adapting your PATH environment variable for use with MacPorts.



