# renaflag

renaflag是一个轻量级的C++命令行解析库 包含一个头文件和一个源程序

使用renaflag只需将头文件放于源程序目录/头文件文件夹下并在源程序内引用：

```CPP
#include"renaflag.h"
```

并在编译时连接 `renaflag.cpp` 即可

## 开发环境

renaflag基于C++14标准 在Windows内使用Mingw64 8.1.0完成编译测试

目前无法确认是否兼容更老版本的C++标准或linux

## 使用说明

在头文件 `renaflag.h` 中定义了 `renaflag` 封装于命名空间 `rena` 下

这是renaflag实现命令行解析的基类

使用时应在程序头部定义

```CPP
rena::renaflag rf;
```

### 添加命令标识

renaflag的命令标识有层级之分 例如：

```
[ROOT] add (string)
  ┃
[ROOT] get
  ┣━━[SUB] time (int)
  ┃
[ROOT] set
  ┗━━[SUB] overlay (bool)
```

为了实现这样的一个层级 应在创建 `renaflag` 类 `rf` 后 以此如此声明标识：

```CPP
rf.addargs( "add" , F_ROOT , F_NOTBOOL );
rf.addargs( "get" , F_ROOT , F_NOTBOOL );
rf.addargs( "time" , "get" , F_NOTBOOL );
rf.addargs( "set" , F_ROOT , F_NOTBOOL );
rf.addargs( "overlay" , "set" , F_BOOL );
```

成员函数 `addargs` 包含三个参数：

| 参数 | 类型 | 作用 |
| --- | --- | --- |
| `__arg` | 字符串 | 标识符名称 |
| `__parent` | 字符串 | 父标识符 (若为根标识符则填入 `F_ROOT`) |
|`__type` | 布尔型 | 声明标识符 标识布尔型或非布尔型 (仅可以填入 `F_BOOL` 或 `F_NOTBOOL`) |

在renaflag内 所有类型的命令行标识符**在实际上没有区别** 均使用 `char*` 类型存储

不同类型标识符的唯二使用上区别是：

- 布尔型标识符需要特殊声明

- 从 `renaflag` 类内获取值时使用的函数不同

例如 当在程序中需要获取以上标识符的值时 则应先传入命令行：

```CPP
rf.parse( argc , argv );
```

再如此读取每一个标识符的值

```CPP
std::string arg_add         = rf.getstring( "add" , F_ROOT );
int         arg_get_time    = rf.getint( "time" , "get" );
bool        arg_set_overlay = rf.getbool( "overlay" , "set" );
```

不难看出 get系列成员函数包括两个参数：

| 参数 | 类型 | 作用 |
| --- | --- | --- |
| `__arg` |  字符串 | 获取的标识符名称 |
| `__parent` | 字符串 | 该标识符的父标识符 (若为根标识符则填入 `F_ROOT`) |

假设 我们在 `test.cpp` 内对这些命令行标识符做了声明 并在命令行中给出如下命令：

```
.\test add TEST get -time 20221128 set -overlay
```

则三个变量的值依次为：

```CPP
arg_add         == TEST
arg_get_time    == 20221128
arg_set_overlay == true
```

对于命令行参数传入规则 将在后文详细讲解

### 支持的标识符类型

| 类型 | 声明标识符 | 提取值用函数 | 默认值 |
| --- | --- | --- | --- |
| int | `F_NOTBOOL` | `geti32()` | `0` |
| unsigned int | `F_NOTBOOL` | `getu32()` | `0` |
| short | `F_NOTBOOL` | `geti16()` | `0` |
| unsigned short | `F_NOTBOOL` | `getu16()` | `0` |
| long | `F_NOTBOOL` | `geti64()` | `0` |
| unsigned long | `F_NOTBOOL` | `getu64()` | `0` |
| float | `F_NOTBOOL` | `getfloat()` | `0` |
| double | `F_NOTBOOL` | `getdouble()` | `0` |
| char | `F_NOTBOOL` | `getchar()` | `''` |
| string | `F_NOTBOOL` | `getstring()` | `""` |
| bool | `F_BOOL` | `getbool()` | `false` |