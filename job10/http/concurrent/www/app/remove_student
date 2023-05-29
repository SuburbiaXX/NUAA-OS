#!/usr/bin/python3
import os

# name=tom&age=11&gender=male

query_string = os.getenv('QUERY_STRING')
query_list = query_string.split('&')

dict = {}
for query in query_list:
    key, value = query.split('=')
    dict[key] = value

name = dict['name']

lines = []
f = open('student.txt', 'r')
for line in f:
    fields = line.split()
    if name != fields[0]:
        lines.append(line)
f.close()

f = open('student.txt', 'w')
for line in lines:
    f.write(line)
f.close()

print('<h1>Remove student %s OK</h1>' % name)
print('''
<script>
setTimeout(function() {
    location.href = '/app/list_student';
}, 1000);
</script>
''')
