# 图书借阅管理系统

《计算机编程实训》（0830A034）课程项目 1「单向链表类模板及其应用（三层结构）」的选题实现。

以实验四提供的 `LinkList<T>` 类模板为基础，设计一个图书借阅管理系统。模板本身**一行未改**，全部工作集中在数据域类与业务层：读者与借阅流水构成「链表的链表」，一次借还操作同时牵动读者流水与图书库存两条链表。

代码按「一个类一对文件」组织，注释尽量保留，适合当作 C++ 面向对象部分的阅读材料。

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
| 构造函数、初始化列表、默认实参 | `Book::Book(...) : isbn(Isbn)`，`Book.cpp:6` |
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
| 常成员函数与 `const_cast` | `Reader::NumUnreturned()`，`Reader.cpp:11` |
| 静态局部变量 | `Locate` 里的 `static Node<T> *p`，`LinkList.h:385` |
| 动态内存 `new` / `delete` | 结点的创建与释放 |
| C 风格字符串处理 | `strncpy` / `atoi` / `atoll`，`Book.cpp:47` |
| 文件流 | `ifstream` / `ofstream`，`LinkList::Save` / `Load` |
| 输入流的状态与恢复 | `cin.clear()` + `cin.ignore()`，`Main.cpp:43` |
| 异常抛出 | `throw(int)`，`DeleteCurNode` / `CurData` |
| 头文件保护与预处理宏 | `#ifndef LINKLIST_H` |
| 命名空间 | `using namespace std;` |

## 从代码里能学到什么

下面每一段都是项目里真实存在的代码，可以直接对着源文件读。

### 1. 为什么必须自己写赋值运算符

`LinkList.h:103`

```cpp
template <typename T>
LinkList<T> & LinkList<T>::operator=(const LinkList &list)
{
    if (this == &list)                  // ← 自赋值检查
        return *this;
    FreeList();                         // 先释放本对象原有的结点
    Node<T> *temp = NULL;
    for (Node<T> *p = list.head; p != NULL; p = p->next)
    {
        Append(p->data);                // 逐个 new 出新结点，而不是拷指针
        if (p == list.cur_node)
            temp = cur_node;            // 顺便把“当前结点”也搬过去
    }
    cur_node = temp;
    return *this;
}
```

如果不写这个函数，编译器生成的默认赋值会把 `head` 指针直接拷过去，于是两个对象指向同一批结点；其中任何一个析构时 `delete` 掉这些结点，另一个就只剩一堆野指针——这就是**浅拷贝**。这里的做法是「先清空自己，再逐个复制结点」，称为**深拷贝**。

`if (this == &list)` 那三行同样关键：如果右值就是自己，`FreeList()` 会把自己（也就是右值）的结点全删掉，循环再去遍历一条已经空了的链表。

拷贝构造函数 `LinkList.h:91` 只有一行 `*this = list;`，把活交给赋值运算符——这是复用代码的常见写法：先把自己构造为空链表，再整体赋值。

### 2. 友元类：把私有成员开给模板

`LinkList.h:14`

```cpp
template <typename T> class Node
{
public:
    Node() : next(NULL) {}
    Node(const T &t) : data(t), next(NULL) {}          // 转换构造函数
    friend class LinkList<T>;                          // ← 友类声明
private:
    T data;                    // 数据域
    Node<T> *next;             // 指针域
};
```

`data` 和 `next` 都是私有的，`LinkList<T>` 却能直接写 `p->next`、`p->data`。原因是 `Node` 里那句 `friend class LinkList<T>;`——友元不是成员，但**被授权访问私有成员**。

对比一下另一种做法：给 `Node` 加 `T & Data()` 和 `Node<T> *& Next()` 两个公有接口。那样多出两个函数、每次访问都要绕一层，而且把「链表怎么串起来」这件事暴露给了所有人。友元在这里换到的是**性能与简洁**，代价是 `Node` 的封装被 `LinkList` 单向打开了。

另外注意 `template <typename T> class LinkList;` 这条前置声明（`LinkList.h:12`）。`Node` 里用到了 `LinkList<T>`，而 `LinkList` 又用到了 `Node<T>`，两个类互相引用，必须先声明后定义，否则编译器在读到 `friend class LinkList<T>;` 时还不知道它是什么。

### 3. 类型转换运算符：不改模板就能多键排序

`Reader.h:12`

```cpp
operator int()    const { return id;   }
operator string() const { return name; }
```

模板里的 `Sort` 和 `Locate` 都是这样比较的（`LinkList.h:392`、`LinkList.h:413`）：

```cpp
TYPE(p->data) != TYPE(data)      // Locate 判相等
(TYPE)p->data < (TYPE)head->data // Sort 判大小
```

`TYPE` 是调用时传进来的类型。于是同一个 `LinkList<Reader>`，换个实参就换了一把「尺子」（`Library.cpp:159`）：

```cpp
readers.Sort(int(0),    asc);   // 按学号排：把 Reader 转成 int 再比
readers.Sort(string(""), asc);  // 按姓名排：把 Reader 转成 string 再比
```

`int(0)` 和 `string("")` 本身没有意义，它们只是在告诉编译器「这次用哪种转换」。这样 `LinkList` 一行都不用改，就同时支持了按学号排、按姓名排、按 ISBN 查、按题名查——**把「比较什么」从模板里移到了调用处**。

顺带一个坑：`operator int()` 和 `operator long long()` 同时存在时，`cout << r` 这种地方编译器可能不知道该选哪个转换，容易产生歧义，所以输出一律走显式的 `Show()` 或 `operator<<`。

### 4. 静态局部变量：`Locate` 的续查状态

`LinkList.h:385`

```cpp
template <typename T> template <typename TYPE>
Node<T> *LinkList<T>::Locate(const TYPE &x, bool newsearch)
{
    static Node<T> *p = head;        // ← 注意是 static
    static TYPE data = x;
    if (newsearch)                   // 新开一轮查找
    {
        p = head;
        data = x;
    }
    for (; p != NULL && TYPE(p->data) != TYPE(data); p = p->next)
        ;                            // 从上次停下的地方接着找
    cur_node = p;
    if (p != NULL)
        p = p->next;                 // 悄悄往后挪一步，为下一次续查做准备
    return cur_node;
}
```

`Locate` 要支持「找出所有同名的人」，所以它记住上次找到的位置，下次不传 `newsearch` 就从那儿继续找。`static` 局部变量在函数返回后依然存在，正好用来保存这个状态。

代价是：**这个状态在所有 `LinkList<T>` 对象之间共享**。`readers.Locate(...)` 和 `books.Locate(...)` 的类型不同，各有一份；但同一个类型的多条链表会互相踩。所以项目里凡是「开始一轮新查找」的地方，一律显式写 `Locate(x, true)`；只有循环里的续查才省略第二个参数：

```cpp
Node<Reader> *p = readers.Locate(id, true);     // 第一轮，重置
while (p != NULL)
{
    n++;
    readers.CurData().Show(cout);
    p = readers.Locate(id);                     // 续查，接着上次的位置
}
```

### 5. 引用返回：让函数调用能当左值

`LinkList.h:334`

```cpp
template <typename T>
T & LinkList<T>::CurData() throw(int)
{
    if (cur_node != NULL)
        return cur_node->data;      // 返回引用，不是返回值
    else
        throw -1;
}
```

返回类型带 `&`，`records.CurData()` 就成了「那个结点里的数据域本身」，可以直接赋值：

```cpp
records.CurData().SetReturned(y, m, d);   // 直接改链表里的对象
```

如果去掉 `&` 改成 `T CurData()`，返回的是一份**拷贝**，改了也白改。

同样的道理出现在 `Library.cpp:148`：

```cpp
Reader &r = readers.CurData();   // 必须用引用
r.AppendRecord(...);             // 追加进链表里的那个 Reader
```

写成 `Reader r = readers.CurData();` 就拷贝了一个 `Reader` 出来，它的内层链表和链表里的那份毫无关系，追加的记录在函数返回时随副本一起消失——这类 bug 不会报错，只会「数据莫名不见了」。

### 6. 先校验、后修改：一次借还牵动两条链表

`Library.cpp:41`

```cpp
bool Library::Lend(int readerId, long long isbn, int y, int m, int d)
{
    Node<Book> *pb = books.Locate(isbn, true);
    if (pb == NULL) return false;          // ① 书不存在
    Book &b = books.CurData();
    if (!b.Lend()) return false;           // ② 没有库存
    Node<Reader> *pr = readers.Locate(readerId, true);
    if (pr == NULL)                        // ③ 读者不存在
    {
        b.GiveBack();                      // ← 回滚刚才减掉的库存
        return false;
    }
    Reader &r = readers.CurData();
    r.AppendRecord(BorrowRecord(isbn, y, m, d, 30));
    return true;
}
```

借一本书要动两处：图书的 `remain--` 和读者流水表里追加一条。这两步要么都成功，要么都不发生，否则库存就悄悄少了一本。

正确顺序是**把所有可能失败的条件先检查完，再动手修改**。还书 `GiveBack`（`Library.cpp:60`）把这一点做得更彻底：

```cpp
if (books.Locate(isbn, true) == NULL)   return false;
if (readers.Locate(readerId, true) == NULL) return false;
if (!readers.CurData().GiveBackRecord(isbn, y, m, d)) return false;
books.CurData().GiveBack();             // 前三步都过了，才动库存
```

三行校验全部通过，才执行最后一句。这就是写出「不会留下半截状态」的代码的办法。

### 7. `const` 成员函数里的 `const_cast`

`Reader.cpp:11`

```cpp
int Reader::NumUnreturned() const
{
    LinkList<BorrowRecord> &rl = const_cast<LinkList<BorrowRecord> &>(records);
    int n = 0;
    for (Node<BorrowRecord> *p = rl.GoTop(); p != NULL; p = rl.Skip(1))
        if (!rl.CurData().IsReturned())
            n++;
    return n;
}
```

函数声明为 `const`，意味着它不修改 `Reader`，也意味着成员 `records` 在这里是常量。可 `LinkList` 的遍历接口 `GoTop()`、`Skip()` 都会改动「当前结点」这个内部状态，没有提供 `const` 版本，于是编译不过。

`const_cast` 就是把 `records` 上的 `const` 临时去掉，让它能调用那些非 `const` 成员函数。这也是**唯一**能这么用的场合：遍历只改了「当前结点指针」，没有改链表里的任何数据，逻辑上仍然是只读的。

补偿手段是接口设计——`Reader` 只对外暴露 `RecordsTop()` / `RecordsNext()` / `CurRecord()` 三个受限入口，别的代码拿不到整条内层链表。更干净的做法是让 `LinkList` 增加一组 `const` 遍历接口，但那要改老师提供的模板，与本项目「模板一行不改」的约束冲突。

### 8. 显示格式与存盘格式必须分开

`Reader.cpp:36` 和 `Reader.cpp:45`

```cpp
void Reader::Show(ostream &out) const          // 给人看
{
    out << "学号：" << setfill('0') << setw(4) << id << ...
    records.PutList(out);
}

ostream & operator<<(ostream &out, const Reader &r)   // 给文件看
{
    out << r.id << '\t' << r.name << '\t' << r.phone << endl;
    r.records.PutList(out);
    return out;
}
```

一个类有两个「输出」函数，输出的内容还不一样，看着像重复，其实是必须的。

假如偷懒让 `operator<<` 直接调用 `Show()`，存进文件的就会是 `学号：0001` 这样的字符串；读回来时 `atoi("学号：0001")` 得 0，所有数据变成 0 或空。**存盘格式是给机器读的，必须是纯数据；显示格式是给人看的，可以随便加中文。** 两者混在一起，程序第一次存盘就会静默毁掉全部数据。

## 建议的阅读顺序

1. `Book.h` / `Book.cpp`——最小的数据域类，看清「一个类怎样满足模板的三项要求」；
2. `BorrowRecord.h` / `.cpp`——同样是数据域类，多了日期与到期判断；
3. `Reader.h` / `.cpp`——内层链表的持有者，「链表的链表」的关键；
4. `Library.h` / `.cpp`——业务层，只看 `Lend` / `GiveBack` / `OverdueList` 三个函数即可；
5. `Main.cpp`——菜单与输入，最后看；
6. `LinkList.h`——模板本身，可以先只看 `Node` 与 `Append` / `Locate` / `Sort` 三个函数。

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
