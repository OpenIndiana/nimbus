#! /bin/bash
# $1 is the searched pattern $2 is the pattern to be changed $3 is the replacement pattern

for file in `find . -name "$1*"`
#for file in `find . -name "network.*"  | grep -v actions`
do
  new_name=`echo $file | sed -e "s/$2/$3/g"`
  echo "moving $file to $new_name"
  mv $file $new_name
done
