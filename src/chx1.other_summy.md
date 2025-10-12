# 单片机技术总结说明(x1) 其它功能总结

- [嵌入式编译hex文件代码分析](#feature-1)
- [嵌入式系统中的对齐机制](#feature-2)
- [嵌入式平台基本语法知识](#feature-3)

## feature-1

嵌入式编译hex文件代码分析。

HEX是单片机保存编译结果文件的一种方式，常见的还有axf和bin，其中HEX是基于ASCII保存的格式，具体结构如下。

```shell
: | LL | aaaa | TT | D..D | CC
字段含义
:       起始字段，hex都是以':'开头
LL      2个字节，表示后续数据段的长度
aaaa    地址域，表示数据再段内的偏移地址
TT      记录数据的类型
    00 - 表示内容为保存数据
    01 - 表示文件结束
    02 - 表示内容为扩展段起始地址
    03 - 表示内容为开始段起始地址
    04 - 表示内容为扩展线性起始地址
    05 - 表示内容为扩开始线性地址
D...D   数据域，保存的数据字节
CC      校验和
```

按照格式，分解的数据数据格式如下。

```c
:020000040800F2
- 02：数据长度(2字节)
- 0000：段内偏移地址，此数据内没有含义
- 04：数据段为扩展线性起始地址
- 0800：表示后续数据的起始地址，0x08000000
- F2：校验和

:1000000028AB00205103000851A300084D8A0008C6
- 10：数据长度(16字节)
- 0000：段内偏移地址，表示起始地址偏移值0x0000, 实际地址为扩展线性起始地址+段内偏移=0x08000000
- 00：表示为保存数据，即生成的结果数据
- 28AB0020 51030008 51A30008 4D8A0008：实际数据，小端模式
- C6：校验和 
```

使用python将hex转换成bin的方法如下所示。

```py
#!/usr/bin/python3

import os
import sys
import six
from struct import *

class Hex_Type(int):
    DATA_TYPE = 0x00,
    FILE_END  = 0x01,
    EXTEND_SEGMENT = 0x02,
    START_SEGMENT_ADDR  = 0x03,
    EXTEND_LINE_ADDR = 0x04,
    EXTEND_START_SEGMENT = 0x05,

class HexConvertBin:
    def __init__(self, hexfile: str, binfile: str):
        self.hexfile_ = hexfile
        self.binfile_ = binfile
        self.begin_addr_ = 0
        self.high_addr_ = 0
        self.write_size_ = 0
        self.is_first_data_line_ = True

    def convert(self):
        fin = open(self.hexfile_, 'r')
        fout = open(self.binfile_, 'wb')
        result = []

        for linestr in fin.readlines():
            linestr = linestr.strip()
            self.size_ = int(linestr[1:3], 16)
            self.type_ = Hex_Type(int(linestr[7:9], 16))

            #获取高位地址
            if self.type_ == Hex_Type.EXTEND_LINE_ADDR[0]:
                self.high_addr_ = (int(linestr[9:11], 16)<<8) + int(linestr[11:13], 16)
                print("{0} {1} {2}".format(self.size_, self.type_, self.high_addr_))
            
            #处理写入数据
            elif self.type_ == Hex_Type.DATA_TYPE[0]:
                self.offset_ = (int(linestr[3:5], 16)<<8) + int(linestr[5:7], 16)
                self.now_addr_ = self.high_addr_<<16 | self.offset_

                if self.is_first_data_line_ == True:
                    self.is_first_data_line_ = False
                    self.begin_addr_ = self.high_addr_<<16 | self.offset_
                
                #偏移值高，表示中间有空白，填充0
                offset_zero = self.now_addr_ - self.begin_addr_ - self.write_size_
                
                if offset_zero > 0:
                    fout.write(b'\00' * offset_zero)
                    self.write_size_ += offset_zero
                elif offset_zero < 0:
                    print("not support for hex address no increase!")
                    return False

                for h in range(0, self.size_):
                    b = int(linestr[9+h*2:9+h*2+2], 16)
                    result.append(six.int2byte(b))
                fout.write(b''.join(result))
                self.write_size_ += self.size_
                result = []

        fin.close()
        fout.close()
        return True

if len(sys.argv) != 3:
    print('usage:')
    print('convert hexadecimal format to binary format:')
    print('fromehextobin.py hexfile binfile')
    exit(0)

obj = HexConvertBin(sys.argv[1], sys.argv[2])
obj.convert()

```

## feature-2

嵌入式系统中的对齐机制。

- __align(num)

相当于#pragma pack(num) ....... #pragma pack()，后者可用于所有数据类型。这个用于修改最高级别对象的字节边界。在汇编中使用LDRD或者STRD时就要用到此命令__align(8)进行修饰限制。来保证数据对象是相应对齐。这个修饰对象的命令最大是8个字节限制,可以让2字节的对象进行4字节 对齐,但是不能让4字节的对象2字节对齐。__align是存储类型修改,它只修饰最高级类型对象不能用于结构或者函数对象。

- __packed
  
__packed是进行1字节对齐  

对齐的注意点:

- 不能对packed的对象进行对齐  
- 所有对象的读写访问都进行非对齐访问  
- float及包含float的结构联合及未用__packed的对象将不能字节对齐  
- __packed对局部整形变量无影响  
- 强制由unpacked对象向packed对象转化是未定义,整形指针可以合法定义为packed。 __packed int* p;  //__packed int 则没有意义  
- 对齐或非对齐读写访问带来问题

```c
__packed struct STRUCT_TEST
{
    char a;
    int b;
    char c;
};    //定义如下结构此时b的起始地址一定是非对齐的
    //在栈中访问b可能有问题,因为栈上数据肯定是对齐访问[from CL]
```

- 非对齐访问表示每次只读取8bit，即1字节
  对齐访问则每次会读取对齐的量。

不同数据类型的对齐方式:

1. 数组 ：按照基本数据类型对齐，第一个对齐了后面的自然也就对齐了。
2. 联合 ：按其包含的长度最大的数据类型对齐。
3. 结构体： 结构体中每个数据类型都要对齐。

什么是自然对齐: 一个变量的起始内存地址正好是它长度的整数倍，就是自然对齐的

1. 基本数据类型的自身对齐值:  char 1; short 2;int 4;
2. 结构体或者类的自身对齐值:  其成员中自身对齐值最大的那个值
3. 指定对齐值:    __align(num)或者__packed 指定的对齐值
4. 数据成员、结构体和类的有效对齐值：自身对齐值和指定对齐值中小的那个值

结构体本身也要根据自身的有效对齐值圆整(就是结构体成员变量占用总长度需要是对结构体有效对齐值的整数倍，如果出现对齐或者赋值问题首先查看

1. 编译器的 big little 端设置
2. 看这种体系本身是否支持非对齐访问
3. 如果支持就查询设置了对齐与否,如果没有则查询访问时需要增加某些特殊的修饰来标识其特殊的访问操作

在设计不同 CPU 下的通信协议时，或者编写硬件驱动程序时寄存器的结构这两个地方都需要按一字节对齐。 即使看起来本来就自然对齐的也要使其对齐， 以免不同的编译器生成的代码不一样.

理解对齐，可以实验下面两个例子。

```c
stuct A
{
    char i;
    //reserved[3];
    int j;
    short k;
    //reserved[2];
}a;

struct B
{
    char i;
    //reserved[1];
    short j;
    int k;
}b;

#pragram pack(1)
struct C
{
    char i;
    int j;
    short k;
}c;
#pragram pack(0)

int main()
{
    cout<<sizeof(a);//值为12
    Cout<<sizeof(b);//值为8
    Cout<<sizeof(c);//值为7
}
```

上面说过，结构体自然对齐值为其内部最大的那个值,上述两个结构体都为4，要考虑到内部每个位的对齐值，则a占的空间为"1+3+4+2+2(为什么最后要加2呢，这是为了保证后续结构体变量的自然对齐）= 12"，b占的空间为"1+1+2+4 = 8"，C是定义结构体为非对齐访问，则中间就不会有空隙，即占空间为1+4+2=7，非对齐看似节省了时间，但是CPU取指令却要需要更多的次数，效率比较低，甚至有些cpu不支持非对齐访问，但是在不同cpu之间进行通讯或者数据传递时，必须使用非对齐数据，这是因为不同cpu之间的默认对齐方式有区别，可能出现传输完成后读取数据无法识别。

## feature-3

嵌入式平台基本语法知识。

### 基础数据类型

char    1字节
short   2字节
int     2/4字节 与系统或者编译器位数有关
float   4字节
Double  8字节
单片机堆栈指针的字节数 与系统有关，32位为4字节，64位为8字节

### 预编译指令的功能

```c
#include <file>    //包含函数库 
#define             //宏定义

#if ()              //如果，条件判断选择
#elif ()            //否则，条件判断选择
#else               //否则如果


#ifdef ()           //如果，定义判断选择
#ifndef ()          //否则，定义判断选择
#endif              //作为预定义的结尾
```

其实条件判断理解成if语句也没问题，他的好处是在编译前就做好了判断，减少了不必要的编译，用来优化和测试十分方便，这点在嵌入式中使用起来很普遍。

### 指针的用法

- 标识符

- 读取指针变量优先级规则()>[]>*;

```C
  int *f[10]
  f is array(10) of pointer to int//f是指向int指针的数组(元素10)
  int (*f)[10]
  f is pointer to array(10) of int//f是指向int数组(元素10）的指针
  int (*f)(double)
  f is pointer to function(double) returning int//f是指向返回int类型函数的指针
```

指针在嵌入式的使用中是很重要的一部分，特别是指向函数的指针，对于嵌入式实时操作系统，指向函数的指针应用正是实现多任务处理下压栈，出栈实现的基础，这一块也是指针方面的难点，需要熟练掌握。

### 用变量a给出下列定义

```C
整型:int a
指向整型的指针：int *a
指向指针的指针，它指向的指针指向一个整型：int **a
一个有10个整型的数组：int a[10];
一个有10个指针的数组，该指针指向一个整型变量: int *a[10]
一个指向有10个整型变量数组的指针：int (*a)[10]
一个指向函数的指针，该函数有一个整型参数并返回一个整型变量: int  (*a)(int)
一个有10个指针的数组,该指针指向一个函数，该函数有一个整型参数并返回一个整型数：int (*a[10])(int)
```

### #define的用法

- 利用#define定义一年多少秒钟

```c
#define SECOND_PER_YEAR     (unsigned long)(365*24*60*60)
```

- 利用#define定义宏来表示程序代码段

```c
//如定义一个标准宏，返回较小的值
#define  MIN(A,B)    ((A)<=(B)?(A):(B))
```

宏#define的这种运用主要方便移植，如把能够多次利用的底层硬件处理模块用宏定义，在移植时仅需要修改宏定义部分即可，极大地减少出错可能和时间消耗，宏定义的运用可能不影响当前软件的编写，但用的好的话会让以后的更新升级十分便利。

### static的作用

1. 声明静态数据变量，变量在函数调用过程中会保持上一次结束时的值
2. 在模块内部却在函数体外，声明为静态的变量或者函数只能被模块内的所有函数访问，也就是是说属于本地化数据和代码（重要）

### const的含义

const表示只读，避免误修改，增加程序的健壮性。在嵌入式中主要修饰不需要修改，仅被读取的存储在flash中变量数组。

### volatile用法

易变的，表示会被意想不到的改变
用途：1.并行设备的硬件寄存器(例如状态寄存器)
    2.一个中断服务历程访问的非自动变量
    3.多线程应用中共享的变量
一个变量可能即是const又是volatile吗？
可以的，如只读状态寄存器，const只读表示程序不应该试图修改它，但是它是volatile因为它可能被意想不到的改变

### 整型变量a，写两段代码，置位bit3和清除bit3

```c
#define  BIT3   (1<<3)
void set_bit3(void)
{
    a |= BIT3;
}
void reset_bit3(void)
{
    a&=~BIT3;
}
```

取补码或者反码时要考虑到编辑器的位数，以及基本数据类型在不同位数的编辑器中不一样，要善于用~和<<,>>等位变量。

### 设置一个绝对地址为0x67a9的整型变量的值为0xaa66

```c
int *ptr;
ptr = (int *)0x67a9;
*ptr = 0xaa66
或者
*(int * const)(0x67a9) = 0xaa66;
```

### ISR中断服务程序的注意点

1. ISR不能有返回值
2. ISR不能传递参数
3. 在许多编译器中，浮点数一般是不能重入得。即使允许,ISR中进行浮点运算效率很低
4. printf的不可重入，如果在中断函数中使用，需要进入临界区。

### 隐性的类型强制转换

如无符号数和负数相加，负数被强制转换成无符号数，结果就是一个很大的正数。 实际程序中不推荐使用，隐形的强制转换一般用于考试中迷惑学生，但在实际工作中，如果有隐形转换一定要在程序中加上强制的显性转换，避免程序出问题后很难查出错误。

### 内存分配问题

全局变量和静态变量：从静态存储区分配，内存在程序编译时分配并存在与整个程序周期中

1. 局部变量：从栈上分配，子函数调用时申请，结束后释放
2. 动态内存：从堆上分配，由程序员自己决定生存期，动态内存会产生内存碎片，比较好的减少方法就是申请->释放一一配对，在释放前不要申请新的动态内存。

### 大小端的判断

‌大端模式和小端模式的区别在于字节的存储顺序不同‌。大端模式（Big-Endian）中，**数据的高字节保存在内存的低地址中**，而数据的低字节保存在内存的高地址中，存储模式类似把数据当作字符串顺序处理。小端模式（Little-Endian）中，**数据的高字节保存在内存的高地址中**，而数据的低字节保存在内存的低地址中，存储模式将地址的高低和数据位权有效地结合起来

```c
#include <stdio.h>

union A
{
    unsigned int a;
    unsigned short b;  
};

int main(void) 
{
    //使用union进行判断
    A test;
    test.a = 0xFFFF0000;
    if (test.b == 0x0000) {
        printf("小端模式");
    } else {
        printf("大端模式");
    }

    //使用指针强制转换
    unsigned int val = 0xFFFF0000;
    unsigned short *pstr = (unsigned short *)&val;
    if (*pstr == 0x0000) {
        printf("小端模式, 0x%x", *pstr);
    } else {
        printf("大端模式, 0x%x", *pstr);
    }
}

```

## 下一章节

[返回目录](./../README.md)

直接开始下一小节: [系统分散加载机制](./chx2.distributed_loading.md)
