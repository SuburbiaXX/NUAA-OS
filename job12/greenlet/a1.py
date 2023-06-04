from greenlet import greenlet

def ping(number):
    print('ping', number)
    pong_gr.switch(number + 1)

def pong(number):
    print('pong', number)

ping_gr = greenlet(ping)
pong_gr = greenlet(pong)
ping_gr.switch(1)
