#!usr/bin/python
import os

print ('<html>')
print ('<head>')
print ('<title>Hello World - First CGI Program</title>')
print ('</head>')
print ('<body>')

print ('<h1>Hello World! This is my first CGI program</h1>')
print ('<h2>Hello World! This is my first CGI program</h2>')
print ('<h3>Hello World! This is my first CGI program</h3>')
print ('<h4>Hello World! This is my first CGI program</h4>')
print ('<h5>Hello World! This is my first CGI program</h5>')
# print ('<p>I am just a little python script uwu</p>')
print ('<p>Here, check the environement variables of the CGI, pass with execve() (c++):</p>')

for i, j in os.environ.items():
    print(i, j)
    print ('<br>')

# method to get/print the params stored in the CGI variables, QUERY_STRING
# query = os.environ.get("QUERY_STRING")
# print ('<br>')
# print ('We are the arguments of the QUERY_STRING: ')
# print (query)

print ('</body>')
print ('</html>')
