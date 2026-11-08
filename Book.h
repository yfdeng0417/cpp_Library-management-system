// Book.h  图书数据域类
// 只描述"一种书"：ISBN、题名、作者、馆藏总量、在架数。
// 与 LinkList<T> 对接靠三样东西：四大函数、operator<< / >>、类型转换运算符。
#ifndef BOOK_H
#define BOOK_H

#include <iostream>
#include <string>
using namespace std;

// 图书数据域类。它只管"一种书"自身的数据与借还，不负责管理一堆书，
//    那是业务层 Library 与容器 LinkList<Book> 的事。
//    LinkList<T> 对数据域类只提三项要求，这个类三条都满足：
//      一、四大函数行为正确——成员全是定长数组与整数，编译器生成的版本就够用；
//      二、重载 operator<< 与 operator>>，供模板的 PutList / GetList 调用；
//      三、能参与关系运算——本项目用下面的类型转换运算符实现。
class Book
{
public:
	// 构造函数带默认实参，同时充当默认构造函数与转换构造函数：
	//          Book b;                     // 全部取默认值
	//          Book b(9787302500001);      // 只给 ISBN
	//      Remain 默认 -1 表示"未指定"，真正的处理在 Book.cpp 的初始化列表里。
	Book(long long Isbn = 0, const char *Title = "", const char *Author = "",
		 int Total = 1, int Remain = -1);
	void Set(long long Isbn, const char *Title, const char *Author, int Total, int Remain);

	// 两个改变库存的操作都返回 bool：能不能借、能不能还，
	//     由数据域类自己判断，业务层不必重复检查 remain 与 total 的关系。
	bool Lend();
	bool GiveBack();

	// 三个只读访问器都声明为 const——查询不改变图书对象本身。
	long long Isbn()  const { return isbn;  }
	int Total()       const { return total; }
	int Remain()      const { return remain;}

	// 两个类型转换运算符，这是整个项目复用模板的关键。
	//     LinkList<T>::Locate 与 Sort 内部靠 TYPE(p->data) 比较，
	//     调用时传什么类型，就转换成什么类型来比：
	//          books.Locate(isbn, true)           // isbn 是 long long，按 ISBN 查
	//          books.Locate(string(title), true)  // 传 string，按题名查
	//     模板一行都不用改，同一个类就支持了两种查找键和两种排序键。
	operator long long() const { return isbn;  }
	operator string()    const { return title; }

	// operator<< / operator>> 声明为友元而非成员：
	//     运算符的左操作数是 ostream / istream，只能写成非成员函数；
	//     非成员函数又访问不到私有成员，于是用 friend 授权。
	//     这两个函数只写纯数据，供存盘使用，与给人看的显示格式分开。
	friend ostream & operator<<(ostream &out, const Book &b);
	friend istream & operator>>(istream &in,  Book &b);

private:
	// 定长字符数组而不是 string，是为了与实验四教师给出的模板风格一致，
	//     存盘时直接写纯文本，不需要额外的（反）序列化代码。
	//     代价是超长会被截断，具体处理见 Book.cpp 的构造函数。
	long long isbn;
	char title[41], author[21];
	int total;
	int remain;
};

#endif
