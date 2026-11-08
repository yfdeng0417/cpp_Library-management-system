#ifndef BORROWRECORD_H
#define BORROWRECORD_H

#include <iostream>
#include <string>
using namespace std;

class BorrowRecord
{
public:
	BorrowRecord(long long Isbn = 0, int Y = 0, int M = 0, int D = 0, int Days = 30);
	void Set(long long Isbn, int Y, int M, int D, int Days);
	void SetReturned(int Y, int M, int D);

	int  DueDate()  const;
	bool Overdue(int today) const;

	long long Isbn() const { return isbn; }
	bool IsReturned() const { return returned; }

	operator long long() const { return isbn; }

	friend ostream & operator<<(ostream &out, const BorrowRecord &r);
	friend istream & operator>>(istream &in,  BorrowRecord &r);

private:
	long long isbn;
	int y, m, d;
	int days;
	bool returned;
	int ry, rm, rd;
};

#endif
