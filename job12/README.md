# job12

## 利用 python 中的 greenlet 实现协程

### 作业详情
1. pc.py
- 系统中有2个协程：生产者、消费者
- 生产者生产'a'、'b'、'c'、‘d'、'e'、'f'、'g'、'h'八个字符
- 消费者获得生产者传递的数据，并将其打印到屏幕上
- 此题无需使用 buffer，使用协程切换实现


2. pcc.py
- 系统中有3个协程：生产者、计算者、消费者
- 生产者生产'a'、'b'、'c'、‘d'、'e'、'f'、'g'、'h'八个字符
- 计算者从生产者取出字符，将小写字符转换为大写字符，发送给消费者
- 消费者从计算者取出字符，将其打印到屏幕上
- 此题无需使用 buffer，使用协程切换实现