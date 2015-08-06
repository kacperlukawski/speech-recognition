#!/bin/bash

file_to_split=$1
description_file=$2
destination_dir=$3
silence_duration=$4

if [ -z "$file_to_split" ] || [ -z "$description_file" ] || [ -z "$destination_dir" ]
then
  echo "ERROR: Incorrect arguments"
  echo "Syntax: ./split_records.sh file_to_split description_file destination_dir [silence_duration=0.5]"
  echo "Example: ./split_records.sh ./records/record_22.wav ./variants/words_22.txt ./test 0.01"
  echo "To run this script you have to install sox (sudo apt-get install sox)"
else
  if [ -z "$silence_duration" ]
  then
    silence_duration=0.5
  else
    reg_exp='^[0-9]+([.][0-9]+)?$'
    if ! [[ $silence_duration =~ $reg_exp ]] 
    then
      echo "ERROR: Variable silence_duration must be a number.";
      exit 1;
    fi
  fi

  tmp="$destination_dir/tmp_.wav"
  sox $file_to_split $tmp silence 1 0.5 1% 1 $silence_duration 1% : newfile : restart
 
  words=$(head -n 1 $description_file)
  wordsArr=(${words//,/ })
  
  index=1
  for i in "${wordsArr[@]}"
  do
    echo $index $i
    mv "$destination_dir/tmp_`printf %03d $index`.wav" "$destination_dir/$i.wav"
    ((++index))
  done
fi
