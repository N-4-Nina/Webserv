#!/usr/bin/python
import os
import time

n = 10

while n > 0:
    time.sleep(1)
    n -= 1

protocol = os.environ.get('SERVER_PROTOCOL')
status = os.environ.get('REDIRECT_STATUS')
#content_type = os.environ.get('CONTENT_TYPE')

print (protocol + ' ' + status + ' OK')
#print ('Content-Length ' + )
print ('Content-Type: ' + 'text/html')
print ('')

print ('<html>')
print ('<head>')
print ('<title>Hello World - Third CGI Program</title>')
print ('</head>')
print ('<body>')

print ('<p>\(=___=)/</p>')
print ('<p>hi... am waking up...</p>')

print ('</body>')
print ('</html>')
