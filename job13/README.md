# 实际上是 job13
## 使用 C 语言协程

### 1. 对称协程

+ 只提供一个协程的切换操作
  - switch(target)
+ 可以从当前协程，切换到任意的目标协程 target 

### 2. 非对称协程

+ 提供两个协程的切换操作
  - value = resume(target)
  - yield(value)

+ 涉及到两个协程
  - 调用者 caller
    * 执行 resume 操作恢复 callee 的执行
  - 被调用者 callee
    * 执行 yield 操作返回到 caller

### 3. 两者的区别

+ switch(target) 可以切换到任意的目标协程
+ yield(value) 只能切换回 caller
+ resume(target) 时
  - 需要在 target 中，记录下 caller
  - yield(value) 时，切换回 caller 

### 4. 阅读 pc.py

+ 阅读当前目录下的文件 pc.py
+ python 从语法程序支持半对称协程

### 5. 作业 pcc.py

+ 放在当前目录下
+ 要求输出如下：

```
produce 0
  covert 0 -> 0
      consume 0
produce 1
  covert 1 -> 10
      consume 10
produce 2
  covert 2 -> 20
      consume 20
```

### 6. 阅读 pc.c

1. void coro_boot(coro_t *that_coro);
+ 启动第一个协程
+ 从主函数中跳转到第一个协程
+ 之后，程序在协程之间切换

2. coro_t *coro_new(void (*entry)());
+ 新创建一个协程，入口函数为 entry

3. void coro_delete(coro_t *coro);
+ 删除协程

4. unsigned long coro_resume(coro_t *that_coro);
+ 跳转到 callee 协程
+ 接收来自 callee 的返回值 

5. void coro_yield(unsigned long value);
+ 返回 caller 协程
+ 将值 value 传递给 caller

### 7. 作业 pfc.c

要求输出如下：

```
                CONSUME
        FILTER
PRODUCE
produce 0
        filter 0 -> 0
                consume 0
produce 1
        filter 1 -> 10
                consume 10
produce 2
        filter 2 -> 20
                consume 20
produce 3
        filter 3 -> 30
                consume 30
produce 4
        filter 4 -> 40
                consume 40
produce 5
        filter 5 -> 50
                consume 50
produce 6
        filter 6 -> 60
                consume 60
produce 7
        filter 7 -> 70
                consume 70
                END
```

### 8. 作业 find.c grep.c 

1. 在文件 fs_walk.c 编写中编写函数 fs_walk
   + fs_walk 是一个生产者协程
   + 遍历指定目录下所有的目录项
   + 使用 co_yield 函数向消费者协程传递目录项

2. 在文件 find.c 中实现 find 程序的功能
   + 引用 fs_walk.c 
   + 编写一个消费者协程，获取 fs_walk 传递的目录项
   + 打印符合要求的目录项 

```
在目录 /usr/include 下查找文件 stdio.h
$ find /usr/include -name stdio.h
/usr/include/stdio.h
/usr/include/c++/7/tr1/stdio.h
/usr/include/x86_64-linux-gnu/bits/stdio.h
```

3. 在文件 grep.c 中实现 grep 程序的功能
   + 引用 fs_walk.c 
   + 编写一个消费者协程，获取 fs_walk 传递的目录项
   + 检索指定的字符串 

```
在目录 /usr/include 下查找包含有字符串 linux 的文件
$ grep -r linux /usr/include
/usr/include/selinux/selinux.h:extern int is_selinux_e
nabled(void);
/usr/include/selinux/selinux.h:extern int is_selinux_m
ls_enabled(void);
/usr/include/selinux/selinux.h:/* Wrappers for the sel
inuxfs (policy) API. */
```

```

