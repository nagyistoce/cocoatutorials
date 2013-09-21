#!/bin/bash
# Test driver for video files

source ./functions.source

echo "Creating temporary directory modifiedvideo to test Video Metadata Read-Write functionality*****" 

mkdir $testdir/modifiedvideo
videos=$testdir/modifiedvideo/video*

#copy all test Video files to temporary Directory
cd $testdir
for file in ../data/video/video-*; do
	 video="`basename "$file"`"
		if [ $video != "video-test.out" ] ; then
			copyTestFile "video/$video" "modifiedvideo/$video"
		fi
done

#Write all Metadata to the files
echo "Writing Metadata To the RIFF,QuickTime and Matroska video Files.."

runTest exiv2 -M "set Xmp.video.MicroSecPerFrame 64" $videos
runTest exiv2 -M "set Xmp.video.MaxDataRate 4096" $videos
runTest exiv2 -M "set Xmp.video.FrameCount 2048" $videos
runTest exiv2 -M "set Xmp.video.InitialFrames 4" $videos
runTest exiv2 -M "set Xmp.video.StreamCount 2" $videos
runTest exiv2 -M "set Xmp.video.SuggestedBufferSize 1024" $videos
runTest exiv2 -M "set Xmp.video.Width 240" $videos
runTest exiv2 -M "set Xmp.video.Height 320" $videos
runTest exiv2 -M "set Xmp.video.FileDataRate 128" $videos
runTest exiv2 -M "set Xmp.video.Duration 2048" $videos
runTest exiv2 -M "set Xmp.video.Codec mjpg" $videos
runTest exiv2 -M "set Xmp.video.FrameRate 1024" $videos
runTest exiv2 -M "set Xmp.video.VideoQuality 128" $videos
runTest exiv2 -M "set Xmp.video.VideoSampleSize 256" $videos
runTest exiv2 -M "set Xmp.audio.Codec mpv4" $videos
runTest exiv2 -M "set Xmp.audio.SampleRate 32" $videos
runTest exiv2 -M "set Xmp.audio.SampleCount 32" $videos
runTest exiv2 -M "set Xmp.video.DateUT $date" $videos
runTest exiv2 -M "set Xmp.video.Comment Metadata was Edited Using Exiv2" $videos
runTest exiv2 -M "set Xmp.video.Language Kannada" $videos
runTest exiv2 -M "set Xmp.video.Country India" $videos
runTest exiv2 -M "set Xmp.video.Copyright Photographer" $videos
runTest exiv2 -M "set Xmp.video.Genre Sample Test Video" $videos
runTest exiv2 -M "set Xmp.video.Software Exiv2 0.25" $videos
runTest exiv2 -M "set Xmp.video.Junk Its a junk Data" $videos
runTest exiv2 -M "set Xmp.video.MediaLanguage English" $videos

echo "Metadata has been written to the File"

(	cd "$testdir"

    for file in $testdir/modifiedvideo/video-*; do
        video="`basename "$file"`"
		if [ $video != "video-test.out" ] ; then
	        printf "." >&3
    	    echo
        	echo "-----> $video <-----"
    	    echo
        	echo "Command: exiv2 -u -pa $video"
	        runTest exiv2 -u -pa "$video"
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


#Delete Temporary Directory 
rm $testdir/modifiedvideo -rf

diffCheck "$testdir/video-test.out" "$testdir/$datadir/video-test.out"

if [ $errors ]; then
	red='\e[0;31m' #Display fonts in Red color
	NC='\e[0m' # No Color
	echo -e "${red}Test cases Failed because of Changes in the Source code or Test samples${NC}"
else
	green='\e[0;32m' #Display fonts in Red color
	NC='\e[0m' # No Color
	echo -e "${green}All testcases passed.${NC}"
	echo "That's all Folks!"
fi

# That's all Folks!
##
