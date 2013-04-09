#!/bin/bash
# Test driver for httptest and httpIo

source ./functions.source
##
#function to test samples/httptest.cpp
httpTest()
{
    src=`basename "$1"`
    test=${src}.httptst
    good=$datapath/${src}.httpgd

    #run tests
    runTest httpTest -url $1 | grep --text -v -e ^Date  -v -e ^Last -v -e ^$ | sed 's/ //g' | sort > $test
    runTest httpTest -url $1 -verb HEAD | grep --text -v -e ^Date  -v -e ^Last -v -e ^$ | sed 's/ //g' | sort >> $test
    runTest httpTest -url $1 -header 'Range: bytes=200-800' | grep --text -v -e ^Date  -v -e ^Last -v -e ^$ | sed 's/ //g' | sort >> $test

    #check results
    diffCheck $test $good
    printf "."
}
##
#function to test httpio class in basicio.cpp
httpIoTest()
{
    src=`basename "$1"`
    test=${src}.httpiotst
    good=$datapath/${src}.httpiogd

    #run tests
    runTest exifprint $1 > $test
	
    #check results
    diffCheck $test $good
    printf "."
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

    #A test for httptest
    echo 
    printf "Run test for httptest"
    httpTest $httpTest_file
	
    if [ $errors -eq 0 ]; then
        #Tests for httpIo
        echo 
        printf "Run tests for httpIo"
        for i in $httpIoTest_files; do httpIoTest $i; done

        printf "\n---------------------------------------------------------\n"
        if [ $errors -eq 0 ]; then
            echo 'All test cases passed'
        else
            echo $errors 'test cases for httpIo failed!'
        fi
    else
        printf "\n---------------------------------------------------------\n"
        echo 'The test for httptest failed! Dont run tests for httpIo.'
    fi
)

# That's all Folks!
##