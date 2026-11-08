#include "BorrowRecord.h"
#include <cstdlib>
#include <cstdio>

static bool IsLeap(int y)
{
	return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}
static int DaysInMonth(int y, int m)
{
	static const int t[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	if (m == 2 && IsLeap(y))
		return 29;
	return t[m - 1];
}
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

int BorrowRecord::DueDate() const
{
	int ty = y, tm = m, td = d;
	AddDays(ty, tm, td, days);
	return ty * 10000 + tm * 100 + td;
}

bool BorrowRecord::Overdue(int today) const
{
	return !returned && today > DueDate();
}

ostream & operator<<(ostream &out, const BorrowRecord &r)
{
	out << r.isbn << '\t'
		<< r.y << '-' << r.m << '-' << r.d << '\t'
		<< r.days << '\t'
		<< (r.returned ? 1 : 0) << '\t'
		<< r.ry << '-' << r.rm << '-' << r.rd;
	return out;
}

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
