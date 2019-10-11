#include <iostream>

#include "String.hpp"


using namespace std;









int main()
{
	String t;
	String p;
	cin>>t>>p;
	cout<<"Target("<<t.Size()<<"):"<<endl<<t<<endl<<"Pattern("<<p.Size()<<"):"<<endl<<p<<endl;
	auto r = t.Find(p);
	cout<<"Index:"<<r<<endl;


	return 0;
}