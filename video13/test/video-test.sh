#!/bin/bash
# Test driver for video files
#
# video-asf.wmv        http://www.educationalquestions.com/video/DLP_PART_2_768k.wmv
# video-avi.avi        http://redmine.yorba.org/attachments/631/Nikon_Coolpix_S3000.AVI
# video-matroska.mkv   http://www.bunkus.org/videotools/mkvtoolnix/samples/vsshort-vorbis-subs.mkv
# video-quicktime.mp4  http://dev.exiv2.org/attachments/362/20100709_002.mp4

source ./functions.source

##
# set up the output file
out=$(real_path "$testdir/$this.out")
copyTestFile "video/$this.out"

(	cd "$testdir"

	copyVideoFiles
	##
	# find video files data/video and copy them for testing
	declare -a videos
	for video in $datadir/video/* ; do
		# http://stackoverflow.com/questions/965053/extract-filename-and-extension-in-bash
		ext="${video##*.}"
		if [ $ext != out ]; then
			copyTestFile "$video" 
			videos+=($(basename "$video"))
		fi
	done
	# http://stackoverflow.com/questions/7442417/how-to-sort-an-array-in-bash
	readarray -t sorted < <(printf '%s\0' "${videos[@]}" | sort -z | xargs -0n1) 
	
	for video in ${videos[*]}; do
	    printf "." >&3
    	echo
        echo "-----> $video <-----"
    	echo
        echo "Command: exiv2 -u -pa $video"
        # run command                 | no binary and no Date tags
	    runTest exiv2 -u -pa "$video" | sed -E -e 's/\d128-\d255/_/g' | grep -v -e Date | grep -v -e NumOfC
    done

) 3>&1 2>&1 > "$out" 

echo "."

# ----------------------------------------------------------------------
# Result
diffCheck "$out" "$testdir/$datadir/video/$this.out" 

if [ $errors ]; then
	echo -e $errors 'test case(s) failed!'
else
	echo -e "all testcases passed."
fi

# That's all Folks!
##
