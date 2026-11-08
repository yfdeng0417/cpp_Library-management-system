// Book.cpp  图书数据域类的实现
// 全部函数都很短，重点在两处：初始化列表里的 Remain 约定，
// 以及 operator<< / operator>> 这一对必须严格对称的存盘函数。
#include "Book.h"
#include <cstdlib>
#include <cstring>
#include <iomanip>

// 初始化列表给 isbn 赋值，其余成员在函数体内赋值。
//     Remain < 0 是"未指定在架数"的约定，此时让在架数等于馆藏总量。
Book::Book(long long Isbn, const char *Title, const char *Author, int Total, int Remain)
	: isbn(Isbn)
{
	// strncpy 只保证最多拷贝 n 个字符，源串更长时不会自动补 '\0'，
	//     所以每次都要手动把最后一个位置置零，否则 cout 会一路读到数组外面去。
	//     这里写 sizeof(title) 而不是 41，是为了让长度始终跟声明保持一致。
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

// 库存判断放在数据域类里，业务层就不必知道 remain 的边界条件。
bool Book::Lend()
{
	if (remain <= 0)
		return false;
	remain--;
	return true;
}

// remain >= total 说明已经还够了，再加就超过馆藏总量，直接拒绝。
bool Book::GiveBack()
{
	if (remain >= total)
		return false;
	remain++;
	return true;
}

// 存盘格式：只写纯数据，字段之间用 '\t' 分隔，行尾换行。
//     这里绝不能改去调用显示函数或加中文标签——
//     存盘格式是给下面的 operator>> 读的，掺进文字后 atoi 会得到 0，
//     数据会在第一次存盘时静默变成 0 或空。给人看的格式见 Reader::Show。
ostream & operator<<(ostream &out, const Book &b)
{
	out << b.isbn << '\t' << b.title << '\t' << b.author << '\t'
		<< b.total << '\t' << b.remain;
	return out;
}

// 与 operator<< 严格对称：按 '\t' 逐段取出，再转成对应类型。
//     getline(str, 80, '\t') 以制表符为界；最后一段后面没有制表符，只给两个参数。
//     ISBN 用 atoll 而不是 atoi：13 位已经超出 int 的范围。
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
