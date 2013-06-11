#!/bin/bash
# Test driver for httptest and httpIo

source ./functions.source

##
#function to test samples/httptest.cpp
httpTest()
{
    arg=$1
    scheme=${arg:0:4}

    src=$(basename "$arg")
    filename=${src%.*}
    test=${filename}.txt
    good=$datapath/${filename}.txt
    dot=x
    
    if [ $scheme = http ]; then
        dot=.
        # run tests
        runTest httptest -url $1                                  | grep -v -e ^Date  -v -e ^Last  -v -e ^Via | tr '[:upper:]' '[:lower:]'  > $test
        runTest httptest -url $1 -verb HEAD                       | grep -v -e ^Date  -v -e ^Last  -v -e ^Via | tr '[:upper:]' '[:lower:]' >> $test
        runTest httptest -url $1 -header 'Range: bytes=200-1800'  | grep -v -e ^Date  -v -e ^Last  -v -e ^Via | tr '[:upper:]' '[:lower:]' >> $test

        # check results
        diffCheckAscii $test $good
        
    fi
    printf $dot
}

##
#function to test httpio class in basicio.cpp
httpIoTest()
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
    runTest exifprint $1 "--nocurl" > $test
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

    errors=0
    testfile="http://$EXIV2_AWSUBUNTU_HOST/httptest.jpg"
    iopngfiles+=(remoteio{1..5}.png)
    iojpgfiles+=(remoteio{6..10}.jpg)
    # httptest (basic sanity test)
    printf 'httptest '
    httpTest $testfile
    
    # httpIo (more files)
    if [ $errors -eq 0 ]; then
        #Tests for httpIo
        echo 
        printf 'httpIo   '
        for name in ${iopngfiles[@]}; do
            httpIoTest "http://$EXIV2_AWSUBUNTU_HOST/$name"
        done
        for name in ${iojpgfiles[@]}; do
            httpIoTest "http://$EXIV2_AWSUBUNTU_HOST/$name"
        done
        if [ $errors -eq 0 ]; then
            printf '\nAll test cases passed\n'
        else
            printf "\n---------------------------------------------------------\n"
            echo $errors 'httpIo failed!'
        fi
    else
        printf "\n---------------------------------------------------------\n"
        echo 'test httptest failed! (httpIo was not run).'
    fi
)

# That's all Folks!
##