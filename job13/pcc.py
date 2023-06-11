# 根据作业要求, 应该是在中间添加一个转换的函数
# produce 0
#   covert 0 -> 0
#       consume 0
# produce 1
#   covert 1 -> 10
#       consume 10
# produce 2
#   covert 2 -> 20
#       consume 20


def produce():
    convertor = covert()
    for i in range(3):
        print('produce', i)
        solved = next(convertor)
        yield solved

def covert():
    for i in range(3):
        print('\tcovert', i, '->', i * 10)
        yield i * 10

def consume():
    producer = produce()
    for i in range(3):
        item = next(producer)
        print('\t\tconsume', item)

consume()
