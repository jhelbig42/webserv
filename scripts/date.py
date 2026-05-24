#!/usr/bin/python3

from datetime import datetime

import sys

print('This is the script running', file=sys.stderr)
print("Content-Type: text/plain")
print()
print(datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
