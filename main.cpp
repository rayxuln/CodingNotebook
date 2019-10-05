/*
	Desciption: 递归下降法计算数学表达式
	Arthor: Rayxuln
	Date: 2019.10.5

	数学表达式定义:
		_Expr     -> _Term0    | _Expr + _Term0  | _Expr - _Term0
		_Term0    -> _Term1    | _Term0 * _Term1 | _Term0 / _Term1
		_Term1    -> _Term2    | _Term1 ^ _Term2
		_Term2    -> _Real     | ( _Expr )       | _Func ( _Args )

		_Real     -> _Positive | - _Positive     | + _Positive
		_Positive -> _Interger | _Interger . _Interger
		_Interger -> digital   | _Interger digital
	
		_Func     -> letter    | _Func letter    | _Func digital
		_Args     -> _Expr     | _Args , _Expr
*/
#include <iostream>
#include <string>
#include <iterator>
#include <map>
#include <functional>
#include <stack>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>

using namespace std;

typedef long double Real;
typedef long long IReal;

enum LexerType{
	OPT,
	REAL,
	FUNC
};

class LexerValue{
public:
	char o;//操作符
	Real r;//实数
	string f;//函数名
};

//表示一个词素，储存这个词的类型以及数据
typedef pair<LexerType, LexerValue> Lexer;

typedef function<Real(const vector<Real> &)> ExpFunc;

class FuncDefinition{
	
	ExpFunc func;
public:
	int argNum;

	Real operator() (const vector<Real> &args)
	{
		return func(args);
	}

	FuncDefinition(){argNum=0;}
	FuncDefinition(int _n, ExpFunc _f):argNum(_n),func(_f){}
	~FuncDefinition(){}
};

class Exp{

	//后序表达式栈
	stack<Lexer> exp;

	map<char, function<Real(Real, Real)>> opt_funcs = {
		{'+', [](Real a, Real b){return a + b;}},
		{'-', [](Real a, Real b){return a - b;}},
		{'*', [](Real a, Real b){return a * b;}},
		{'/', [](Real a, Real b){return a / b;}},
		{'%', [](Real a, Real b){return IReal(a) % IReal(b);}},
		{'^', [](Real a, Real b){return pow(a, b);}}
	};

	map<string, FuncDefinition> exp_funcs = {
		{"increase", FuncDefinition(1,
			[](const vector<Real> &args) -> Real
			{
				return args[0] + 1;
			})
		},
		{"decrease", FuncDefinition(1,
			[](const vector<Real> &args) -> Real
			{
				return args[0] - 1;
			})
		},
		{"clock", FuncDefinition(0,
			[](const vector<Real> &args) -> Real
			{
				return clock();
			})
		},
		{"srand", FuncDefinition(1,
			[](const vector<Real> &args) -> Real
			{
				srand(args[0]);
				return 0;
			})
		},
		{"rand", FuncDefinition(0,
			[](const vector<Real> &args) -> Real
			{
				return rand();
			})
		},
		{"sin", FuncDefinition(1,
			[](const vector<Real> &args) -> Real
			{
				return sin(args[0]);
			})
		},
		{"cos", FuncDefinition(1,
			[](const vector<Real> &args) -> Real
			{
				return cos(args[0]);
			})
		},
		{"tan", FuncDefinition(1,
			[](const vector<Real> &args) -> Real
			{
				return tan(args[0]);
			})
		},
		{"pow", FuncDefinition(2,
			[](const vector<Real> &args) -> Real
			{
				return pow(args[0], args[1]);
			})
		}
	};

	
public:

	//简单执行后序表达式求值算法即可
	Real Eval()
	{
		stack<Lexer> result;
		while(!exp.empty())
		{
			auto lexer = exp.top();exp.pop();
			//cout<<"<"<<(lexer.first == LexerType::OPT ? "OPT" : "REAL")<<","<<lexer.second<<">"<<endl;
			if(lexer.first == LexerType::OPT)
			{
				if(result.size() < 2)
				{
					cout<<"Not enough argument for operator '"<<lexer.second.o<<"'"<<endl;
					throw *this;
				}
				auto b = result.top();result.pop();
				auto a = result.top();result.pop();
				if(a.first != LexerType::REAL || b.first != LexerType::REAL)
				{
					throw *this;
				}
				auto c = opt_funcs[lexer.second.o](a.second.r, b.second.r);
				//cout<<">"<<c<<"<"<<endl;
				LexerValue v;
				v.r = c;
				result.push(make_pair(LexerType::REAL, v));
			}else if(lexer.first == LexerType::FUNC)
			{
				string funcName = lexer.second.f;
				auto find_func = exp_funcs.find(funcName);
				if(find_func != exp_funcs.end())
				{
					auto &theFunc = find_func->second;

					auto argNum = theFunc.argNum;
					vector<Real> args;

					while(argNum--)
					{
						if(result.empty())
						{
							cout<<"Not enough argument for '"<<funcName<<"'"<<endl;
							throw *this;
						}
						auto a = result.top();result.pop();
						if(a.first != LexerType::REAL)
						{
							cout<<"Wrong argument!"<<endl;
							throw *this;
						}
						args.insert(args.begin(), a.second.r);
						
					}
					auto c = theFunc(args);//执行函数
					LexerValue v;
					v.r = c;
					result.push(make_pair(LexerType::REAL, v));
				}else
				{
					cout<<"Unknown function name! '"<<funcName<<"'"<<endl;
					throw *this;
				}
			}else
			{
				result.push(lexer);
			}
		}
		return result.top().second.r;
	}

	friend class ExpParser;

	Exp(){}
	~Exp(){}
};

class ExpParser{
	string::iterator next, end;

	Exp theExp;

	string opts_0 = "+-";
	string opts_1 = "*/%";
	string opts_2 = "^";

	Real StrToReal(string s)
	{
		stringstream ss;
		ss<<s;
		Real x;
		ss>>x;
		return x;
	}
	string RealToStr(Real x)
	{
		stringstream ss;
		ss<<x;
		string s;
		ss>>s;
		return s;
	}

	

	bool IsDigit(char t)
	{
		return t >= '0' && t <= '9';
	}

	bool IsLetter(char t)
	{
		return (t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || t == '_';
	}

	//表达式
	void _Expr()
	{
		_Term0();
		while(true)
		{
			auto find_opt = opts_0.find(*next);
			if(find_opt != string::npos)
			{
				auto op = opts_0[find_opt];
				_Match(op);
				_Term0();
				output += op;
				LexerValue lex;
				lex.o = op;
				theExp.exp.push(make_pair(LexerType::OPT, lex));
			}else if(*next == '.')
			{
				cout<<"Too many '.'!"<<endl;
				throw theExp;
			}else
			{
				return;
			}
		}
	}


	//0级优先级二元操作符
	void _Term0()
	{
		_Term1();
		while(true)
		{
			auto find_opt = opts_1.find(*next);
			if(find_opt != string::npos)
			{
				auto op = opts_1[find_opt];
				_Match(op);
				_Term1();
				output += op;
				LexerValue lex;
				lex.o = op;
				theExp.exp.push(make_pair(LexerType::OPT, lex));
			}else
			{
				return;
			} 
		}
	}

	//1级优先级二元操作符
	void _Term1()
	{
		_Term2();
		while(true)
		{
			auto find_opt = opts_2.find(*next);
			if(find_opt != string::npos)
			{
				auto op = opts_2[find_opt];
				_Match(op);
				_Term2();
				output += op;
				LexerValue lex;
				lex.o = op;
				theExp.exp.push(make_pair(LexerType::OPT, lex));
			}else
			{
				return;
			} 
		}
	}

	//2级优先级二元操作符
	void _Term2()
	{
		if(IsDigit(*next) || (next + 1 != end && (*next == '-' || *next == '+') && IsDigit(*(next + 1))))
		{
			output += '<';
			auto b = output.size();
			_Real();
			string real(output.begin() + b, output.end());
			LexerValue lex;
			lex.r = StrToReal(real);
			theExp.exp.push(make_pair(LexerType::REAL, lex));
			output += '>';
		}else if(*next == '(')
		{
			_Match('(');
			_Expr();
			_Match(')');
		}else if(IsLetter(*next))
		{
			auto b = output.size();
			_Func();
			string func_name(output.begin() + b, output.end());
			output += '(';
			_Match('(');
			_Args();
			_Match(')');
			output += ')';
			LexerValue lex;
			lex.f = func_name;
			theExp.exp.push(make_pair(LexerType::FUNC, lex));
		}
	}


	//实数
	void _Real()
	{
		if(*next == '-')
		{
			_Match('-');
			output += '-';
			_Positive();
		}if(*next == '+')
		{
			_Match('+');
			_Positive();
		}
		else
		{
			_Positive();
		}
	}

	//正数
	void _Positive()
	{
		_Interger();
		if(*next == '.')
		{
			output += '.';
			_Match('.');
			_Interger();
		}
	}

	//正整数
	void _Interger()
	{
		while(IsDigit(*next))
		{
			output += *next;
			_Match(*next);
		}
	}

	//函数名
	void _Func()
	{
		if(IsLetter(*next))
		{
			char l = *next;
			output += l;
			_Match(l);

			while(IsLetter(*next) || IsDigit(*next))
			{
				output += *next;
				_Match(*next);
			}
		}
	}

	//函数的参数
	void _Args()
	{
		while(true)
		{
			_Expr();
			if(__Match(','))
			{
				output += ',';
			}else
			{
				break;
			}
		};
	}

	bool __Match(char t)
	{
		while(*next == ' ')++next;
		if(*next == t)
		{
			do{
				++next;
			}while(*next == ' ');
			return true;
		}
		return false;
	}


	void _Match(char t)
	{
		if(!__Match(t))
		{
			theExp.exp = stack<Lexer>();
			cout<<"\nMatch ( '"<<t<<"' with '"<<*next<<"' ) fail!"<<endl;
			throw theExp;
			return;
		}
	}



public:

	string output;

	string::iterator GetNext()
	{
		return next;
	}

	//解析中序表达式字符串，产生后序表达式栈
	Exp Parse(string::iterator _t, string::iterator _e)
	{
		next = _t;
		end = _e;

		output = string();
		theExp.exp = stack<Lexer>();
		_Expr();

		//翻转栈
		stack<Lexer> exp1;
		while(!theExp.exp.empty()) exp1.push(theExp.exp.top()), theExp.exp.pop();
		theExp.exp.swap(exp1);

		return theExp;
	}

	ExpParser(){}
	~ExpParser(){}
};



void Calculate(string exp)
{
	cout<<"exp: "<<exp<<endl;

	ExpParser p;
	try{
		auto theExp = p.Parse(exp.begin(), exp.end());

		cout<<"parser: "<<p.output<<endl;
		cout<<"result: "<<theExp.Eval()<<endl;
	}catch (Exp e)
	{
		cout<<"parse this exp fail, please check the format of it."<<endl;
		cout<<"This fetal error happened at:"<<endl;
		cout<<"================="<<endl;
		cout<<exp<<endl;
		int n = p.GetNext() - exp.begin();
		for(int i=0;i<n;++i) cout<<" ";
		cout<<"^"<<endl;
		cout<<"================="<<endl;
	}
	
}

int main()
{
	string exp;
	getline(cin, exp);
	Calculate(exp);

	return 0;
}

