#ifndef BOOK_H
#define BOOK_H

#include <iostream>
#include <string>
using namespace std;

class Book
{
public:
	Book(long long Isbn = 0, const char *Title = "", const char *Author = "",
		 int Total = 1, int Remain = -1);
	void Set(long long Isbn, const char *Title, const char *Author, int Total, int Remain);

	bool Lend();
	bool GiveBack();

	long long Isbn()  const { return isbn;  }
	int Total()       const { return total; }
	int Remain()      const { return remain;}

	operator long long() const { return isbn;  }
	operator string()    const { return title; }

	friend ostream & operator<<(ostream &out, const Book &b);
	friend istream & operator>>(istream &in,  Book &b);

private:
	long long isbn;
	char title[41], author[21];
	int total;
	int remain;
};

#endif
