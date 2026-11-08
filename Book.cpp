#include "Book.h"
#include <cstdlib>
#include <cstring>
#include <iomanip>

Book::Book(long long Isbn, const char *Title, const char *Author, int Total, int Remain)
	: isbn(Isbn)
{
	strncpy(title, Title, sizeof(title));	title[sizeof(title)-1] = '\0';
	strncpy(author, Author, sizeof(author));	author[sizeof(author)-1] = '\0';
	total = Total;
	remain = (Remain < 0) ? Total : Remain;
}

void Book::Set(long long Isbn, const char *Title, const char *Author, int Total, int Remain)
{
	isbn = Isbn;
	strncpy(title, Title, sizeof(title));	title[sizeof(title)-1] = '\0';
	strncpy(author, Author, sizeof(author));	author[sizeof(author)-1] = '\0';
	total = Total;
	remain = Remain;
}

bool Book::Lend()
{
	if (remain <= 0)
		return false;
	remain--;
	return true;
}

bool Book::GiveBack()
{
	if (remain >= total)
		return false;
	remain++;
	return true;
}

ostream & operator<<(ostream &out, const Book &b)
{
	out << b.isbn << '\t' << b.title << '\t' << b.author << '\t'
		<< b.total << '\t' << b.remain;
	return out;
}

istream & operator>>(istream &in, Book &b)
{
	char str[80];
	in.getline(str, 80, '\t');	b.isbn = atoll(str);
	in.getline(str, 80, '\t');	strncpy(b.title, str, sizeof(b.title));   b.title[sizeof(b.title)-1] = '\0';
	in.getline(str, 80, '\t');	strncpy(b.author, str, sizeof(b.author)); b.author[sizeof(b.author)-1] = '\0';
	in.getline(str, 80, '\t');	b.total  = atoi(str);
	in.getline(str, 80);		b.remain = atoi(str);
	return in;
}
