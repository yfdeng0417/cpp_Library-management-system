// BorrowRecord.cpp  借阅流水数据域类的实现
// 文件开头的三个函数是日期计算的辅助工具，
// 作用是让"到期日"这件事只在这里算一次，别处不再重复实现。
#include "BorrowRecord.h"
#include <cstdlib>
#include <cstdio>

// 三个辅助函数都声明为 static：只在本文件内可见，
//     不会与别的 .cpp 里的同名函数冲突。
static bool IsLeap(int y)
{
	return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}
// 先查表，再单独处理闰年二月，这样表里不必塞两个 2 月。
static int DaysInMonth(int y, int m)
{
	static const int t[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	if (m == 2 && IsLeap(y))
		return 29;
	return t[m - 1];
}
// 逐日推进：先 ++d，超过当月天数就归 1 并进位到月，月超过 12 再进位到年。
//     借期只有 30 天，逐日循环的开销可以忽略，
//     换来的是闰年、月末、年末统统不用特判。
static void AddDays(int &y, int &m, int &d, int n)
{
	while (n-- > 0)
	{
		if (++d > DaysInMonth(y, m))
		{
			d = 1;
			if (++m > 12) { m = 1; y++; }
		}
	}
}

// 初始化列表一次把所有成员都初始化完，
//     returned 与归还日期在这里被置成"未归还"的初值。
BorrowRecord::BorrowRecord(long long Isbn, int Y, int M, int D, int Days)
	: isbn(Isbn), y(Y), m(M), d(D), days(Days), returned(false), ry(0), rm(0), rd(0)
{
}

void BorrowRecord::Set(long long Isbn, int Y, int M, int D, int Days)
{
	isbn = Isbn;  y = Y;  m = M;  d = D;  days = Days;
	returned = false;  ry = rm = rd = 0;
}

void BorrowRecord::SetReturned(int Y, int M, int D)
{
	returned = true;  ry = Y;  rm = M;  rd = D;
}

// 本函数是 const，不能直接改动 y/m/d，
//     所以先拷到局部变量再推进，最后折算成 20250331 这样的整数返回。
int BorrowRecord::DueDate() const
{
	int ty = y, tm = m, td = d;
	AddDays(ty, tm, td, days);
	return ty * 10000 + tm * 100 + td;
}

// today > DueDate()：到期当天不算超期，次日起才算。
//     today 由调用方传入而不是在这里取系统时间，原因有两层：
//     一次统计里所有记录必须用同一个"今天"；
//     测试时也能指定任意日期，不必去改系统时钟。
bool BorrowRecord::Overdue(int today) const
{
	return !returned && today > DueDate();
}

// 存盘格式：ISBN \t 借出日期 \t 借期天数 \t 是否已还 \t 归还日期
//     日期写成 2025-3-1 这样带连字符的形式，读回时用 sscanf 拆开。
ostream & operator<<(ostream &out, const BorrowRecord &r)
{
	out << r.isbn << '\t'
		<< r.y << '-' << r.m << '-' << r.d << '\t'
		<< r.days << '\t'
		<< (r.returned ? 1 : 0) << '\t'
		<< r.ry << '-' << r.rm << '-' << r.rd;
	return out;
}

// 与上面的输出格式一一对应：sscanf("%d-%d-%d") 拆日期，
//     returned 在文件里存 0/1，读回时转成 bool。
istream & operator>>(istream &in, BorrowRecord &r)
{
	char str[80];
	in.getline(str, 80, '\t');	r.isbn = atoll(str);
	in.getline(str, 80, '\t');	sscanf(str, "%d-%d-%d", &r.y, &r.m, &r.d);
	in.getline(str, 80, '\t');	r.days = atoi(str);
	in.getline(str, 80, '\t');	r.returned = (atoi(str) != 0);
	in.getline(str, 80);		sscanf(str, "%d-%d-%d", &r.ry, &r.rm, &r.rd);
	return in;
}
