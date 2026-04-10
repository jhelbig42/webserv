
import os
import sys

# 1. CGI Headers
print("Content-Type: text/html")
print("Status: 200 OK")
print("") 

# 2. The HTML Body
print("<html>")
print("<head><title>CGI Query String Test</title></head>")
print("<body>")
print("<h1>CGI Environment Test</h1>")

# 3. Accessing the Query String
# In CGI, the query string is found in the environment variables.
query_string = os.environ.get('QUERY_STRING', 'No Query String Found')

print(f"<p><strong>Raw Query String:</strong> {query_string}</p>")

# 4. Parsing the Query String (The '1996' Manual Way)
print("<h3>Parsed Parameters:</h3>")
print("<ul>")
if query_string:
    pairs = query_string.split('&')
    for pair in pairs:
        if '=' in pair:
            key, value = pair.split('=', 1)
            print(f"<li><strong>{key}:</strong> {value}</li>")
        else:
            print(f"<li>{pair} (No value)</li>")
else:
    print("<li>No parameters provided.</li>")
print("</ul>")

# 5. Helpful Debug Info
print("<h3>Other CGI Environment Variables:</h3>")
print("<pre>")
for key in ['REQUEST_METHOD', 'PATH_INFO', 'SCRIPT_NAME', 'REMOTE_ADDR']:
    print(f"{key}: {os.environ.get(key, 'Not Set')}")
print("</pre>")

print("</body>")
print("</html>")