#!/usr/bin/env python3

import os

print("Content-Type: text/html")
print()

print("<html><body><table border='1'>")

for k, v in sorted(os.environ.items()):
    print(f"<tr><th>{k}</th><td>{v}</td></tr>")

print("</table></body></html>")