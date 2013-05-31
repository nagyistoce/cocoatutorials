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
    good=$datapath/${filename}.txt
    dot=x

    if [ $scheme = http ]; then
        dot=.
        # run tests
        runTest exifprint $1 "--nocurl" > $test
    
        #check results
        diffCheckAscii $test $good
    fi
    printf $dot
}

(   cd "$testdir"

    errors=0

    httpTest_file="http://exiv2.nuditu.com/httptest.jpg"
    httpIoTest_files="http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg1.jpg \
                      http://exiv2.nuditu.com/remoteImg2.jpg \
                      http://exiv2.nuditu.com/remoteImg3.jpg \
                      http://exiv2.nuditu.com/remoteImg4.jpg \
                      http://exiv2.nuditu.com/remoteImg5.jpg \
                      http://exiv2.nuditu.com/remoteImg6.jpg \
                      http://exiv2.nuditu.com/remoteImg7.jpg \
                      http://exiv2.nuditu.com/remoteImg8.jpg \
                      http://exiv2.nuditu.com/remoteImg9.jpg"

    # httptest (basic sanity test)
    echo 
    printf 'httptest '
    httpTest $httpTest_file
    
    # httpIo (more files)
    if [ $errors -eq 0 ]; then
        #Tests for httpIo
        echo 
        printf 'httpIo   '
        for i in $httpIoTest_files; do httpIoTest $i; done

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