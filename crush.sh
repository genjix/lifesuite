#!/bin/sh
cd dump/
find . | while read file
do
    ../viewer "$file" "../crush/$file.bmp"
    mv "$file" ../process/
done
