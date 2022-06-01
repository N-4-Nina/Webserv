#!/usr/bin/python

import os
from datetime import date

protocol = os.environ.get('SERVER_PROTOCOL')
status = os.environ.get('REDIRECT_STATUS')
content_type = os.environ.get('CONTENT_TYPE')

print (protocol + ' ' + status + ' OK')
#print ('Content-Length ' + )
print ('Content-Type: ' + content_type)

print ('<html>')
print ('<head>')
print ('<title>Hello World - First CGI Program</title>')
print ('</head>')
print ('<body>')

print ('<h1>Hello World! This is a python script</h1>')

# Read the current date
current_date = date.today()

# Print the formatted date
print ("<h3>Today is: %d/%d/%d" % (current_date.day,current_date.month,current_date.year))
print ("- have a good day</h3>")
print ("<hr>")

# print ('<p>I am just a little python script uwu</p>')
print ('<p>Here, check the environement variables of the CGI, pass with execve() (c++):</p>')
print ("<ul>")
for key in os.environ.keys():
    print ("<li><span style='color:green'>%30s:</span> %s </li>" % (key,os.environ[key]))
print ("</ul>")

params = os.environ.get('QUERY_STRING')
res = params.split('=') #TESTS THAT THERE IS A QUERY STRING

#display based on if params were sent
if  str(res) == "['']":
	print ('no query params')
	print ('<br>')
else:
	print ('some query params were passed:')
	print ('<br>')
    #searchParams is an array of type [['key','value'],['key','value']]	
	searchParams = [i.split('=') for i in params.split('&')] #parse query string
	for key, value in searchParams:
		print('<b>' + key + '</b>: ' + value + '<br>\n')	

print ('</body>')
print ('</html>')
