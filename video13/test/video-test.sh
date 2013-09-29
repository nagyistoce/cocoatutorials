#!/bin/bash
# Test driver for video files
#
# video-asf.wmv        http://www.educationalquestions.com/video/DLP_PART_2_768k.wmv
# video-avi.avi        http://redmine.yorba.org/attachments/631/Nikon_Coolpix_S3000.AVI
# video-matroska.mkv   http://www.bunkus.org/videotools/mkvtoolnix/samples/vsshort-vorbis-subs.mkv
# video-quicktime.mp4  http://dev.exiv2.org/attachments/362/20100709_002.mp4

source ./functions.source

out=$(real_path "$testdir/$this.out")

(	cd "$testdir"

    for file in ../data/video/video-*; do
        video="`basename "$file"`"
		if [ $video != "video-test.out" ] ; then

	        printf "." >&3

    	    echo
        	echo "-----> $video <-----"

	        copyTestFile "video/$video" "$video"

    	    echo
        	echo "Command: exiv2 -u -pa $video"
        	# run command                 | replace high bytes with underscores and remove Date tags
	        runTest exiv2 -u -pa "$video" | sed -E -e 's/[\d128-\d255]/_/g' | grep -v -e Date
    	    exitcode="$?"
        	echo "Exit code: $exitcode"

	        if [ "$exitcode" -ne 0 -a "$exitcode" -ne 253 ] ; then
    	        continue
        	fi
		fi
    done

) 3>&1 > "$testdir/video-test.out" 2>&1

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