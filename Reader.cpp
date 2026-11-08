#include "Reader.h"
#include <cstring>
#include <iomanip>

Reader::Reader(int Id, const char *Name, const char *Phone) : id(Id)
{
	strncpy(name,  Name,  sizeof(name));   name[sizeof(name)-1]   = '\0';
	strncpy(phone, Phone, sizeof(phone));  phone[sizeof(phone)-1] = '\0';
}

int Reader::NumUnreturned() const
{

	LinkList<BorrowRecord> &rl = const_cast<LinkList<BorrowRecord> &>(records);

	int n = 0;
	for (Node<BorrowRecord> *p = rl.GoTop(); p != NULL; p = rl.Skip(1))
		if (!rl.CurData().IsReturned())
			n++;
	return n;
}

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

void Reader::Show(ostream &out) const
{
	out << "学号：" << setfill('0') << setw(4) << id << setfill(' ')
		<< "\t姓名：" << name
		<< "\t电话：" << phone
		<< "\t借阅 " << NumRecords() << " 条，在借 " << NumUnreturned() << " 条" << endl;
	records.PutList(out);
}

ostream & operator<<(ostream &out, const Reader &r)
{
	out << r.id << '\t' << r.name << '\t' << r.phone << endl;
	r.records.PutList(out);
	return out;
}

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
