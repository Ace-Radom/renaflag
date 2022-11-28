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

在头文件 `renaflag.h` 中定义了 `renaflag` 类 封装于命名空间 `rena` 下

这是renaflag实现命令行解析的基类

使用时应在程序头部定义

```CPP
rena::renaflag rf;
```

### 声明命令行标识符

renaflag的命令行标识符有层级之分 例如：

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
// 对于非布尔型的标识符的声明 F_NOTBOOL可以省略
```

成员函数 `addargs` 包含三个参数 (详见 `可使用的成员函数列表` 板块)：

| 参数 | 类型 | 作用 | 默认值 |
| --- | --- | --- | --- |
| `__arg` | string | 标识符名称 | 无 |
| `__parent` | string | 父标识符 (若为根标识符则填入 `F_ROOT`) | 无 |
| `__type` | bool | 声明标识符 标识布尔型或非布尔型 (仅可以填入 `F_BOOL` 或 `F_NOTBOOL`) | `F_NOTBOOL` |

**请务必注意**

1. 声明标识时请尽可能按照层级表依次声明 其中子标识符声明**必须**在根标识符声明之后 否则会出现无法找到根标识符的错误导致解析报错

2. renaflag支持但仅支持双层标识符 即：在一个根标识符的子标识符下再次声明子标识符是不被允许的 任何子标识符的父标识符**必须**是根标识符 \
比如以下示例：在子标识符 `file` 下又声明了子标识符 `path`

    ```CPP
    rf.addargs( "add" , F_ROOT , F_NOTBOOL );
    rf.addargs( "file" , "add" , F_NOTBOOL );
    rf.addargs( "path" , "file" , F_NOTBOOL );
    // 此处按照逻辑 path标识符被挂载在file子标识符下 这是不被允许的
    ```

    在这种情况下 renaflag会将一个运行错误流推送至终端 而 `path` 标识符则不会被添加

3. renaflag**不原生支持**自定义默认值 如果一定需要可以在解析命令行后使用成员函数 `isparsed()` 检测标识符是否被传入 配合 `if` 判断赋值 【参见成员函数表】

### 解析命令行

在renaflag内 所有类型的命令行标识符**在实际上没有区别** 均使用 `char*` 类型存储

不同类型标识符的唯二使用上区别是：

- 布尔型标识符需要特殊声明

- 从 `renaflag` 类内获取值时使用的函数不同

例如 当在程序中需要获取以上标识符的值时 则应先传入命令行：

```CPP
rf.parse( argc , argv , false );
```

`parse` 成员函数的参数如下：

| 参数 | 类型 | 作用 | 默认值 |
| --- | --- | --- | --- |
| `__argc` | int& | 传入标识符个数 | 无 |
| `__argv` | char**& | 传入的各个标识符 | 无 |
| `__save_original_argv` | bool | 是否保存原始读入的 `argv` | `false` 即不保存

再如此读取每一个标识符的值

```CPP
std::string arg_add         = rf.getstring( "add" , F_ROOT );
int         arg_get_time    = rf.getint( "time" , "get" );
bool        arg_set_overlay = rf.getbool( "overlay" , "set" );
```

不难看出 get系列成员函数包括两个参数：

| 参数 | 类型 | 作用 |
| --- | --- | --- |
| `__arg` | string | 获取的标识符名称 |
| `__parent` | string | 该标识符的父标识符 (若为根标识符则填入 `F_ROOT`) |

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

## 支持的标识符类型

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

## 可使用的成员函数列表

1. `addargs`

    - 声明一个命令行标识符

    - 函数声明：

        ```CPP
        errno_t rena::renaflag::addargs( std::string __arg , std::string __parent , bool __type = F_NOTBOOL );
        ```

    - 参数列表：

        | 参数 | 类型 | 作用 | 默认值 |
        | --- | --- | --- | --- |
        | `__arg` | string | 标识符名称 (不能出现除 `?` 或 `_` 以外的字符 不允许出现从 `a` 到 `z` 以外的字母 只有 `_` 允许和字母连用) | 无 |
        | `__parent` | string | 父标识符名称 (若为根标识符则填入 `F_ROOT`) | 无 |
        | `__type` | bool | 声明标识符 标识布尔型或非布尔型 (仅可以填入 `F_BOOL` 或 `F_NOTBOOL`) | `F_NOTBOOL` |

    - 返回值：

        | 返回值 | 状况 | 结果 |
        | --- | --- | --- |
        | `ARG_OK` | 声明标识符成功 | 标识符被成功添加 |
        | `ARG_REDEFINED` | 该标识符在同级被重复定义 | 运行错误流被推送至终端 标识符不会被添加 |
        | `ARG_UNACCEPTEDARG` | 标识符名称非法 | 运行错误流被推送至终端 标识符不会被添加 |
        | `ARG_BOUNDED` | 标识符层级越界 (总层级超过两层) | 运行错误流被推送至终端 标识符不会被添加 |

2. `parse`

    - 解析命令行

    - 函数声明：

        ```CPP
        errno_t rena::renaflag::parse( int& __argc , char**& __argv , bool __save_original_argv = false );
        ```