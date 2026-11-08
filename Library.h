// Library.h  业务层
// 把两条链表组织在一起，负责那些必须"同时操作两条链表"的功能。
// Library 本身不属于三层结构中的任何一层。
#ifndef LIBRARY_H
#define LIBRARY_H

#include "LinkList.h"
#include "Reader.h"
#include "Book.h"

// 业务层。读者链表与图书链表各一条，一起构成整个系统。
class Library
{
public:
	Library();

	// 两层结构的存盘与读盘：读者文件里每位读者后面紧跟着自己的流水，
	//     格式是嵌套的；图书文件是平铺的单层结构。
	//     文件名给了默认值，所以 main 里可以直接写 lib.Load()。
	int Load(const char *readerFile = "Readers.txt", const char *bookFile = "Books.txt");
	int Save(const char *readerFile = "Readers.txt", const char *bookFile = "Books.txt");

	void ShowReaders();
	void ShowBooks();

	void AddReader(const Reader &r) { readers.Append(r); }
	void AddBook(const Book &b)     { books.Append(b);   }

	// 一次借还同时牵动两条链表，所以返回值只表示"整体成功或失败"，
	//     内部保证不会留下"库存已改、流水没改"这种半截状态。
	bool Lend(int readerId, long long isbn, int y, int m, int d);
	bool GiveBack(int readerId, long long isbn, int y, int m, int d);

	int  FindReader(int id);
	int  FindReader(const char *name);
	int  FindBook(long long isbn);
	int  FindBook(const char *title);
	bool ShowOneReader(int id);

	// today 由调用方传入而不是内部取系统时间：
	//     一次统计里所有记录必须用同一个"今天"；
	//     测试时也能指定任意日期，不必改系统时钟。
	int  OverdueList(int today);

	void SortReadersById(bool asc = true);
	void SortReadersByName(bool asc = true);
	void SortBooksByIsbn(bool asc = true);
	void ReverseReaders() { readers.Reverse(); }

	// 暴露链表本身，供 main 直接调用模板的 Sort / Reverse 等操作。
	LinkList<Reader> & Readers() { return readers; }
	LinkList<Book>   & Books()   { return books;   }

private:
	// 两条链表：外层是读者，图书另起一条平铺的链表。
	LinkList<Reader> readers;
	LinkList<Book>   books;
};

#endif
