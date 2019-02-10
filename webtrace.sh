#!/bin/bash

website=$1
lynx -dump $website > newtrace.txt
cp newtrace.txt oldtrace.txt

if [[ ! -e .git ]]; then
    git init
fi
git rm newtrace.txt
git add newtrace.txt
git commit -m 'Initial trace'

sleep $2
i=1
while [[ true ]]; do
    lynx -dump $website > newtrace.txt
    if [[ `diff -q newtrace.txt oldtrace.txt` ]]; then
        zenity --notification --text="There is a change on the website!"
        git add newtrace.txt
        git commit -m "Website update no. $i"
        ((i++))
        cp newtrace.txt oldtrace.txt
    else
        echo Brak zmian
    fi

    sleep $2
done
