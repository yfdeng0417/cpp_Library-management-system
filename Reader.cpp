// Reader.cpp  读者数据域类的实现
// 重点看三处：const 成员函数里的 const_cast、
// 还书时为什么要连续 Locate、以及显示格式与存盘格式为什么要分开写。
#include "Reader.h"
#include <cstring>
#include <iomanip>

// 数组成员在函数体里赋值，同样要手动补 '\0'，理由与 Book 一致。
Reader::Reader(int Id, const char *Name, const char *Phone) : id(Id)
{
	strncpy(name,  Name,  sizeof(name));   name[sizeof(name)-1]   = '\0';
	strncpy(phone, Phone, sizeof(phone));  phone[sizeof(phone)-1] = '\0';
}

// 本函数声明为 const，records 在这里是常量，
//     而 LinkList 的遍历接口 GoTop / Skip 没有 const 版本（它们会改动"当前结点"），
//     所以先用 const_cast 去掉 records 的 const 才能调用。
//     这是唯一允许这样做的场合：遍历只改了当前结点指针，没有改动任何数据。
//     更干净的解法是给 LinkList 增加一组 const 遍历接口，
//     但那要改教师提供的模板，与本项目"模板一行不改"的约束冲突。
int Reader::NumUnreturned() const
{

	LinkList<BorrowRecord> &rl = const_cast<LinkList<BorrowRecord> &>(records);

	int n = 0;
	for (Node<BorrowRecord> *p = rl.GoTop(); p != NULL; p = rl.Skip(1))
		if (!rl.CurData().IsReturned())
			n++;
	return n;
}

// 同一本书可能被借过多次，链表里会有多条 ISBN 相同的流水。
//     Locate 默认只返回"第一条匹配的结点"，如果那一条已经还过了，
//     再还一次就会把已还的记录重写一遍，库存也跟着虚增。
//     所以这里连续调用 Locate：跳过所有已还的流水，直到找到真正未还的那一条。
//     第一轮传 newsearch = true 从头开始，之后省略该参数，靠静态状态接着上次的位置续查。
bool Reader::GiveBackRecord(long long isbn, int y, int m, int d)
{
	Node<BorrowRecord> *p = records.Locate(isbn, true);
	while (p != NULL && records.CurData().IsReturned())
		p = records.Locate(isbn);
	if (p == NULL)
		return false;
	BorrowRecord rec = records.CurData();
	rec.SetReturned(y, m, d);
	records.ModifyData(rec);
	return true;
}

// 给人看的显示格式：中文标签、补零对齐，再附上借阅统计。
//     与下面的 operator<< 是两套格式，职责不同，不能合并。
void Reader::Show(ostream &out) const
{
	out << "学号：" << setfill('0') << setw(4) << id << setfill(' ')
		<< "\t姓名：" << name
		<< "\t电话：" << phone
		<< "\t借阅 " << NumRecords() << " 条，在借 " << NumUnreturned() << " 条" << endl;
	records.PutList(out);
}

// 存盘格式：只写纯数据，字段用 '\t' 分隔，行尾换行，
//     随后把内层流水链表整体写出去（PutList 会先写结点个数）。
//     如果这里偷懒去调 Show()，写进文件的就是"学号：0001"这样的字符串，
//     读回时 atoi("学号：0001") 得 0，全部数据会静默变成 0。
//     显示格式是给人看的，存储格式是给机器读的，两者必须分开。
ostream & operator<<(ostream &out, const Reader &r)
{
	out << r.id << '\t' << r.name << '\t' << r.phone << endl;
	r.records.PutList(out);
	return out;
}

// 读入前必须自己清空 records。
//     模板的 GetList 里那句 FreeList() 被注释掉了，而且整轮读入复用同一个
//     临时对象 x，不清空的话，上一位读者的流水会被累加到这一位身上。
istream & operator>>(istream &in, Reader &r)
{
	char str[80];
	in.getline(str, 80, '\t');	r.id = atoi(str);
	in.getline(str, 80, '\t');	strncpy(r.name,  str, sizeof(r.name));   r.name[sizeof(r.name)-1]  = '\0';
	in.getline(str, 80);		strncpy(r.phone, str, sizeof(r.phone));  r.phone[sizeof(r.phone)-1] = '\0';

	r.records.FreeList();
	r.records.GetList(in);
	return in;
}
