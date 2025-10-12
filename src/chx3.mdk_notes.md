# 单片机技术总结说明(x3) MDK相关编译方法说明

- [常用编译选项](#feature-1)
- [常用快捷键](#feature-2)
- [解决软件仿真状态下某个范围不可读的问题](#feature-3)
- [编译生成bin文件输出](#feature-4)
- [仿真中保存存储中数据输出](#feature-5)
- [使用C语言实现内联汇编](#featuer-6)
- [仿真发生HardFault时如何查看错误地址](#feature-7)
- [使用内联函数方法](#feature-8)
- [使用空指令实现时延](#feature-9)
- [MDK中指定结构体或者对象的对齐机制](#feature-10)
- [使用__attribute__指定变量或者程序存储地址](#feature-11)
- [MDK指定多线程编程数目，提高编译效率](#feature-12)

## feature-1

MDK的常用编译选项

- 在Options卡下，C/C++的Misc Controller可以添加编译选项
  - 优化等级， -O1, -Os
  - 是否带调试信息， -g
  - 屏蔽某种警告，--diag_suppress=[num]

## feature-2

常用快捷键。

F7: 仅编译修改文件，需要options->output下Create Batch File不能勾选
Ctrl+A: 选择文件所有内容
Ctrl+F: 当前文件查找内容
Ctrl+Shift+F: 所有文件查找内容

## feature-3

解决软件仿真状态某个范围地址不可读，报READ WRITE错误。

```c
//可添加ini文件，内部数据如下：
map 0x40000000,0x47ffffff read  write
FUNC void Setup (void) {
    _WDWORD(0x40000060, 0x2);           // re-map Flash to vecter table
    SP = _RDWORD(0x10000000);           // Setup Stack Pointer
    PC = _RDWORD(0x10000004);           // Setup Program Counter
    //SP = _RDWORD(0x10018000);         // Setup Stack Pointer
    //PC = _RDWORD(0x10018004);         // Setup Program Counter
}

LOAD %L INCREMENTAL                  // Download

Setup();                             // Setup for Running

//g, __main
g, SystemInit
```

## feature-4

命令生成bin文件输出。

在options/User/After Build选项卡中添加命令。

```shell
fromelf --bin !L --output randisk.bin
```

## feature-5

仿真中保存内存或者flash中的数据为指定文件

仿真过程中在Command窗口

```shell
SAVE [file] [startAddress],[endAddress]
```

## featuer-6

C语言实现内联汇编。

```c
__asm void led_test_asm(void)
{
PORT    EQU 0x40022000
    //...
    B   LOOP
}
```

## feature-7

HardFault时如何查看错误地址。

1.首先查看register栏下的R13寄存器表示当前使用的堆栈指针
2.根据SP的值，在Memory窗口下查看压栈的数据
3.压栈内容顺序为R0~R3, R12, PC，其中偏移0x14内的值即为出错处的执行地址。

## feature-8

MDK中inline关键字由__inline替代

## feature-9

MDK中使用空指令实现时延。

写入和写出时中间可添加延时函数

__nop();

## feature-10

MDK中指定结构体或者对象的对齐机制。

```c
//结构体以字节对齐
typedef __packed struct
{
    char val1;
    int val2;
}STRUCT_PACKED;
STRUCT_PACKED test1;

//变量以字节对齐，变量必须是基础类型
typedef struct
{
    char val1;
    __packed int val2;
}STRUCT_VAL;
STRUCT_VAL test2;

//使用#pragma保存之前的状态，避免pack污染其他对象
#pragma push
#pragma pack(1)
typedef struct
{
    char val1;
    int val2
}STRUCT_PACKED_1;
#pragma pop
STRUCT_PACKED_1 test3;
```

## feature-11

使用__attribute__指定变量或者程序存储地址。

```c
//指定变量保存在SDRAM指定地址
static uint32_t test_sdram[100] __attribute__((section(".ARM.__at_0xC0000000")));

//指定变量保存在SDRAM指定地址
u16 testsram[250000] __attribute__((at(0XC0000000)));

//指定函数放置在指定地址
#pragma arm section code=".ARM.__at_0x08001000"
static void func_test(void)
{
}
#pragma arm section
```

## feature-12

MDK指定多线程编程数目，提高编译效率。

```c
edit > Configuration > Other > Parallel Build Configuration

修改:
Number of parallel jobs选项，允许的多线程编译数目
```

## next_chapter

[返回目录](./../README.md)

直接开始下一小节: [基于vscode的arm-none-eabi-gcc开发和调试](./chx4.vscode_gcc_openocd.md)
