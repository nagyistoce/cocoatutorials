#!/bin/bash

# http://stackoverflow.com/questions/1494178/how-to-define-hash-tables-in-bash

print()
{
	echo $*
}

# Array pretending to be a Pythonic dictionary
animals=( "cow:moo"
          "dinosaur:roar"
          "bird:chirp"
          "bash:rock" )

for animal in ${animals[@]} ; do
    KEY="${animal%%:*}"
    VALUE="${animal#*:}"
    printf "%s likes to %s.\n" $KEY $VALUE
done


print --------------------------
animals[$x]="caesar:come:see:conquer"

for animal in ${animals[@]} ; do
    KEY=${animal%%:*}
    VALUE=${animal#*:}
    printf "%s likes to %s.\n" $KEY $VALUE
done

print  --------------------------
echo -e "${animals[1]%%:*} is an extinct animal which likes to ${animals[1]##*:}\n"

hput() {
    eval "$1""$2"='$3'
}

hget() {
    eval echo '${'"$1$2"'#hash}'
}

hput capitols France      Paris
hput capitols Netherlands Amsterdam
hput capitols Spain       Madrid
echo `hget capitols France` and `hget capitols Netherlands` and `hget capitols Spain`

# That's all Folks!
##
