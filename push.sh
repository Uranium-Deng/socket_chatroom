#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 comment_message"
    exit
fi

git add .
git commit -m $1
git push

