from greenlet import greenlet

def producer():
    for i in range(0, 8):
        # 生成数据 a b c d e f g h
        data = chr(ord('a') + i)
        print('producer: ', data)
        calculator_gr.switch(data)

def calculator(data):
    while data <= 'h':
        res = data.upper()
        print('\tcalculator: ', res)
        data = consumer_gr.switch(res)

def consumer(res):
    while res <= 'H':
        print('\t\tconsumer: ', res)
        res = producer_gr.switch()


producer_gr = greenlet(producer)
calculator_gr = greenlet(calculator)
consumer_gr = greenlet(consumer)

producer_gr.switch()
