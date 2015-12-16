#!/bin/bash
cd /home/zhongcong/video/intel
num=$(ls -l | grep "^-" | wc -l)
den=10

for (( c=1; c<10; c++))
do
  ((num=$num/$den))
  echo $num
  if test $num -eq 0
  then
    break
  fi
done

file_dir="/home/zhongcong/video/intel/%"$c"d.jpg"
echo "file_dir : " $file_dir
#ffmpeg -f image2 -i $file_dir -r 25  -vf scale=iw/2:ih/2 video.mp4
ffmpeg -f image2 -i $file_dir -r 25  video.mp4

