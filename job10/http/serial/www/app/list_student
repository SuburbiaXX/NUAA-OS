#!/usr/bin/python3

f = open('student.txt')
print('<h1><center>Student List</center></h1>');
print('<table border=1 cellspacing=0 width=100%>')

print('  <tr>')
print('    <td>name</td>')
print('    <td>age</td>')
print('    <td>gender</td>')
print('  </tr>')

for line in f:
    fields = line.split()
    print('  <tr>')
    for field in fields:
        print('    <td>%s</td>' % field)
    print('  </tr>')

print('</table>')
f.close()
