#!/bin/bash
# Test driver for httptest and httpIo

source ./functions.source


##
#function to test remoteio class in basicio.cpp
RemoteIoTest()
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

(   cd "$testdir"

    # check environmental variable
    if [ -z $EXIV2_AWSUBUNTU_HOST ]; then
        echo "***" do not know host for AWSUBUNTU "***"
        exit 1
    fi
    if [ -z $EXIV2_AWSUBUNTU_USERNAME ]; then
        echo "***" do not know username for AWSUBUNTU "***"
        exit 1
    fi
    if [ -z $EXIV2_AWSUBUNTU_PASSWORD ]; then
        echo "***" do not know password for AWSUBUNTU "***"
        exit 1
    fi

    errors=0
    files+=(remoteImg{0..9}.jpg)
    USE_CURL=$("$bin"/exiv2 -v -V | grep ^curlprotocols= | sed s/curlprotocols=//)
    if [ "$USE_CURL" == "" ]; then
        #Skip remoteIo test cases
        echo 'Curl is not used. Skip remoteio test cases.'
    else
        echo 'RemoteIo'
        # HTTP protocol
        if [[ "$USE_CURL" == *http* ]]; then
            errors=0
            printf 'HTTP  '
            for name in ${files[@]}; do
                RemoteIoTest "http://$EXIV2_AWSUBUNTU_HOST/$name"
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
            printf 'HTTPS '
            for name in ${files[@]}; do
                RemoteIoTest "https://$EXIV2_AWSUBUNTU_HOST/$name"
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
            printf 'FTP   '
            for name in ${files[@]}; do
                RemoteIoTest "ftp://$EXIV2_AWSUBUNTU_USERNAME:$EXIV2_AWSUBUNTU_PASSWORD@$EXIV2_AWSUBUNTU_HOST/www/$name"
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
            printf 'SFTP  '
            for name in ${files[@]}; do
                RemoteIoTest "sftp://$EXIV2_AWSUBUNTU_USERNAME:$EXIV2_AWSUBUNTU_PASSWORD@$EXIV2_AWSUBUNTU_HOST/var/www/$name"
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