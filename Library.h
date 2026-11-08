#ifndef LIBRARY_H
#define LIBRARY_H

#include "LinkList.h"
#include "Reader.h"
#include "Book.h"

class Library
{
public:
	Library();

	int Load(const char *readerFile = "Readers.txt", const char *bookFile = "Books.txt");
	int Save(const char *readerFile = "Readers.txt", const char *bookFile = "Books.txt");

	void ShowReaders();
	void ShowBooks();

	void AddReader(const Reader &r) { readers.Append(r); }
	void AddBook(const Book &b)     { books.Append(b);   }

	bool Lend(int readerId, long long isbn, int y, int m, int d);
	bool GiveBack(int readerId, long long isbn, int y, int m, int d);

	int  FindReader(int id);
	int  FindReader(const char *name);
	int  FindBook(long long isbn);
	int  FindBook(const char *title);
	bool ShowOneReader(int id);

	int  OverdueList(int today);

	void SortReadersById(bool asc = true);
	void SortReadersByName(bool asc = true);
	void SortBooksByIsbn(bool asc = true);
	void ReverseReaders() { readers.Reverse(); }

	LinkList<Reader> & Readers() { return readers; }
	LinkList<Book>   & Books()   { return books;   }

private:
	LinkList<Reader> readers;
	LinkList<Book>   books;
};

#endif
