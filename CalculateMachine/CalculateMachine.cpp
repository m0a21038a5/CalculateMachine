#define _USE_MATH_DEFINES
#include <iostream>
#include <string>
#include <stack>
#include <sstream>
#include <vector>
#include <regex>
#include <cmath>
#include <map>



using namespace std;


enum class Tokentype {
	OPERATOR,
	NUMBER,
	OPEN,
	CLOSE,
	FUNCTION,
	END
};

struct Token {
	Tokentype type;
	string value;
};

map<string, double> variables = { {"PI", M_PI},{"e", M_E} };
map<string, string> functionDefinitions;

vector<Token> ToPostfix(const vector<Token>& fixTokens);
vector<Token> tokenize(const string& expression);
double evaluatePostfix(const vector<Token>& postfixTokens);



double evaluateFunction(const string& functionName, string argument) {


	vector<Token> infixTokens = tokenize(argument);
	vector<Token> postfixTokens = ToPostfix(infixTokens);

	double arg = evaluatePostfix(postfixTokens);
	double argumentInRadians{};
	if (argument.find("PI") != string::npos)
	{
		argumentInRadians = arg;
	}
	else
	{
		argumentInRadians = arg * M_PI / 180.0;
	}

	if (functionName == "sin") {
		return sin(argumentInRadians);
	}
	else if (functionName == "cos") {
		return cos(argumentInRadians);
	}
	else if (functionName == "tan") {
		return tan(argumentInRadians);
	}
	else {
		// 未知の関数名に対する処理
		cout << "Unknown function: " << functionName << endl;
		return 0.0;
	}
}

double calculateLog(const string& functionName, string argument) {
	vector<Token> infixTokens = tokenize(argument);
	vector<Token> postfixTokens = ToPostfix(infixTokens);

	double arg = evaluatePostfix(postfixTokens);

	if (arg < 0)
	{
		cout << "Error" << endl;
		return 0.0;
	}
	else
	{
		return log(arg);
	}
}

vector<Token> tokenize(const string& expression) {
	vector<Token> tokens;
	istringstream iss(expression);
	string token;
	smatch m;
	regex functionRegex("-?(sin|cos|tan)\\(([-+*/^0-9a-zA-Z]+)\\)");
	regex LogRegex("-?(log)\\(([-+*/^0-9a-zA-Z]+)\\)");
	regex exponentiationRegex("-?[0-9a-zA-Z]+\\^-?[0-9a-zA-Z]+.*[0-9a-zA-Z]?[+*/^]?[0-9a-zA-Z]?.*[0-9a-zA-Z]?");
	regex equalRegex("\\$([a-zA-Z]+)=-?\\d+\\.?\\d*");
	regex fregex("([a-zA-Z]+)\\(([^)]+)\\)\\s*=\\s*(-?[0-9a-zA-Z+*/^]+)");
	regex stairRegex("\\d+!");

	while (iss >> token)
	{
		if (token == "+")
		{
			tokens.push_back({ Tokentype::OPERATOR, "+" });
		}
		else if (token == "-")
		{
			tokens.push_back({ Tokentype::OPERATOR, "-" });
		}
		else if (token == "*")
		{
			tokens.push_back({ Tokentype::OPERATOR, "*" });
		}
		else if (token == "/")
		{
			tokens.push_back({ Tokentype::OPERATOR, "/" });
		}
		else if (token == "(")
		{
			tokens.push_back({ Tokentype::OPEN, "(" });
		}
		else if (token == ")")
		{
			tokens.push_back({ Tokentype::CLOSE, ")" });
		}
		else if (token == "^") {
			tokens.push_back({ Tokentype::OPERATOR, "^" });
		}
		else if (regex_match(token, m, equalRegex))
		{
			string pre = m[1].str();
			double mat = 0;
			smatch s;
			regex numberregex("-?\\d+\\.?\\d*");
			for (size_t i = 0; i < token.size(); ++i) {
				if (i > 0 && token[i - 1] == '=' && token[i] != '=') {
					token.insert(i, " ");
					++i;
				}
				else if (token[i] == '=' && i + 1 < token.size() && token[i + 1] != '=') {
					token.insert(i, " ");
					++i;
				}
			}
			istringstream is(token);
			string d;
			while (is >> d)
			{
				if (regex_match(d, s, numberregex)) {
					mat = stod(s[0].str());
					if (variables.find(pre) != variables.end())
					{
						variables[pre] = mat;
					}
					else
					{
						variables.insert(make_pair(pre, mat));
					}
					tokens.push_back({ Tokentype::NUMBER, to_string(mat) });
				}
			}

		}
		else if (regex_match(token, m, functionRegex))
		{
			string tr = m[1].str();
			string g = token.erase(0, 4);
			g.erase(g.end() - 1);
			for (size_t i = 0; i < g.size(); ++i) {
				if (i > 0 && (g[i - 1] == '+' || g[i - 1] == '-' || g[i - 1] == '*' || g[i - 1] == '/' || g[i - 1] == '(' || g[i - 1] == ')') &&
					(g[i] != '+' || g[i] != '-' || g[i] != '*' || g[i] != '/' || g[i] != '(' || g[i] != '-')) {
					g.insert(i, " ");
					++i;
				}
				else if ((g[i] == '+' || g[i] == '-' || g[i] == '*' || g[i] == '/' || g[i] == '(') &&
					i + 1 < g.size() &&
					(g[i + 1] != '+' || g[i + 1] != '-' || g[i + 1] != '*' || g[i + 1] != '/' || g[i + 1] != '(')) {
					g.insert(i, " ");
					++i;
				}
			}
			tokens.push_back({ Tokentype::NUMBER, to_string(evaluateFunction(tr, g)) });
		}
		else if (variables.find(token) != variables.end()) {
			token = to_string(variables[token]);
			tokens.push_back({ Tokentype::NUMBER, token });
		}
		else if (regex_match(token, m, LogRegex)) {
			string tr = m[1].str();
			string g = token.erase(0, 4);
			g.erase(g.end() - 1);
			for (size_t i = 0; i < g.size(); ++i) {
				if (i > 0 && (g[i - 1] == '+' || g[i - 1] == '-' || g[i - 1] == '*' || g[i - 1] == '/' || g[i - 1] == '(' || g[i - 1] == ')' || g[i - 1] == '^') &&
					(g[i] != '+' || g[i] != '-' || g[i] != '*' || g[i] != '/' || g[i] != '(' || g[i] != '-' || g[i] != '^')) {
					g.insert(i, " ");
					++i;
				}
				else if ((g[i] == '+' || g[i] == '-' || g[i] == '*' || g[i] == '/' || g[i] == '(' || g[i] == '^') &&
					i + 1 < g.size() &&
					(g[i + 1] != '+' || g[i + 1] != '-' || g[i + 1] != '*' || g[i + 1] != '/' || g[i + 1] != '(' || g[i + 1] != '^')) {
					g.insert(i, " ");
					++i;
				}
			}
			tokens.push_back({ Tokentype::NUMBER, to_string(calculateLog(tr, g)) });
		}
		else if (regex_match(token, m, exponentiationRegex)) {
			string g = token;
			for (size_t i = 0; i < g.size(); ++i) {
				if (i > 0 && g[i - 1] == '^' && g[i] != '^') {
					g.insert(i, " ");
					++i;
				}
				else if (g[i] == '^' && i + 1 < g.size() && g[i + 1] != '^') {
					g.insert(i, " ");
					++i;
				}
			}
			vector<Token> exponentiationTokens = tokenize(g);
			vector<Token> postfixTokens = ToPostfix(exponentiationTokens);

			double arg = evaluatePostfix(postfixTokens);
			tokens.push_back({ Tokentype::NUMBER, to_string(arg) });
		}
		else if (regex_match(token, m, fregex))
		{
			regex funcnameRegex("([a-zA-Z]+)\\(([^)]+)\\)");
			regex funcRegex("(-?[0-9a-zA-Z+*/^]+)");
			string g = token;
			string funcname;
			string func;

			for (size_t i = 0; i < g.size(); ++i) {
				if (i > 0 && g[i - 1] == '=' && g[i] != '=') {
					g.insert(i, " ");
					++i;
				}
				else if (g[i] == '='  && i + 1 < g.size() && g[i + 1] != '=') {
					g.insert(i, " ");
					++i;
				}
			}
	
			istringstream isss(g);
			string d;
			smatch c;
			smatch l;

			while (isss >> d)
			{
				if (regex_match(d, c, funcnameRegex))
				{
					funcname = c[0].str();
				}
				else if (regex_match(d, l, funcRegex))
				{
					func = l[0].str();
					cout << func << endl;
				}
			}
			functionDefinitions[funcname] = func;
			cout << funcname << func << endl;
			tokens.push_back({ Tokentype::FUNCTION,g });
		}
		else if (regex_match(token, m, stairRegex))
		{
			string g = token;
			for (size_t i = 0; i <= g.size(); ++i) {
				if (i > 0 && g[i - 1] == '!' && g[i] != '!') {
					g.insert(i - 1, " ");
					i += 1;
				}
				else if (g[i] == '!' && i + 1 < g.size() && g[i + 1] != '!'){
					g.insert(i + 1, " ");
					i += 1;
				}
			}
			regex numR("\\d+");
			istringstream issss(g);
			string j;
			smatch p;
			int a = 0;
			while (issss >> j)
			{
				if (regex_match(j, p, numR))
				{
					a = stoi(p[0].str());
				}
			}
			double sum = 1;

			for (int i = 1; i <= a; i++)
			{
				sum *= i;
			}
			tokens.push_back({ Tokentype::NUMBER, to_string(sum) });
		}
		else
		{
			tokens.push_back({ Tokentype::NUMBER, token });
		}
	}

	tokens.push_back({ Tokentype::END, "" });
	return tokens;
}

int priority(const string& op) {
	if (op == "+" || op == "-")
	{
		return 1;
	}
	else if (op == "*" || op == "/" || op == "^")
	{
		return 2;
	}
	return 0;
}

vector<Token> ToPostfix(const vector<Token>& fixTokens)
{
	vector<Token> postfixTokens;
	stack<Token> opStack;

	for (const auto& token : fixTokens)
	{
		switch (token.type)
		{
		case Tokentype::NUMBER:
			postfixTokens.push_back(token);
			break;
		case Tokentype::OPERATOR:
			while (!opStack.empty() && priority(opStack.top().value) >= priority(token.value))
			{
				postfixTokens.push_back(opStack.top());
				opStack.pop();
			}
			opStack.push(token);
			break;
		case Tokentype::OPEN:
			opStack.push(token);
			break;
		case Tokentype::CLOSE:
			while (!opStack.empty() && opStack.top().type != Tokentype::OPEN)
			{
				postfixTokens.push_back(opStack.top());
				opStack.pop();
			}
			opStack.pop();
			break;
		case Tokentype::END:
			while (!opStack.empty())
			{
				postfixTokens.push_back(opStack.top());
				opStack.pop();
			}
			break;
		}
	}

	return postfixTokens;
}

double evaluatePostfix(const vector<Token>& postfixTokens) {
	stack<double> valueStack;
	for (const auto& token : postfixTokens) {
		switch (token.type) {
		case Tokentype::NUMBER:
			valueStack.push(stod(token.value));
			break;
		case Tokentype::OPERATOR:
		{
			if (!valueStack.empty()) {
				double operand2 = valueStack.top();
				valueStack.pop();

				if (!valueStack.empty())
				{
					double operand1 = valueStack.top();
					valueStack.pop();

					if (token.value == "+")
					{
						valueStack.push(operand1 + operand2);
					}
					else if (token.value == "-")
					{
						valueStack.push(operand1 - operand2);
					}
					else if (token.value == "*")
					{
						valueStack.push(operand1 * operand2);
					}
					else if (token.value == "/")
					{
						if (operand2 != 0.0)
						{
							valueStack.push(operand1 / operand2);
						}
						else
						{
							cout << "Error" << endl;
							return 0.0;
						}
					}
					else if (token.value == "^")
					{
						valueStack.push(pow(operand1, operand2));
					}
				}
			}
			else {
				cout << "Error" << endl;
				return 0.0;
			}
		}
		break;
		}
	}

	if (!valueStack.empty()) {
		return valueStack.top();
	}
	else {
		cout << "Error" << endl;
		return 0.0;
	}
}

int main()
{
	string expression;
	while (true)
	{
		cout << "<";

		getline(cin, expression);

		if (expression == "end") return 0;

		vector<Token> infixTokens = tokenize(expression);

		vector<Token> postfixTokens = ToPostfix(infixTokens);

		double result = evaluatePostfix(postfixTokens);

		cout << "Ans:" << result << endl;
	}



	return 0;
}