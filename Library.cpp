#include "Library.h"
#include <iomanip>

Library::Library()
{
}

int Library::Load(const char *readerFile, const char *bookFile)
{
	readers.FreeList();
	books.FreeList();
	int n = readers.Load(readerFile);
	n += books.Load(bookFile);
	return n;
}

int Library::Save(const char *readerFile, const char *bookFile)
{
	int n = readers.Save(readerFile);
	books.Save(bookFile);
	return n;
}

void Library::ShowReaders()
{
	cout << "\n========== 读者链表（共 " << readers.NumNodes() << " 个结点）==========" << endl;
	for (Node<Reader> *p = readers.GoTop(); p != NULL; p = readers.Skip(1))
	{
		cout << "----------------------------------------" << endl;
		readers.CurData().Show(cout);
	}
}

void Library::ShowBooks()
{
	cout << "\n========== 图书链表（共 " << books.NumNodes() << " 个结点）==========" << endl;
	cout << "ISBN\t\t题名\t\t作者\t总量\t在架" << endl;
	books.PutList(cout);
}

bool Library::Lend(int readerId, long long isbn, int y, int m, int d)
{
	Node<Book> *pb = books.Locate(isbn, true);
	if (pb == NULL)
		return false;
	Book &b = books.CurData();
	if (!b.Lend())
		return false;
	Node<Reader> *pr = readers.Locate(readerId, true);
	if (pr == NULL)
	{
		b.GiveBack();
		return false;
	}
	Reader &r = readers.CurData();
	r.AppendRecord(BorrowRecord(isbn, y, m, d, 30));
	return true;
}

bool Library::GiveBack(int readerId, long long isbn, int y, int m, int d)
{
	if (books.Locate(isbn, true) == NULL)
		return false;
	if (readers.Locate(readerId, true) == NULL)
		return false;
	if (!readers.CurData().GiveBackRecord(isbn, y, m, d))
		return false;
	books.CurData().GiveBack();
	return true;
}

int Library::FindReader(int id)
{
	int n = 0;
	Node<Reader> *p = readers.Locate(id, true);
	while (p != NULL)
	{
		n++;
		readers.CurData().Show(cout);
		p = readers.Locate(id);
	}
	if (n == 0)	cout << "没有学号为 " << id << " 的读者" << endl;
	else		cout << "找到 " << n << " 位读者" << endl;
	return n;
}

int Library::FindReader(const char *name)
{
	int n = 0;
	Node<Reader> *p = readers.Locate(string(name), true);
	while (p != NULL)
	{
		n++;
		readers.CurData().Show(cout);
		p = readers.Locate(string(name));
	}
	if (n == 0)	cout << "没有姓名为 " << name << " 的读者" << endl;
	else		cout << "找到 " << n << " 位读者" << endl;
	return n;
}

int Library::FindBook(long long isbn)
{
	int n = 0;
	Node<Book> *p = books.Locate(isbn, true);
	while (p != NULL)
	{
		n++;
		cout << books.CurData() << endl;
		p = books.Locate(isbn);
	}
	if (n == 0)	cout << "没有 ISBN 为 " << isbn << " 的图书" << endl;
	else		cout << "找到 " << n << " 种图书" << endl;
	return n;
}

int Library::FindBook(const char *title)
{
	int n = 0;
	Node<Book> *p = books.Locate(string(title), true);
	while (p != NULL)
	{
		n++;
		cout << books.CurData() << endl;
		p = books.Locate(string(title));
	}
	if (n == 0)	cout << "没有题名为 " << title << " 的图书" << endl;
	else		cout << "找到 " << n << " 种图书" << endl;
	return n;
}

bool Library::ShowOneReader(int id)
{
	if (readers.Locate(id, true) == NULL)
	{
		cout << "没有学号为 " << id << " 的读者" << endl;
		return false;
	}
	readers.CurData().Show(cout);
	return true;
}

int Library::OverdueList(int today)
{
	int n = 0;
	for (Node<Reader> *pr = readers.GoTop(); pr != NULL; pr = readers.Skip(1))
	{
		Reader &r = readers.CurData();
		for (Node<BorrowRecord> *pb = r.RecordsTop(); pb != NULL; pb = r.RecordsNext())
			if (r.CurRecord().Overdue(today))
			{
				n++;
				cout << r.Name() << " 学号 " << r.Id() << " 逾期：" << r.CurRecord() << endl;
			}
	}
	return n;
}

void Library::SortReadersById(bool asc)		{ readers.Sort(int(0),     asc); }
void Library::SortReadersByName(bool asc)	{ readers.Sort(string(""),  asc); }
void Library::SortBooksByIsbn(bool asc)		{ books.Sort((long long)0, asc); }
