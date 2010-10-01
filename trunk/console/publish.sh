#!/bin/bash

rm -rf build
mv console.pdf ~/clanmills/articles/cocoatutorials/
zip            ~/clanmills/articles/cocoatutorials/console.zip -r *
cd ~/clanmills
./syncup
