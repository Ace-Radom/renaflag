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

**若没有特殊说明 本文中提到的“标准拉丁字母”皆代表自 `a` 至 `z` 的26个英文字母**

在头文件 `renaflag.h` 中定义了 `renaflag` 类 封装于命名空间 `rena` 下

这是renaflag实现命令行解析的基类

### 初始化

在 `renaflag` 类内声明了未被实现的成员函数 `preset` 所有命令行标识符声明 帮助文档声明等等预设选项 都应包含在源程序里对 `preset` 成员函数的实现代码中

因此 初始化renaflag的完整过程为：

```CPP
// test.cpp
#include"renaflag.h"

void rena::renaflag::preset(){
    ... // 声明标识符

    ... // 声明帮助文档 (可选)
}

rena::renaflag rf;
```

类 `renaflag` 的构析函数会自动调用 `preset` 成员函数 无需在程序内再次初始化

因为renaflag的标识符声明是通过调用函数实现 所以理论上有极高的自由度 在 `preset` 成员函数内加入任何被允许出现在类构析函数内的语句都是合法的

但由于 `preset` 是类内函数 有权更改类内的所有值 包括被声明在 `private` 下的值 所以建议在该函数内只加入标识符 帮助文档声明和一些判断语句 避免将 `preset` 成员函数实现的太过臃肿 以避免不必要的错误

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

为了实现该层级 成员函数 `preset` 的实现代码应如此声明标识：

```CPP
void rena::renaflag::preset(){
    addargs( "add" , F_ROOT , F_NOTBOOL );
    addargs( "get" , F_ROOT , F_NOTBOOL );
    addargs( "time" , "get" , F_NOTBOOL );
    addargs( "set" , F_ROOT , F_NOTBOOL );
    addargs( "overlay" , "set" , F_BOOL );
    // 对于非布尔型的标识符的声明 F_NOTBOOL可以省略

    ... // 声明帮助文档 (可选)
}
```

成员函数 `addargs` 包含三个参数 (详见 `可使用的成员函数列表` 板块)：

| 参数 | 类型 | 作用 | 默认值 |
| --- | --- | --- | --- |
| `__arg` | string | 标识符名称 | 无 |
| `__parent` | string | 父标识符 (若为根标识符则填入 `F_ROOT`) | 无 |
| `__type` | bool | 声明标识符 标识布尔型或非布尔型 (仅可以填入 `F_BOOL` 或 `F_NOTBOOL`) | `F_NOTBOOL` |

**请务必注意**

1. renaflag允许非同层级的标识符重名 但同级标识符重名是不允许的

    例如：

    ```
    [ROOT] get (string)
      ┣━━[SUB] time (bool)
      ┃
    [ROOT] set (string)
      ┗━━[SUB] time (int)
    // 合法 两个time子标识符分别挂载于get和set根标识符下

    [ROOT] set (string)
      ┣━━[SUB] folder (bool)
      ┃
    [ROOT] set (bool)
      ┗━━[SUB] time (int)
    // 非法 set根标识符被重复定义
    ```

2. 声明标识时请尽可能按照层级表依次声明 其中子标识符声明**必须**在根标识符声明之后 否则会出现无法找到根标识符的错误导致解析报错

3. renaflag支持但仅支持双层标识符 即：在一个根标识符的子标识符下再次声明子标识符是不被允许的 任何子标识符的父标识符**必须**是根标识符 \
比如以下示例：在子标识符 `file` 下又声明了子标识符 `path`

    ```CPP
    rf.addargs( "add" , F_ROOT , F_NOTBOOL );
    rf.addargs( "file" , "add" , F_NOTBOOL );
    rf.addargs( "path" , "file" , F_NOTBOOL );
    // 此处按照逻辑 path标识符被挂载在file子标识符下 这是不被允许的
    ```

    在这种情况下 renaflag会将一个运行错误流推送至终端 而 `path` 标识符则不会被添加

4. renaflag**不原生支持**自定义默认值 如果一定需要可以在解析命令行后使用成员函数 `isparsed()` 检测标识符是否被传入 配合 `if` 判断赋值 【参见成员函数表】

### 解析命令行

在renaflag内 所有类型的命令行标识符**在实际上没有区别** 均使用 `char*` 类型存储

不同类型标识符的唯二使用上区别是：

- 布尔型标识符需要特殊声明

- 从 `renaflag` 类内获取值时使用的函数不同

例如 当在程序中需要获取以上标识符的值时 则应先传入命令行：

```CPP
rf.parse( argc , argv , false );
```

在主程序中 建议如此使用解析命令行成员函数 `parse`：

```CPP
#include"renaflag.h"

rena::renaflag rf;

rf.addargs( "test" , F_ROOT , F_NOTBOOL );
...

int main( int argc , char** argv ){

    rf.parse( argc , argv , false );
    // 在主程序开始后直接调用parse

    ...

    return 0;
}
```

或在解析时保持 `argc` 和 `argv` 为被人为改动

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

## 命令行标识符传入规则

renaflag会自动清除所有传入的值 (无论是标识符或是普通值) 第一和第二位的字符或非标准拉丁字母 (若第一位不为标准拉丁字母)

这意味着 以下所有标识符在命令行中被传入是 对于renaflag是等价的 在readme中所有子标识符前加上 `-` 仅是为了方便标识

```
test -test --test /test +test ++test ?test étest
```

但这样传入标识符是错误的 它会被认定为字符和字母混用 因为他的第一位是标准拉丁字母

```
t/test
```

renaflag的命令行标识符在存储和判定时**不区分大小写** 所有字母都会被转为小写形式进行比较

这意味着 标识符 `test` 和 `Test` 或 `tEsT` 等价

renaflag严格要求所有非布尔型标识符后**不允许出现非被声明的标识符的值** 一旦非布尔型标识符被解析 他就会被记录为 `true` 反之则为 `false`

并且 一旦非布尔型标识符被解析 如果他是根标识符且他没有被声明的子标识符 那么在其之后的一个值将自动记录为该非布尔型标识符的值 **无论他是否是被声明的标识符** 

一个没有被声明的子标识符的非布尔型根标识符或一个非布尔型子标识符后没有值是不被允许的

例如当声明了布尔型根标识符 `bool` 和非布尔型根标识符 `notbool`：

```
.\test bool notbool YES
// bool = true
// notbool = "YES"

.\test bool notbool
// 非法 notbool没有子标识符且后没有值

.\test notbool bool
// notbool = "bool"
// bool 未被解析
```

### 根标识符的值和子标识符的关系

renaflag允许同时给根标识符赋值和传入该根标识符的子标识符 但有一定限制

例如在程序 `test` 中如此声明标识符：

```
[ROOT] get (string)
  ┣━━[SUB] time (bool)
  ┃
[ROOT] set (string)
  ┗━━[SUB] time (int)
```

并在命令行中输入

```
.\test get type -time set -time 20221129
// get      = "type"
// get_time = true
// set      = NULL
// set_time = "20221129"
```

此时 因为 `type` 并不是被声明的标识符 所以他被解析为根标识符 `get` 的值

但这么传入是非法的：

```
.\test get time -time
// 非法 在get之后的time会被解析为子标识符time 但在其之后又出现了子标识符time 布尔型标识符被重复赋值
```

。。。

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

    - 参数列表：

        | 参数 | 类型 | 作用 | 默认值 |
        | --- | --- | --- | --- |
        | `__argc` | int& | 传入标识符个数 | 无 |
        | `__argv` | char**& | 传入的各个标识符 | 无 |
        | `__save_original_argv` | bool | 是否保存原始读入的 `argv` | `false` 即不保存

    - 返回值：

        | 返回值 | 状况 | 结果 |
        | --- | --- | --- |
        | 正整数 | 解析成功 | 所有标识符的值被写入 返回有效的标识符个数 |
        | `P_TITLEONLY` 或 `0` | 没有任何标识符或值被解析 (即 `__argc` 的值为1) | 成员函数 `is_titleonly()` 返回 `true` |
        | `P_UNDEFINED` | 出现未被声明的标识符 | 解析失败 成员函数 `get_errorarg_in_parse()` 返回第一个未被声明的标识符 |
        | `P_VALUEAFTERBOOL` | 布尔型标识符后出现了非被声明标识符的值 | 解析失败 成员函数 `get_errorarg_in_parse()` 返回第一个出错的布尔型标识符 |
        | `P_NOVALUEAFTERNOTBOOL` | 非布尔型标识符后没有值 | 解析失败 成员函数 `get_errorarg_in_parse()` 返回第一个出错的非布尔型标识符 |