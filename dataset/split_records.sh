#!/bin/bash

file_to_split=$1
description_file=$2
destination_dir=$3
duration=$4
silence_threshold=$5

if [ -z "$file_to_split" ] || [ -z "$description_file" ] || [ -z "$destination_dir" ]
then
  echo "ERROR: Incorrect arguments"
  echo "Syntax: ./split_records.sh file_to_split description_file destination_dir [duration=0.5 silence_threshold=0.3]"
  echo "Example: ./split_records.sh ./records/record_22.wav ./variants/words_22.txt ./test 0.01"
  echo "To run this script you have to install sox (sudo apt-get install sox)"
else
  if [ -z "$duration" ]
  then
    duration=0.5
  else
    reg_exp='^[0-9]+([.][0-9]+)?$'
    if ! [[ $duration =~ $reg_exp ]] 
    then
      echo "ERROR: Variable duration must be a number.";
      exit 1;
    fi
  fi
  if [ -z "$silence_threshold" ]
  then
    silence_threshold=0.5
  else
    reg_exp='^[0-9]+([.][0-9]+)?$'
    if ! [[ $silence_threshold =~ $reg_exp ]] 
    then
      echo "ERROR: Variable silence_threshold must be a number.";
      exit 1;
    fi
  fi

  tmp="$destination_dir/tmp_.wav"
  
  # TODO Tu parametry sa zle przekazywane! Ale dziwnym trafem dla nagrania 11 byly optymalne, wiec zostawiam
  sox $file_to_split $tmp silence 1 $silence_threshold 1% 1 $duration 1% : newfile : restart
  # TODO Tu juz parametry lepiej wygladaja, ale w ogole ich dobrac dobrze nie moge... 
  # trzeba to zgrac z dokumentacja http://sox.sourceforge.net/sox.html#OPTIONS silence
  #sox $file_to_split $tmp silence 1 $duration $silence_threshold% 1 $duration $silence_threshold% : newfile : restart
 
  words=$(head -n 1 $description_file)
  IFS=',' read -a wordsArr <<< "$words"

  index=1
  for i in "${wordsArr[@]}"
  do
    trim=$( echo "$i" | sed -e "s/^\ *//g" -e "s/\ *$//g")
    echo "$index $trim"
    mv "$destination_dir/tmp_`printf %03d $index`.wav" "$destination_dir/$trim.wav"
    ((++index))
  done
fi
