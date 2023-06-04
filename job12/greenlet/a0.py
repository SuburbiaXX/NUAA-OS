from greenlet import greenlet

def test(x, y):
    print('test', x, y)

gr = greenlet(test)
gr.switch("hello", "world")
