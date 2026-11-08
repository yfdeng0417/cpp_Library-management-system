# 图书借阅管理系统

以实验四提供的 `LinkList<T>` 类模板为基础，设计一个图书借阅管理系统。模板本身**一行未改**，全部工作集中在数据域类与业务层：读者与借阅流水构成「链表的链表」，一次借还操作同时牵动读者流水与图书库存两条链表。

代码按「一个类一对文件」组织。源文件里写了比较详细的中文注释，重点不在「这一行做了什么」，而在「为什么必须这样写、不这样写会出什么问题」——每一处注释都对应一个真实踩过的坑，适合当作 C++ 面向对象部分的阅读材料。

## 功能

| 键 | 功能 | 说明 |
| --- | --- | --- |
| `1` / `P` | 浏览读者 | 连同每位读者的借阅明细一起输出 |
| `2` / `B` | 浏览图书 | 显示馆藏总量与在架数 |
| `3` / `N` | 新增读者 | 追加到读者链表尾 |
| `4` / `M` | 新增图书 | 追加到图书链表尾 |
| `5` / `L` | 借书 | 库存减一、读者流水追加一条 |
| `6` / `G` | 还书 | 流水标记已还、库存加一 |
| `7` / `S` | 查读者 | 按学号或姓名 |
| `8` / `F` | 查图书 | 按 ISBN 或题名 |
| `9` / `O` | 读者按学号排序 | 升序 / 降序 |
| `A` / `T` | 读者按姓名排序 | 升序 / 降序 |
| `V` | 读者链表倒置 | 就地逆置 |
| `D` | 超期提醒 | 双层遍历，统计未还且已过期的流水 |
| `W` | 存盘 | 两层结构分别写入两个文件 |
| `Q` | 读盘 | 从文件重建两条链表 |
| `0` / `Esc` | 退出 | 退出前询问是否存盘 |

## 三层结构

| 层次 | 内容 | 本项目的实现 |
| --- | --- | --- |
| 底层 | 数据域（形式类型 `T`） | `Book`、`BorrowRecord`、`Reader` |
| 中层 | 结点类 `Node<T>`（`data` + `next`） | 直接复用，未改 |
| 高层 | 链表类 `LinkList<T>` | 直接复用，未改 |

数据域类只需满足三项约定就能被模板复用：

1. 四大函数（构造、拷贝构造、析构、赋值）正确；
2. 重载 `operator<<` / `operator>>`；
3. 能参与关系运算——本项目用**类型转换运算符**实现，而不重载比较运算符。

## 链表的链表

`Reader` 内部持有 `LinkList<BorrowRecord> records`，于是外层读者链表的每个结点里又装着一条完整的链表：

```
LinkList<Reader> readers
 ├─ Node<Reader> ─ 张三 ─ LinkList<BorrowRecord> records
 │                          ├─ Node<BorrowRecord>  C++程序设计   2025-3-1   未还
 │                          └─ Node<BorrowRecord>  数据结构与算法 2025-2-15  已还
 ├─ Node<Reader> ─ 李四 ─ LinkList<BorrowRecord> records
 │                          └─ Node<BorrowRecord>  C++程序设计   2025-3-10  未还
 └─ Node<Reader> ─ 王五 ─ LinkList<BorrowRecord> records（空）
```

`records` 是私有成员，外部拿不到整条内层链表，只能通过三个受限入口遍历：

```cpp
Node<BorrowRecord> * RecordsTop();    // 定位到首结点
Node<BorrowRecord> * RecordsNext();   // 后移一个结点
BorrowRecord       & CurRecord();     // 取当前结点的数据
```

这样业务层能读、能改单个流水，却无法整条替换或清空内层链表。

## 这个项目用到了哪些 C++ 知识

| 知识点 | 出现在哪里 |
| --- | --- |
| 类的定义与封装 | `Book` / `BorrowRecord` / `Reader` 三个数据域类 |
| 构造函数、初始化列表、默认实参 | `Book::Book(...) : isbn(Isbn)`，`Book.cpp:11` |
| 函数重载 | `Library::FindReader(int)` 与 `FindReader(const char *)` |
| 类模板与模板的成员函数 | `Node<T>`、`LinkList<T>`，以及 `template <typename TYPE> Locate(...)` |
| 模板的声明与定义必须同处一个头文件 | `LinkList.h` 445 行，没有对应的 `.cpp` |
| 前置声明（解决相互引用） | `LinkList.h:12` |
| 友元类 `friend class` | `Node<T>` 把私有成员开给 `LinkList<T>`，`LinkList.h:25` |
| 运算符重载（成员函数形式） | `operator=` |
| 运算符重载（非成员函数形式） | `operator<<` / `operator>>` |
| 类型转换运算符 | `operator long long()`、`operator string()`、`operator int()` |
| 深拷贝、自赋值、双重释放 | `LinkList` 的拷贝构造函数与赋值运算符 |
| 析构函数与资源释放 | `~LinkList()` 调 `FreeList()` |
| 引用返回，使函数调用表达式可作左值 | `T & CurData()`，`LinkList.h:334` |
| 常成员函数与 `const_cast` | `Reader::NumUnreturned()`，`Reader.cpp:21` |
| 静态局部变量 | `Locate` 里的 `static Node<T> *p`，`LinkList.h:385` |
| 动态内存 `new` / `delete` | 结点的创建与释放 |
| C 风格字符串处理 | `strncpy` / `atoi` / `atoll`，`Book.cpp:64` |
| 文件流 | `ifstream` / `ofstream`，`LinkList::Save` / `Load` |
| 输入流的状态与恢复 | `cin.clear()` + `cin.ignore()`，`Main.cpp:53` |
| 异常抛出 | `throw(int)`，`DeleteCurNode` / `CurData` |
| 头文件保护与预处理宏 | `#ifndef LINKLIST_H` |
| 命名空间 | `using namespace std;` |

## 建议的阅读顺序

1. `Book.h` / `Book.cpp`——最小的数据域类，看清「一个类怎样满足模板的三项要求」；
2. `BorrowRecord.h` / `.cpp`——同样是数据域类，多了日期与到期判断；
3. `Reader.h` / `.cpp`——内层链表的持有者，「链表的链表」的关键；
4. `Library.h` / `.cpp`——业务层，只看 `Lend` / `GiveBack` / `OverdueList` 三个函数即可；
5. `Main.cpp`——菜单与输入，最后看；
6. `LinkList.h`——模板本身，可以先只看 `Node` 与 `Append` / `Locate` / `Sort` 三个函数。

每个文件开头的注释先说明这个文件负责什么；读到某个函数时，函数上方的注释通常会解释它要绕开的坑。下面「这个项目用到了哪些 C++ 知识」只列清单，具体解释都在源码里，对着看即可。

## 文件说明

| 文件 | 作用 |
| --- | --- |
| `LinkList.h` | 实验四提供的链表类模板（数据域 / 结点 / 链表三层），**未作任何修改** |
| `Book.h` `.cpp` | 图书：ISBN、题名、作者、馆藏总量、在架数；提供 `Lend()` / `GiveBack()` |
| `BorrowRecord.h` `.cpp` | 借阅流水：ISBN、借出日期、借期天数、归还标记、归还日期；含到期日与超期判断 |
| `Reader.h` `.cpp` | 读者：学号、姓名、电话，内含 `LinkList<BorrowRecord>` |
| `Library.h` `.cpp` | 业务层：组织两条链表，负责借还、查询、排序、倒置、超期统计、读写文件 |
| `Main.cpp` | 控制台菜单与输入处理 |
| `Readers.txt` / `Books.txt` | 演示数据 |
| `Library.cbp` | Code::Blocks 工程文件 |

## 编译与运行

**Code::Blocks**：打开 `Library.cbp`，按 `F9`（或先 `Ctrl+F11` 重建）。

**命令行**（MinGW / TDM-GCC）：

```bat
g++ -std=c++11 Main.cpp Library.cpp Reader.cpp Book.cpp BorrowRecord.cpp -o Library.exe
```

`-std=c++11` 是必须的：`LinkList.h` 使用了 `throw(int)` 这种动态异常规范，该特性在 C++17 中已被移除，用默认的 C++17 编译会直接报错。

运行时**工作目录必须是 `Readers.txt` / `Books.txt` 所在的目录**，否则读不到数据。

## 数据文件格式

读者文件采用两层结构：顶层是读者个数，每个读者一行基本信息，紧跟着这个读者的流水条数，再跟若干条流水。

```
(3)
1	张三	13837213721
(2)
9787302500001	2025-3-1	30	0	0-0-0
9787111600002	2025-2-15	30	1	2025-2-20

2	李四	13912345678
(1)
9787302500001	2025-3-10	30	0	0-0-0

3	王五	13387654321
(0)
```

图书文件是平铺的单层结构：

```
(3)
9787302500001	C++程序设计	李青	3	1
9787111600002	数据结构与算法	王红	2	2
9787111000003	操作系统概念	赵明	1	1
```

流水字段依次为：ISBN、借出日期、借期天数、是否已还（0/1）、归还日期（未还为 `0-0-0`）。
日期一律以 `YYYYMMDD` 形式的整数保存与比较。

## 已知限制

- 追加与查找都是 `O(n)`，超期统计是双层遍历 `O(n·m)`；这是单向链表且不允许修改模板的必然结果。
- 日期只按整数比较，未处理跨月跨年的进位校验，借期天数固定为 30。
- 读者姓名、电话用定长字符数组保存，超长会被截断。
- 无并发控制，单机单进程使用。

## 说明

除 `README.md` 为 UTF-8 外，其余源文件均为 **GBK（代码页 936）** 编码，与实验四教师提供的 `LinkList.h` 保持一致。用 VS Code 打开时请以 GBK 重新打开，否则中文注释与字符串会显示为乱码——**不要另存为 UTF-8**，那会破坏文件内容。

演示数据中的人名、电话均为虚构。
