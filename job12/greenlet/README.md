# python 中的协程

## 0. 说明

1. 本节课学习 python 中的协程的概念
2. 下节课讲解如何使用 C 语言实现协程

完成如下两个作业

1. pc.py
+ 系统中有2个协程：生产者、消费者
+ 生产者生产'a'、'b'、'c'、‘d'、'e'、'f'、'g'、'h'八个字符
+ 消费者获得生产者传递的数据，并将其打印到屏幕上
+ 此题无需使用 buffer，使用协程切换实现

2. pcc.py
+ 系统中有3个协程：生产者、计算者、消费者
+ 生产者生产'a'、'b'、'c'、‘d'、'e'、'f'、'g'、'h'八个字符
+ 计算者从生产者取出字符，将小写字符转换为大写字符，发送给消费者
+ 消费者从计算者取出字符，将其打印到屏幕上
+ 此题无需使用 buffer，使用协程切换实现

## 1. 协程概述

协程（Coroutine）是一种用户态的轻量级线程，也被称为“微线程”或“纤程”。与操作系统线程不同，协程由程序员自己控制调度，避免了线程切换所带来的开销。协程可以在一个线程中实现多任务并发执行，使得编写高效的并发程序变得更加容易。

在协程中，程序不会像传统的函数调用那样从头到尾执行完毕，而是可以在执行过程中暂停、保存当前状态，等待下一次激活时再继续执行。这种特性使得协程可以在多个任务之间切换，实现异步编程和协作式多任务处理，适用于诸如网络编程、异步IO、事件驱动等场景。

## 2. greenlet 概述

Python Greenlet是一种协程库，提供了比Python标准库中的线程更轻量级和更高效的并发处理方式。Greenlet库允许开发者在应用程序中使用协程来处理并发任务，这些协程可以像函数调用一样控制任务的执行流程，并且可以在运行时动态地挂起和恢复，避免了线程切换的开销。

Greenlet库的核心是一个叫做“greenlet”的对象，它代表了一个协程。每个协程都有自己的状态和栈空间，但是它们共享进程的内存空间。Greenlet库允许开发者通过创建多个协程并在它们之间进行切换来实现并发处理。

相对于其他并发处理方式（如线程、进程等），Greenlet库具有以下优点：

资源占用更少：由于协程是轻量级的，每个协程只需要很少的内存和CPU资源，因此可以在同一进程中创建大量的协程，从而提高系统的并发处理能力。

线程安全：由于Greenlet库是单线程的，不存在多线程竞争的问题，因此不需要像多线程编程那样考虑线程安全问题，简化了编程难度。

更容易编写和维护：相对于多线程编程，使用协程编写的代码更加简洁、优雅，并且容易维护。

## 3. greenlet API

+ 安装
  - sudo apt install python3
    * 安装 python3
  - pip3 install greenlet
    * 使用 python3 的 pip3 包安装器安装 greenlet

+ id = greenlet(function)
  - 创建一个协程
  - 入口函数为 function
  - 返回该协程的 id

+ target.switch()
  - 从当前协程切换到目标协程 target

+ target.switch(value)
  - 从当前协程切换到目标协程 target
  - 并向目标协程传递参数
  - 请参考例子 a0.py a1.py a2.py

## 4. 手工切换协程

Greenlet库中的switch函数是用来在不同的协程之间进行切换的。每个协程都有一个相应的Greenlet对象，当一个协程需要切换到另一个协程时，就可以通过切换它们对应的Greenlet对象实现。

具体来说，switch函数接收一个参数，表示要切换到的目标Greenlet对象，然后将当前协程挂起，并切换到指定的目标协程中执行。同时，目标协程会从上次调用switch函数的位置开始执行，伪装成当前协程继续执行。这种方式避免了线程切换的开销，提高了并发处理的效率。

## 5. 例子 1: sw.py 

在程序中定义了两个函数 test1() 和 test2()，它们都包含了打印语句和用于切换到另一个协程的 greenlet.switch() 方法。

接下来，在程序中创建了两个 greenlet 对象 gr1 和 gr2，并将 test1() 函数和 test2() 函数分别传递给它们。

然后，通过调用 gr1.switch()，开始执行 test1() 函数。该函数首先打印数字 12，然后通过调用 gr2.switch()，切换到 gr2 对应的 test2() 函数中。

test2() 函数会打印数字 56，然后再次切换回 gr1 对应的 test1() 函数。

test1() 函数继续执行并打印数字 34，然后再次通过 gr2.switch() 切换回 test2() 函数。

最后，test2() 函数打印数字 78，整个程序结束。

```python
from greenlet import greenlet

def test1():
    print(12)
    gr2.switch()
    print(34)
    gr2.switch()

def test2():
    print(56)
    gr1.switch()
    print(78)

gr1 = greenlet(test1)
gr2 = greenlet(test2)
gr1.switch()
```

运行程序: 
+ 在 linux 下，有 2 个版本的 python: python 和 python3
+ 使用 python3 而不是 python 运行 sw.py

```
$ python3 sw.py
```

程序输出
```
12
56
34
78
```

## 6. 在协程之间传递参数

target.switch(param) 切换到目标协程，并向目标协程传递参数 param：

1. 如果目标协程还没有启动
   - 跳转到目标协程
   - 则将 param 作为目标协程入口函数的参数
   - 请参考例子 a0.py

2. 如果目标协程已经启动
   - 则目标协程必然位于某一条执行 x.switch() 的调用中
   - 跳转到目标协程
     * 目标协程从 x.switch() 中返回
     * 将 param 作为 x.switch() 的返回值
   - 请参考例子 a1.py a2.py

## 7. 例子 a0.py

在程序中定义了一个函数 test(x,y)，它包含了打印语句，和两个参数 x 和 y。

接下来，在程序中创建了一个 greenlet 对象 gr，并将 test() 函数传递给它。

然后，通过调用 gr.switch("hello", "world")，开始执行 test() 函数。

该函数会打印字符串 "test1" 以及传入的两个参数 x 和 y 的值，即 "hello" 和 "world"。

```python
from greenlet import greenlet

def test(x, y):
    print('test1', x, y)

gr = greenlet(test)
gr.switch("hello", "world")
```

运行程序
```
$ python3 a0.py
```

程序输出
```
test hello world
```

## 8. 例子 a1.py

它定义了两个函数 ping() 和 pong()，并创建了两个 greenlet 对象 ping_gr 和 pong_gr 分别用于执行这两个函数。

在主程序中，首先通过调用 ping_gr.switch(1) 来切换到 ping() 函数，并将参数 1 传递给它。

ping() 函数会打印字符串 "ping" 和传入的参数 number 的值，然后调用 pong_gr.switch(number + 1) 方法，将 number+1 作为参数切换到 pong() 函数。

pong() 函数会打印字符串 "pong" 和传入的参数 number 的值。

```python
from greenlet import greenlet

def ping(number):
    print('ping', number)
    pong_gr.switch(number + 1)

def pong(number):
    print('pong', number)

ping_gr = greenlet(ping)
pong_gr = greenlet(pong)
ping_gr.switch(1)
```

运行程序
```
$ python3 a1.py
```

程序输出
```
ping 1
pong 2
```

## 9. 例子 a2.py

它定义了两个函数 ping() 和 pong()，并创建了两个 greenlet 对象 ping_gr 和 pong_gr 分别用于执行这两个函数。

在主程序中，首先通过调用 ping_gr.switch(1) 来切换到 ping() 函数，并将参数 1 传递给它。

ping() 函数会打印字符串 "ping" 和传入的参数 number 的值，然后调用 pong_gr.switch(number + 1) 方法，将 number+1 作为参数切换到 pong() 函数。

pong() 函数会打印字符串 "pong" 和传入的参数 number 的值，然后调用 ping_gr.switch(number + 1) 方法，将 number+1 作为参数再次切换回 ping() 函数。

当从 pong() 函数调用 ping_gr.switch(number + 1) 切换回 ping() 函数时，pong 协程将参数 number + 1传递给 ping 协程，**此时 ping 协程正处于 number = pong_gr.switch(number + 1)**，通过函数的返回值获取 ping 协程的参数。

即 ping() 函数会将返回值赋值给 number 变量，然后再次打印字符串 "ping" 和新的 number 值，并打印字符串 "end" 表示当前协程已经执行完毕

```python
from greenlet import greenlet

def ping(number):
    print('ping', number)
    number = pong_gr.switch(number + 1)
    print('ping', number)
    print('end')

def pong(number):
    print('pong', number)
    ping_gr.switch(number + 1)

ping_gr = greenlet(ping)
pong_gr = greenlet(pong)
ping_gr.switch(1)
```

运行程序
```
$ python3 a2.py
```

程序输出
```
ping 1
pong 2
ping 3
end
```

