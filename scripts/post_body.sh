#!/bin/bash

POST_DIR="/home/jhelbig/Desktop/webserv/post"

mkdir -p "$POST_DIR"

FILENAME="$POST_DIR/$(date +%s%N)_$$.txt"

dd bs=1 count="$CONTENT_LENGTH" 2>/dev/null > "$FILENAME"

echo "Content-Type: text/plain"
echo ""
echo "written to: $FILENAME"
