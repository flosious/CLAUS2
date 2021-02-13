

#ifndef TEST_HPP
#define TEST_HPP

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class A
{
public:
	class B
	{
	public:
		vector<int> pop;
		string delimiter;
		void do_sth();
		B(string delimiter);
		void pop_it(int c);
	};
	class C
	{
	public:
		string delimiter;
		void do_sthelse();
		C(string delimiter);
	};
public:
	B* B_p = nullptr;
	C* C_p = nullptr;
// 	A(B B_s, C C_s);
	A();
};

class Aa : public A
{
public:
	class B : public A::B
	{
	public:
// 		A::B A_B();
		void do_sth_more();
		B();
	};
	class C : public A::C
	{
	public:
// 		A::C A_C();
		void do_sthelse_more();
		C();
	};
public:
	B B_p;
	C C_p;
	Aa();
};

#endif // TEST_HPP
