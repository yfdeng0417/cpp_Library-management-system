// Reader.h  读者数据域类
// 这里藏着整个项目的核心：读者对象内部又挂着一条链表，
// 于是"外层读者链表的结点里装着一条完整的链表"，构成链表的链表。
#ifndef READER_H
#define READER_H

#include "LinkList.h"
#include "BorrowRecord.h"

// 读者。除了学号、姓名、电话，还持有一条借阅流水链表 records，
//     这就是题目要求的"链表的链表"的落点。
class Reader
{
public:
	Reader(int Id = 0, const char *Name = "", const char *Phone = "");

	// 两个转换运算符：学号转 int，姓名转 string。
	//     于是 readers.Sort(int(0), asc) 按学号排，
	//          readers.Sort(string(""), asc) 按姓名排，
	//     模板的 Sort 一行都不用改。
	operator int()    const { return id;   }
	operator string() const { return name; }

	int  Id()    const { return id;    }
	const char * Name()  const { return name;  }
	const char * Phone() const { return phone; }

	// 业务层只能"追加一条流水"，不能替换或清空整条内层链表。
	void AppendRecord(const BorrowRecord &r) { records.Append(r); }
	int  NumRecords()    const { return records.NumNodes(); }
	int  NumUnreturned() const;

	bool GiveBackRecord(long long isbn, int y, int m, int d);

	// 三个受限入口：定位首结点、后移一个结点、取当前结点的数据。
	//     内层链表是私有成员，外部拿不到 LinkList 本身，只能这样一个个走过去。
	//     这种"只给动作、不给结构"的做法，保证嵌套结构不会被整体替换。
	//
	//     注意 CurRecord 的返回类型是引用（BorrowRecord &）：
	//     调用方拿到的就是链表里那个对象本身，可以直接改。
	//     若去掉 & 返回的是一份拷贝，改了也回不到链表里。
	Node<BorrowRecord> * RecordsTop()  { return records.GoTop();  }
	Node<BorrowRecord> * RecordsNext() { return records.Skip(1);  }
	BorrowRecord       & CurRecord()   { return records.CurData(); }

	void Show(ostream &out) const;

	friend ostream & operator<<(ostream &out, const Reader &r);
	friend istream & operator>>(istream &in,  Reader &r);

// records 私有，是"链表的链表"能保持一致性的前提。
private:
	int id;
	char name[21], phone[12];
	LinkList<BorrowRecord> records;
};

#endif
