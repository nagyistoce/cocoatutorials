#!/bin/bash

book=`basename $PWD`

rm -rf $book.ps
rm -rf $book.pdf

# a2ps ReadMe.txt -1 --font-size=9 "--setpagedevice=PageSize:[595 842]" --output $book.ps
enscript ReadMe.txt -T 4 --output $book.ps
pstopdf $book.ps
rm -rf  $book.ps
open    $book.pdf
