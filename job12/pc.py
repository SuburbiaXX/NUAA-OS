from greenlet import greenlet

def producer():
    for i in range(0, 8):
        # 生成数据 a b c d e f g h
        data = chr(ord('a') + i)
        print('producer: ', data)
        consumer_gr.switch(data)

def consumer(res):
    while res <= 'h':
        print('\tconsumer: ', res)
        res = producer_gr.switch()


producer_gr = greenlet(producer)
consumer_gr = greenlet(consumer)

producer_gr.switch()
