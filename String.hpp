#include <iostream>

typedef long long SizeType;

class String {
	char *data;
	char *cdata;
	SizeType size;

	SizeType _CntCStr(const char *_cs) {
		SizeType res = 0;
		while (_cs[res] != '\0')
			res += 1;
		return res;
	}
	void _SafeReleaseCData() {
		if (cdata) {
			delete[] cdata;
			cdata = nullptr;
		}
	}

	// void _GetNext(String &p, SizeType next[]) {
	// 	using std::cout;
	// 	using std::endl;
	// 	next[0] = -1;
	// 	auto pn = p.Size();
	// 	SizeType i = 0, j = -1;
	// 	while (i < pn) 
	// 	{
	// 		if (j == -1 || p[i] == p[j]) {
	// 			i += 1;
	// 			j += 1;
	// 			//if(p[i] != p[j])
	// 				next[i] = j;
	// 			//else
	// 			//	next[i] = next[j];
	// 		} else {
	// 			j = next[j];
	// 		}
	// 	}
	// }
	void _GetNext(String &p, SizeType next[]) {
		using std::cout;
		using std::endl;
		next[0] = 0;
		auto pn = p.Size();
		SizeType i = 1, j = 0;
		while (i < pn) 
		{
			if (p[i] == p[j]) {
				//if(p[i] == p[next[j]])
				//	next[i] = next[j];
				//else
					next[i] = j + 1;
				i += 1;
				j += 1;
			} else {
				if(j)
					j = next[j - 1];
				else
					i += 1;
			}
		}
	}

	SizeType _KMP(String &p) {
		using std::cout;
		using std::endl;

		if (p.Size() > size)
			return -1;
		auto &t = *this;
		auto pn = p.Size();
		auto tn = size;
		auto next = new SizeType[pn];
		_GetNext(p, next);
		cout<<"next = {\n\t";
		for (int i = 0; i < pn; ++i)
		{
			cout<<next[i];
			if(i < pn-1)
				cout<<',';
		}
				
		cout<<"\n}\n";
		cout<<endl;
		SizeType i = 0, j = 0;
		SizeType res = -1;

		cout<<"=======KMP算法=============="<<endl;
		int step = 0;
		while (i < tn) {
			step += 1;
			cout << endl;
			// for(int k=0;k<tn;++k)
			// {
			// 	if(k==i)
			// 	{
			// 		cout<<"\033[7m"<<t[k]<<"\033[0m";
			// 	}else
			// 	{
			// 		cout<<t[k];
			// 	}
			// }
			// cout<<endl;
			// for(int k=0;k<pn;++k)
			// {
			// 	if(k==j)
			// 	{
			// 		cout<<"\033[7m"<<p[k]<<"\033[0m";
			// 	}else
			// 	{
			// 		cout<<p[k];
			// 	}
			// }
			// cout<<endl;
			cout << CStr() << endl;
			for (int k = 0; k < i; ++k)
				cout << (k < i - j ? ' ' : '~');
			cout << '^' << endl;
			cout << p.CStr() << endl;
			for (int k = 0; k < j; ++k)
				cout << ' ';
			cout << '^' << endl;
			cout << endl;

			// if (j == -1 || t[i] == p[j]) {
			// 	i += 1;
			// 	j += 1;
			// } else 
			// {
			// 	j = next[j];
			// }
			if (t[i] == p[j]) {
				i += 1;
				j += 1;
			} else 
			{
				if(j)
					j = next[j-1];
				else
					i += 1;
			}


			if (j == pn) {
				res = i - j;
				break;
			}
		}
		delete[] next;

		cout<<"Steps: "<<step<<endl;
		cout<<"Result: "<<res<<endl;
		cout<<"=========================="<<endl<<endl;
		return res;
	}

	SizeType _SimpleFind(String &p)
	{
		using std::cout;
		using std::endl;

		SizeType res = -1;
		auto &t = *this;
		auto pn = p.Size();
		auto tn = size;
		SizeType i = 0;
		SizeType j = 0;

		cout<<"=======朴素算法=============="<<endl;
		int step = 0;
		while(i < tn)
		{
			step += 1;
			cout << endl;
			cout << CStr() << endl;
			for (int k = 0; k < i; ++k)
				cout << (k < i - j ? ' ' : '~');
			cout << '^' << endl;
			cout << p.CStr() << endl;
			for (int k = 0; k < j; ++k)
				cout << ' ';
			cout << '^' << endl;
			cout << endl;
			if(t[i] == p[j])
			{
				i += 1;
				j += 1;
			}else
			{
				i = i - j + 1;
				j = 0;
			}

			if(j == pn)
			{
				res = i - j;
				break;
			}
		}
		cout<<"Steps: "<<step<<endl;
		cout<<"Result: "<<res<<endl;
		cout<<"=========================="<<endl<<endl;
		return res;
	}

public:
	SizeType Size() { return size; }

	const char *CStr() {
		_SafeReleaseCData();
		cdata = new char[size + 1];
		for (SizeType i = 0; i < size; ++i) {
			cdata[i] = data[i];
		}
		cdata[size] = '\0';
		return cdata;
	}

	SizeType Find(String &p) 
	{ 
		return _KMP(p), _SimpleFind(p); 
	}
	//SizeType Find(String &p) { return _SimpleFind(p); }

	const char operator[](SizeType i) { return data[i]; }

	String(const char *_cs) : cdata(nullptr) {
		size = _CntCStr(_cs);
		data = new char[size];
		for (int i = 0; i < size; ++i) {
			data[i] = _cs[i];
		}
	}
	String() : data(nullptr), cdata(nullptr), size(0) {}
	~String() { _SafeReleaseCData(); }
};

std::ostream &operator<<(std::ostream &out, String &s) {
	out << s.CStr();
	return out;
}

#define STRING_MAX_NUM 4096
std::istream &operator>>(std::istream &in, String &s) {
	char buf[STRING_MAX_NUM];
	in >> buf;
	s = String(buf);
	return in;
}
