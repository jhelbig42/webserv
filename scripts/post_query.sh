#!/bin/bash

POST_DIR="/home/julia/projects/webserv/post"

mkdir -p "$POST_DIR"

FILENAME="$POST_DIR/$(date +%s%N)_$$.txt"

echo "$QUERY_STRING" > "$FILENAME"

echo "Content-Type: text/plain"
echo ""
echo "written to: $FILENAME"

