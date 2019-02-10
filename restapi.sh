#!/bin/bash

download=`curl -s https://api.thecatapi.com/v1/images/search?format=json \
--header 'Content-Type: application/json' | jq -r '.[0].url'`

wget -q $download
image=`echo $download | awk -F '/' '{print $4}'`

img2txt $image

curl -s https://api.icndb.com/jokes/random | jq -r '.value.joke'

rm $image
