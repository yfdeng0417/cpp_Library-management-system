// BorrowRecord.h  借阅流水数据域类
// 一条流水记录"借了什么书、什么时候借的、该什么时候还、还了没有"。
// 它同时也是内层链表的元素类型：Reader 里那条 LinkList<BorrowRecord>。
#ifndef BORROWRECORD_H
#define BORROWRECORD_H

#include <iostream>
#include <string>
using namespace std;

// 借阅流水。日期用三个 int 分开保存，比较与运算时再折算成
//     YYYYMMDD 形式的整数，存盘可读性最好，也不受 time_t 位数影响。
class BorrowRecord
{
public:
	// 默认实参让"一条空流水"可以被默认构造出来——
	//     模板 GetList 里那句 `T x;` 正是靠它构造临时对象的。
	BorrowRecord(long long Isbn = 0, int Y = 0, int M = 0, int D = 0, int Days = 30);
	void Set(long long Isbn, int Y, int M, int D, int Days);
	// 还书只改一条流水，所以单独提供 SetReturned，
	//     而不是要求调用方拼一个完整的新对象再整条替换。
	void SetReturned(int Y, int M, int D);

	// 到期日与超期判断留在数据域类内部，日期怎么算对外不可见，
	//     业务层只需要问"这一天超期了吗"。
	int  DueDate()  const;
	bool Overdue(int today) const;

	long long Isbn() const { return isbn; }
	bool IsReturned() const { return returned; }

	// 只提供一个转换运算符：内层链表只需要按 ISBN 查找。
	//     如果这里再加 operator int 或 operator string，
	//     Locate 里的 TYPE(p->data) 就会有两种可行转换，编译时报二义性。
	operator long long() const { return isbn; }

	friend ostream & operator<<(ostream &out, const BorrowRecord &r);
	friend istream & operator>>(istream &in,  BorrowRecord &r);

// 归还日期只在 returned 为 true 时有意义，未归还时三个分量保持 0。
private:
	long long isbn;
	int y, m, d;
	int days;
	bool returned;
	int ry, rm, rd;
};

#endif
