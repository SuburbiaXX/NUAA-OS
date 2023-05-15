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
age = dict['age']
gender = dict['gender']

f = open('student.txt', 'a')
line = '%s\t%s\t%s\n' % (name, age, gender)
f.write(line)
f.close()

print('<h1>Add student %s OK</h1>' % name)
print('''
<script>
setTimeout(function() {
    location.href = '/app/list_student';
}, 1000);
</script>
''')
