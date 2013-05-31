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

    errors=0

    remoteIoTest_files="http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg0.jpg \
                      http://exiv2.nuditu.com/remoteImg0.jpg \
                      https://54.251.248.216/remoteImg0.jpg  \
                      https://54.251.248.216/remoteImg1.jpg  \
                      https://54.251.248.216/remoteImg2.jpg  \
                      https://54.251.248.216/remoteImg3.jpg  \
                      https://54.251.248.216/remoteImg4.jpg  \
                      https://54.251.248.216/remoteImg5.jpg  \
                      https://54.251.248.216/remoteImg6.jpg  \
                      https://54.251.248.216/remoteImg7.jpg  \
                      https://54.251.248.216/remoteImg8.jpg  \
                      https://54.251.248.216/remoteImg9.jpg  \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg \
                      ftp://exiv2%40nuditu.com:2943026@nuditu.com/remoteImg0.jpg"

    USE_CURL=$("$bin"/exiv2 -v -V | grep ^curl= | sed s/curl=//)
    if [ "$USE_CURL" -eq "1" ]; then
      #Tests for remoteIo
      echo 
      printf 'remoteIo   '
      for i in $remoteIoTest_files; do RemoteIoTest $i; done

      if [ $errors -eq 0 ]; then
          printf '\nAll test cases passed\n'
      else
          printf "\n---------------------------------------------------------\n"
          echo $errors '\nremoteIo failed!'
      fi
    else
      #Skip remoteIo test cases
      printf '\nCurl is not used. Skip remoteio test cases.\n'
    fi
)

# That's all Folks!
##