#!/usr/bin/env python
import os
print "Content-Type: text/html\n"
print "<html><body><table border='1'>"
for k,v in sorted(os.environ.iteritems()):
  print "<tr><th>%s</th><td>%s</td></tr>" % (k,v)
print "</table></body></html>"