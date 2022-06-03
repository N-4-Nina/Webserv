#!/usr/bin/python

import os

protocol = os.environ.get('SERVER_PROTOCOL')
status = os.environ.get('REDIRECT_STATUS')
print (protocol + ' ' + status + ' OK')
print ('Content-Type: ' + 'text/html')
print ('')

print ('<html>')
print ('<head>')
print ('<title>Hello World - First CGI Program</title>')
print ('</head>')
print ('<body>')

print ('<h1>Hello World! This is a seconde python script</h1>')


print ('</body>')
print ('</html>')
