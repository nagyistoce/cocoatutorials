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
    test=${src}.httptst
    good=$datapath/${src}.httpgd
    dot=x
    
    if [ $scheme = http ]; then
        dot=.
        # run tests
        runTest httptest -url $1                                  | grep -v -e ^Date  -v -e ^Last          | tr -d $'\r'  > $test
        runTest httptest -url $1 -verb HEAD                       | grep -v -e ^Date  -v -e ^Last -v -e ^$ | tr -d $'\r' >> $test
        runTest httptest -url $1 -header 'Range: bytes=200-1800'  | grep -v -e ^Date  -v -e ^Last -v -e ^$ | tr -d $'\r' >> $test

        # check results
        diffCheck $test $good 
        
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
    test=${src}.httpiotst
    good=$datapath/${src}.httpiogd
    dot=x

    if [ $scheme = http ]; then
        dot=.
        # run tests
        runTest exifprint $1 > $test
    
        #check results
        diffCheck $test $good
    fi
    printf $dot
}

(   cd "$testdir"

    errors=0

    httpTest_file="http://exiv2.nuditu.com/httpio0.jpg"
    httpIoTest_files="http://exiv2.nuditu.com/httpio0.jpg \
                      http://exiv2.nuditu.com/httpio1.jpg \
                      http://exiv2.nuditu.com/httpio2.jpg \
                      http://exiv2.nuditu.com/httpio3.jpg \
                      http://exiv2.nuditu.com/httpio4.jpg \
                      http://exiv2.nuditu.com/httpio5.jpg \
                      http://exiv2.nuditu.com/httpio6.jpg \
                      http://exiv2.nuditu.com/httpio7.jpg \
                      http://exiv2.nuditu.com/httpio8.jpg \
                      http://exiv2.nuditu.com/httpio9.jpg"

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
            echo 'All test cases passed'
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