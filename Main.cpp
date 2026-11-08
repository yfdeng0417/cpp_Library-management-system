// Main.cpp  控制台菜单与输入
// 只负责输入输出与按键分发，业务逻辑全部在 Library 里。
// 日期在这里折算成 YYYYMMDD 形式的整数，与数据文件保持一致。
#include "Library.h"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <conio.h>

// 在选项字符串里找按键的位置，找不到返回 -1。
int Pos(char ch, const char *str)
{
	for (int i = 0; str[i] != '\0'; i++)
		if (ch == str[i])
			return i;
	return -1;
}

// 只接受 options 里列出的按键，其它键一律忽略。
//     getch() 不回显、不需要回车，交互更顺手。
//     打印提示时把 '\x1b' 这类不可见字符显示成 [Esc]。
int Choice(const char *prompt, const char *options = "")
{
	cout << prompt << " {";
	for (int i = 0; options[i] != '\0'; i++)
	{
		if (' ' < (options[i] & 0x7f) && (options[i] & 0x7f) < 127)
			cout << options[i] << ' ';
		else
		{
			switch (options[i])
			{
			case '\t':   cout << "[Tab] ";   break;
			case '\x1b': cout << "[Esc] ";   break;
			case ' ':    cout << "[Space] "; break;
			}
		}
	}
	cout << "\b}: ";

	int key;
	do
	{
		key = getch();
	} while (options[0] != '\0' && Pos(key, options) < 0);
	cout << endl;
	return key;
}

// 读一行文本。cin.getline 在输入超长时会置 fail 位并停止工作，
//     之后所有输入都会失败，所以必须 clear() 清状态、
//     再用 ignore() 丢掉缓冲区里剩下的字符。
void ReadLine(char *buf, int n, const char *prompt)
{
	cout << prompt;
	cin.getline(buf, n);
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(1024, '\n');
		buf[0] = '\0';
	}
}

// 把当前日期折算成 20251108 这样的整数，与项目里日期的表示法一致。
//     tm_year 是"从 1900 年起的年数"，tm_mon 从 0 起，都要换算。
int Today()
{
	time_t t = time(NULL);
	tm *lt = localtime(&t);
	return (lt->tm_year + 1900) * 10000 + (lt->tm_mon + 1) * 100 + lt->tm_mday;
}

// YYYYMMDD 拆回年月日，供 Lend / GiveBack 使用。
void SplitDate(int v, int &y, int &m, int &d)
{
	y = v / 10000;
	m = v / 100 % 100;
	d = v % 100;
}

// 菜单只负责显示，真正的按键分发在 main 的 switch 里。
void ShowMenu()
{
	cout << "\n\n============ 图书借阅管理系统 ============" << endl;
	cout << "  1 --- 浏 览 读 者（含各人借阅明细）  P" << endl;
	cout << "  2 --- 浏 览 图 书（总量/在架）       B" << endl;
	cout << "  3 --- 新 增 读 者                    N" << endl;
	cout << "  4 --- 新 增 图 书                    M" << endl;
	cout << "  5 --- 借        书                   L" << endl;
	cout << "  6 --- 还        书                   G" << endl;
	cout << "  7 --- 查读者（1学号 2姓名）           S" << endl;
	cout << "  8 --- 查图书（1 ISBN 2题名）          F" << endl;
	cout << "  9 --- 读者按学号排序（升/降）          O" << endl;
	cout << "  A --- 读者按姓名排序（升/降）          T" << endl;
	cout << "  V --- 读者链表倒置                    V" << endl;
	cout << "  D --- 超 期 提 醒                     D" << endl;
	cout << "  W --- 存        盘                   W" << endl;
	cout << "  Q --- 读        盘                   Q" << endl;
	cout << "  0 --- 退        出                  [Esc]" << endl;
	cout << "==========================================" << endl;
}

// 日期做成可选项：直接回车就用今天，也可以手动输入。
//     这样演示超期、补录历史借阅都不用改系统时间。
void DoLend(Library &lib)
{
	char buf[80];
	int readerId, today = Today(), v, y, m, d;
	long long isbn;

	ReadLine(buf, 80, "请输入学号: ");			readerId = atoi(buf);
	ReadLine(buf, 80, "请输入 ISBN: ");			isbn = atoll(buf);
	ReadLine(buf, 80, "借出日期 YYYYMMDD（直接回车 = 今天）: ");
	v = (buf[0] == '\0') ? today : atoi(buf);
	SplitDate(v, y, m, d);

	if (lib.Lend(readerId, isbn, y, m, d))
		cout << "借书成功。" << endl;
	else
		cout << "借书失败。" << endl;
}

void DoGiveBack(Library &lib)
{
	char buf[80];
	int readerId, today = Today(), v, y, m, d;
	long long isbn;

	ReadLine(buf, 80, "请输入学号: ");			readerId = atoi(buf);
	ReadLine(buf, 80, "请输入 ISBN: ");			isbn = atoll(buf);
	ReadLine(buf, 80, "归还日期 YYYYMMDD（直接回车 = 今天）: ");
	v = (buf[0] == '\0') ? today : atoi(buf);
	SplitDate(v, y, m, d);

	if (lib.GiveBack(readerId, isbn, y, m, d))
		cout << "还书成功。" << endl;
	else
		cout << "还书失败。" << endl;
}

void DoAddReader(Library &lib)
{
	char buf[80], name[80], phone[80];
	int id;
	ReadLine(buf, 80, "学号: ");		id = atoi(buf);
	ReadLine(name, 80, "姓名: ");
	ReadLine(phone, 80, "电话: ");
	lib.AddReader(Reader(id, name, phone));
	cout << "已追加到读者链表尾结点之后。" << endl;
}

void DoAddBook(Library &lib)
{
	char buf[80], title[80], author[80];
	long long isbn;
	int total, remain;
	ReadLine(buf, 80, "ISBN: ");		isbn = atoll(buf);
	ReadLine(title, 80, "题名: ");
	ReadLine(author, 80, "作者: ");
	ReadLine(buf, 80, "馆藏总量: ");	total = atoi(buf);
	ReadLine(buf, 80, "在架数（直接回车 = 等于总量）: ");
	remain = (buf[0] == '\0') ? -1 : atoi(buf);
	lib.AddBook(Book(isbn, title, author, total, remain));
	cout << "已追加到图书链表尾结点之后。" << endl;
}

void DoFindReader(Library &lib)
{
	char buf[80];
	int k = Choice("按 1学号 2姓名 查询", "12\x1b");
	if (k == 27) return;
	if (k == '1')
	{
		ReadLine(buf, 80, "请输入学号: ");
		lib.FindReader(atoi(buf));
	}
	else
	{
		ReadLine(buf, 80, "请输入姓名: ");
		lib.FindReader(buf);
	}
}

void DoFindBook(Library &lib)
{
	char buf[80];
	int k = Choice("按 1 ISBN 2题名 查询", "12\x1b");
	if (k == 27) return;
	if (k == '1')
	{
		ReadLine(buf, 80, "请输入 ISBN: ");
		lib.FindBook(atoll(buf));
	}
	else
	{
		ReadLine(buf, 80, "请输入题名: ");
		lib.FindBook(buf);
	}
}

// 主循环：显示菜单，取一个合法按键，再分发处理。
//     按键同时接受数字与字母（大小写均可），对应菜单右侧那一列。
int main()
{
	Library lib;
	char buf[80];
	int key, yn;

	cout << "从 Readers.txt / Books.txt 读取数据……" << endl;
	int n = lib.Load();
	cout << "共读入 " << n << " 条记录（读者 + 图书）。" << endl;

	while (true)
	{
		ShowMenu();
		key = Choice("请选择", "1234567890AaBbDdFfGgLlMmNnOoPpQqSsTtVvWw\x1b");

		if (key == 27 || key == '0')
			break;

		switch (key)
		{
		case '1': case 'p': case 'P':	lib.ShowReaders();				break;
		case '2': case 'b': case 'B':	lib.ShowBooks();				break;
		case '3': case 'n': case 'N':	DoAddReader(lib);				break;
		case '4': case 'm': case 'M':	DoAddBook(lib);					break;
		case '5': case 'l': case 'L':	DoLend(lib);					break;
		case '6': case 'g': case 'G':	DoGiveBack(lib);				break;
		case '7': case 's': case 'S':	DoFindReader(lib);				break;
		case '8': case 'f': case 'F':	DoFindBook(lib);				break;

		case '9': case 'o': case 'O':
			lib.SortReadersById(true);
			cout << "\n按学号（升序）排序结果：" << endl;
			lib.ShowReaders();
			lib.SortReadersById(false);
			cout << "\n按学号（降序）排序结果：" << endl;
			lib.ShowReaders();
			break;

		case 'a': case 't': case 'T':
			lib.SortReadersByName(true);
			cout << "\n按姓名（升序）排序结果：" << endl;
			lib.ShowReaders();
			lib.SortReadersByName(false);
			cout << "\n按姓名（降序）排序结果：" << endl;
			lib.ShowReaders();
			break;

		case 'v': case 'V':
			cout << "\n倒置之前：" << endl;
			lib.ShowReaders();
			lib.ReverseReaders();
			cout << "倒置之后：" << endl;
			lib.ShowReaders();
			break;

		case 'd': case 'D':
			ReadLine(buf, 80, "以哪天为今天？YYYYMMDD（直接回车 = 今天）: ");
			n = lib.OverdueList(buf[0] == '\0' ? Today() : atoi(buf));
			cout << "\n共 " << n << " 条超期记录" << endl;
			break;

		case 'w': case 'W':
			n = lib.Save();
			cout << "已将 " << n << " 位读者写入 Readers.txt，图书写入 Books.txt" << endl;
			break;

		case 'q': case 'Q':
			n = lib.Load();
			cout << "共读入 " << n << " 条记录（读者 + 图书）。" << endl;
			break;
		}
	}

	// 退出前询问是否存盘。
	//     程序不会自动存盘，直接关窗口会丢掉本次的全部修改，
	//     所以默认值取 Y：直接回车就是存盘。
	cout << "\n即将退出，数据是否存盘 ([Y]/n)? ";
	yn = getche();
	if (yn != 'n' && yn != 'N')
	{
		n = lib.Save();
		cout << "\n已将 " << n << " 位读者写入 Readers.txt，图书写入 Books.txt" << endl;
	}
	cout << "\n自动调用析构函数释放所有结点，返回操作系统。\n" << endl;
	return 0;
}
