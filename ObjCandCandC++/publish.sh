#!/bin/bash

tut=`basename $PWD`
rm -rf build
mv      $tut.pdf ~/clanmills/articles/cocoatutorials/
zip              ~/clanmills/articles/cocoatutorials/$tut.zip -r *
# cd      ~/clanmills
# ./syncup
