/*
	calculator08buggy.cpp

	Helpful comments removed.

	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
	
	Грамматика:
	
Инструкция:
	statement 
	Печать
	Выход

Печать:
	';' - print
Выход:
	'Q' - quit

statement:
	declaration
	expression
		


expression:
	term
	expression+term
	expression-term
term:
	primary
	term*primary
	term/primary
primary:
	"(" expression ")"
	"-"primary
	+primary
	number
	name
	
*/

#include "std_lib_facilities.h"

struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string n) :kind(ch), name(n) { }
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }
	Token get();
	void unget(Token t) { buffer=t; full=true; }
	void ignore(char);
};

const char let = 'L';
const char const_user = 'C';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';

Token Token_stream::get()
{
	if (full) { full=false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case print:
	case '=':
	case quit:
	case ',':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget();
		double val;
		cin >> val;
		return Token(number,val);
	}
	default:
		if (isalpha(ch) || ch == '_') {
			string s;
			s += ch;
			while(cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s+=ch;
			cin.unget();
			if (s == "let") return Token(let);	
			if (s == "exit") return Token(quit);
			if (s == "const") return Token(const_user);	
			return Token(name,s);
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c==buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin>>ch)
		if (ch==c) return;
}

struct Variable {
	string name;
	double value;
	char let_const;
	Variable(string n, double v, char c) :name(n), value(v), let_const(c) { }
};
vector<Variable> names;

class Symbol_table {
public:
	void set(string s, double d);
	double get(string s);
	bool is_declared(string s);
	double define(string var, double val, char c);
};

Symbol_table stab;


//vector<Variable> names;	

//double get_value(string s)
double Symbol_table::get(string s)
{
	for (int i = 0; i<names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ",s);
}

//void set_value(string s, double d)
void Symbol_table::set(string s, double d)
{
	for (int i = 0; i<=names.size(); ++i) {
		if (names[i].name == s && names[i].let_const == const_user) error("Нельзя изменять именнованные константы!");
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	}
	error("set: undefined name ",s);
}

//bool is_declared(string s)
bool Symbol_table::is_declared(string s)
{
	for (int i = 0; i<names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

//double define_name(string var, double val, char c)
double Symbol_table::define(string var, double val, char c)
// добавляем пару (var,val) в вектор names
{
	if (is_declared(var)) error(var," declared twice");
	names.push_back(Variable(var,val,c));
	return val;
}

Token_stream ts;

double sqrt1(double g){
	if (g < 0) error("Отрицательный аргумент квадратичного корня!");
	return sqrt(g);
}

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	
		double d = expression();
		t = ts.get();
		if (t.kind == ',') ts.unget(t);
		else if (t.kind != ')') error("')' expected1");
		return d;
	}
	case ',':
	{
		double d = expression();
		t = ts.get();
		if (t.kind != ')') error("')' expected2");
		return d;
	}
	case '-':
		return - primary();
	case number:
		return t.value;
	case name:
	{
		if (t.name == "sqrt") return sqrt1(expression());
		if (t.name == "pow") return pow(expression(), expression());
		string s = t.name;
		t  = ts.get();
		if(t.kind == '=') stab.set(s, expression());
		else ts.unget(t);
		return stab.get(s);
	}
	default:
		error("primary expected");
	}
}

double term()
{
	double left = primary();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
			if (d == 0) error("divide by zero");
			left /= d;
			break;
		}
		default:
			ts.unget(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

double declaration(char c)
{
	Token t = ts.get();
	if (t.kind != 'a') error ("name expected in declaration");
	string name = t.name;
	if (stab.is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of " ,name);
	double d = expression();
	names.push_back(Variable(name,d,c));
	return d;
}

double statement()
{
	Token t = ts.get();
	char let_const = t.kind;
	switch(t.kind) {
	case let:
		return declaration(let_const);
	case const_user:
		return declaration(let_const);
	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}


const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while(true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t=ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch(runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

	try {
		system ("chcp 1251"); 
		stab.define("pi",3.14, const_user);
		calculate();
		return 0;
	}
	catch (exception& e) {
		cerr << "exception: " << e.what() << endl;
		char c;
		while (cin >>c&& c!=';') ;
		return 1;
	}
	catch (...) {
		cerr << "exception\n";
		char c;
		while (cin>>c && c!=';');
		return 2;
	}
