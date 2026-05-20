#!/bin/sh
# This is run once a day - don't care about performance
# RFC date must have GMT in the end
# but GNU date return +0000 instead and bb date returns UTC
# So we must strip +0000 or UTC and add GMT ourselves
DATE=$(date -R -u | sed 's/\UTC/GMT/' | sed 's/\+0000/GMT/')
echo "Date: $DATE\nStatus: 200\n";