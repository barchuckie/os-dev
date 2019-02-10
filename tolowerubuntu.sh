#!/bin/bash
#Author: Patryk Barczak

for file in *; do
    if [ -f "$file" ]; then #check if it is a file
        newname=$(echo "$file" | sed 's/.*/\L&/g')
        if [ "$file" = "$newname" ] || [ -e "$newname" ]; then  #avoids overwritting existing files
            continue
        fi
        mv -- "$file" "$newname"
    fi
done
