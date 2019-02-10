#!/usr/bin/env bash
#Author: Patryk Barczak
#MAC OS is case insensitive in filenames

for file in ./*; do
    if [[ -f "$file" ]]; then   #check if it is a file
        newname=$(echo "$file" | tr '[:upper:]' '[:lower:]')
        if [[ "$file" != "$newname" ]]; then #otherwise nothing to do
            mv -- "$file" "$newname"
        fi
    fi
done
