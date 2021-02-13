#include "test.hpp"

// A::A(A::B B_s, A::C C_s) : B_p(B_s), C_p(C_s)
// {
// }
// A::A(std::__cxx11::string B_contents, std::__cxx11::string C_contents) : B_p(B_contents),C_p(C_contents)
// {
// }
// 

A::A()
{	
}

A::B::B(std::__cxx11::string delimiter) : delimiter(delimiter)
{
}

A::C::C(std::__cxx11::string delimiter) : delimiter(delimiter)
{
}





Aa::B::B() : A::B("Aa::B_del correct")
{
}

Aa::C::C() : A::C("Aa::C_del correct")
{
}


Aa::Aa()
{
	A::B_p = &B_p;
	A::C_p = &C_p;
}

void A::B::pop_it(int c)
{
	for (int i=0;i<c;i++)
		pop.push_back(i);
}
