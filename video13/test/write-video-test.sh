#!/bin/bash
# Test driver for video files

source ./functions.source

echo "Creating temporary directory modifiedvideo to test Video Metadata Write functionality" 

mkdir $testdir/$datadir/video/modifiedvideo 

videos=$testdir/$datadir/video/modifiedvideo/video*

#copy all test Video files to temporary Directory
cp $testdir/$datadir/video/video*.avi $testdir/$datadir/video/modifiedvideo -rf
cp $testdir/$datadir/video/video*.mkv $testdir/$datadir/video/modifiedvideo -rf
cp $testdir/$datadir/video/video*.mp4 $testdir/$datadir/video/modifiedvideo -rf
cp $testdir/$datadir/video/video*.wmv $testdir/$datadir/video/modifiedvideo -rf
cp $testdir/$datadir/video/video*.asf $testdir/$datadir/video/modifiedvideo -rf

#Write all Metadata to the files
echo "Writing Metadata To the RIFF and Matroska video Files.."

$bin/exiv2 -M "set Xmp.video.MicroSecPerFrame 64" $videos
$bin/exiv2 -M "set Xmp.video.MaxDataRate 4096" $videos
$bin/exiv2 -M "set Xmp.video.FrameCount 2048" $videos
$bin/exiv2 -M "set Xmp.video.InitialFrames 4" $videos
$bin/exiv2 -M "set Xmp.video.StreamCount 2" $videos
$bin/exiv2 -M "set Xmp.video.SuggestedBufferSize 1024" $videos
$bin/exiv2 -M "set Xmp.video.Width 240" $videos
$bin/exiv2 -M "set Xmp.video.Height 320" $videos
$bin/exiv2 -M "set Xmp.video.FileDataRate 128" $videos
$bin/exiv2 -M "set Xmp.video.Duration 2048" $videos
$bin/exiv2 -M "set Xmp.video.Codec mjpg" $videos
$bin/exiv2 -M "set Xmp.video.FrameRate 1024" $videos
$bin/exiv2 -M "set Xmp.video.VideoQuality 128" $videos
$bin/exiv2 -M "set Xmp.video.VideoSampleSize 256" $videos
$bin/exiv2 -M "set Xmp.audio.Codec mpv4" $videos
$bin/exiv2 -M "set Xmp.audio.SampleRate 32" $videos
$bin/exiv2 -M "set Xmp.audio.SampleCount 32" $videos
$bin/exiv2 -M "set Xmp.video.DateUT $date" $videos
$bin/exiv2 -M "set Xmp.video.Comment Metadata was Edited Using Exiv2" $videos
$bin/exiv2 -M "set Xmp.video.Language Kannada" $videos
$bin/exiv2 -M "set Xmp.video.Country India" $videos
$bin/exiv2 -M "set Xmp.video.Copyright Photographer" $videos
$bin/exiv2 -M "set Xmp.video.Genre Sample Test Video" $videos
$bin/exiv2 -M "set Xmp.video.Software Exiv2 0.25" $videos
$bin/exiv2 -M "set Xmp.video.Junk Its a junk Data" $videos
$bin/exiv2 -M "set Xmp.video.MediaLanguage English" $videos

echo "Metadata has been written to the File"