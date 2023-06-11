# 1. 执行函数 produce 返回一个协程
# 2. yield value 返回调用者 
# 3. next(producer) 切换到协程 producer
# 4. produce 结束后，raise StopIteration 

def produce():
    for i in range(3):
        print('produce', i)
        yield i

def consume():
    producer = produce()
    for i in range(3):
        item = next(producer)
        print('  consume', item)

consume()
