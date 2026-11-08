#ifndef READER_H
#define READER_H

#include "LinkList.h"
#include "BorrowRecord.h"

class Reader
{
public:
	Reader(int Id = 0, const char *Name = "", const char *Phone = "");

	operator int()    const { return id;   }
	operator string() const { return name; }

	int  Id()    const { return id;    }
	const char * Name()  const { return name;  }
	const char * Phone() const { return phone; }

	void AppendRecord(const BorrowRecord &r) { records.Append(r); }
	int  NumRecords()    const { return records.NumNodes(); }
	int  NumUnreturned() const;

	bool GiveBackRecord(long long isbn, int y, int m, int d);

	Node<BorrowRecord> * RecordsTop()  { return records.GoTop();  }
	Node<BorrowRecord> * RecordsNext() { return records.Skip(1);  }
	BorrowRecord       & CurRecord()   { return records.CurData(); }

	void Show(ostream &out) const;

	friend ostream & operator<<(ostream &out, const Reader &r);
	friend istream & operator>>(istream &in,  Reader &r);

private:
	int id;
	char name[21], phone[12];
	LinkList<BorrowRecord> records;
};

#endif
