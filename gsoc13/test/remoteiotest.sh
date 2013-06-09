#!/bin/bash
# Test driver for httptest and httpIo

source ./functions.source


##
#function to test remote read access in basicio.cpp
RemoteReadTest()
{
    arg=$1
    scheme=${arg:0:4}

    src=$(basename "$arg")
    filename=${src%.*}
    test=${filename}.txt
    good=$datapath/${filename}.txt
    dot=.
    # run tests
    runTest exifprint $1 "--curl" > $test
    #check results
    diffCheckAscii $test $good

    printf $dot
}

##
#function to test remoteio read/write access in basicio.cpp
RemoteIOTest()
{
    arg=$1
    scheme=${arg:0:4}

    src=$(basename "$arg")
    filename=${src%.*}
    test=${filename}.txt
    good=$datapath/remoteio.txt
    cmdadd=$datapath/cmdremoteadd.txt
    cmddel=$datapath/cmdremotedel.txt
    dot=.
    # add/set metadata
    runTest exiv2 "-m" $cmdadd $1
    # print out the metadata
    runTest exifprint $1 "--curl" > $test
    # delete metadata
    runTest exiv2 "-m" $cmddel $1
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
    iopngfiles+=(remoteio{1..5}.png)
    iojpgfiles+=(remoteio{6..10}.jpg)
    USE_CURL=$("$bin"/exiv2 -v -V | grep ^curlprotocols= | sed s/curlprotocols=//)
    if [ "$USE_CURL" == "" ]; then
        #Skip remoteIo test cases
        echo 'Curl is not used. Skip remoteio test cases.'
    else
        # HTTP protocol
        if [[ "$USE_CURL" == *http* ]]; then
            errors=0
            printf 'HTTP  IO '
            for name in ${iopngfiles[@]}; do
                RemoteIOTest "http://$EXIV2_AWSUBUNTU_HOST/$name"
            done
            for name in ${iojpgfiles[@]}; do
                RemoteIOTest "http://$EXIV2_AWSUBUNTU_HOST/$name"
            done
            if [ $errors -eq 0 ]; then
                printf '\nAll test cases passed\n'
            else
                echo $errors ' test cases failed!'
            fi
        else
            echo 'Curl doesnt support HTTP'
        fi

        if [[ "$USE_CURL" == *https* ]]; then
            errors=0
            printf 'HTTPS IO '
            for name in ${iopngfiles[@]}; do
                RemoteIOTest "http://$EXIV2_AWSUBUNTU_HOST/$name"
            done
            for name in ${iojpgfiles[@]}; do
                RemoteIOTest "http://$EXIV2_AWSUBUNTU_HOST/$name"
            done
            if [ $errors -eq 0 ]; then
                printf '\nAll test cases passed\n'
            else
                echo $errors ' test cases failed!'
            fi
        else
            echo 'Curl doesnt support HTTPS'
        fi

        if [[ "$USE_CURL" == *ftp* ]]; then
            errors=0
            printf 'FTP READ '
            for name in ${files[@]}; do
                RemoteReadTest "ftp://$EXIV2_AWSUBUNTU_USERNAME:$EXIV2_AWSUBUNTU_PASSWORD@$EXIV2_AWSUBUNTU_HOST/www/$name"
            done
            if [ $errors -eq 0 ]; then
                printf '\nAll test cases passed\n'
            else
                echo $errors ' test cases failed!'
            fi
        else
            echo 'Curl doesnt support FTP'
        fi

        if [[ "$USE_CURL" == *sftp* ]]; then
            errors=0
            printf 'SFTP READ '
            for name in ${files[@]}; do
                RemoteReadTest "sftp://$EXIV2_AWSUBUNTU_USERNAME:$EXIV2_AWSUBUNTU_PASSWORD@$EXIV2_AWSUBUNTU_HOST/var/www/$name"
            done
            if [ $errors -eq 0 ]; then
                printf '\nAll test cases passed\n'
            else
                echo $errors ' test cases failed!'
            fi
        else
            echo 'Curl doesnt support SFTP'
        fi
    fi
)

# That's all Folks!
##