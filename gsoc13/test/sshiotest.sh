#!/bin/bash
# Test driver for SshIo

source ./functions.source

##
#function to test remote read access in basicio.cpp
SshReadTest()
{
    arg=$1
    scheme=${arg:0:4}

    src=$(basename "$arg")
    filename=${src%.*}
    test=${filename}.txt
    good=$datapath/${filename}.txt
    dot=.
    # run tests
    runTest exifprint $1 > $test
    #check results
    diffCheckAscii $test $good

    printf $dot
}

(   cd "$testdir"

    # check environmental variable
    if [ -z $EXIV2_AWSUBUNTU_HOST ]; then
        echo "***" please set the environmental variable EXIV2_AWSUBUNTU_HOST "***"
        exit 1
    fi
    if [ -z $EXIV2_AWSUBUNTU_USERNAME ]; then
        echo "***" please set the environmental variable EXIV2_AWSUBUNTU_USERNAME "***"
        exit 1
    fi
    if [ -z $EXIV2_AWSUBUNTU_PASSWORD ]; then
        echo "***" please set the environmental variable EXIV2_AWSUBUNTU_PASSWORD "***"
        exit 1
    fi

    errors=0
    files+=(remoteImg{0..9}.jpg)
    USE_SSH=$("$bin"/exiv2 -v -V | grep ^ssh= | sed s/ssh=//)
    if [ "$USE_SSH" == "0" ]; then
        #Skip remoteIo test cases
        echo 'Ssh is not used. Skip sshio test cases.'
    else
        printf 'SSH READ '
        for name in ${files[@]}; do
            SshReadTest "ssh://$EXIV2_AWSUBUNTU_USERNAME:$EXIV2_AWSUBUNTU_PASSWORD@$EXIV2_AWSUBUNTU_HOST/www/$name"
        done
        if [ $errors -eq 0 ]; then
            printf '\nAll test cases passed\n'
        else
            echo $errors ' test cases failed!'
        fi
    fi
)
# That's all Folks!
##