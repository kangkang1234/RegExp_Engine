#ifndef REGEXP_H
#define REGEXP_H
#include<string>
#include<vector>
#include<set>
#include<stack>
#include<queue>
#include<map>
#include "graph.h"
using namespace std;

class nfa
{
private:
	vector<int> nfa_prefix;
	stack<int>chS;//构造nfa时，需要知道参与构造算法的节点位置，栈中存放这个位置()
public:
	Graph nfa_G;
	int start, end;
	nfa(vector<int> prefix) :nfa_prefix(prefix) {
		generateNfa();
	}
	void generateNfa();
	void insertEdge(int a, int b, int chi);
	void insertVertex();
};
class dfa
{
public:
	Graph &nfa_G;
	Graph dfa_G;
	int start, end;
	int dfa_start = 0;
	map<set<int>, int>sets;//存放所有集合
	queue<set<int>>q;//广度优先遍历
	dfa(nfa& nfa_o):nfa_G(nfa_o.nfa_G),start(nfa_o.start),end(nfa_o.end) {
		generateDfa();
	}
	void generateDfa();
	void epsilon_closure(int index,set<int>& s);
};
class mini_dfa
{
public:
	Graph& dfa_G;
	Graph mini_dfa_G;
	set<int>begin;//执行子集划分时的两个状态之一
	set<int>final;//执行子集划分时两个状态之一
	int start;//标志状态入口
	set<int> end;//标志终结状态
	map<int, set<int>>sets;
	int setsIndex = 0;//递增，对集合进行标识
	vector<int>stateToSetTable;//用来查找某个状态属于哪一个集合
	vector<int>stateSetTransferTable;//用来查找某个状态通过c可以转换到的状态属于哪个集合，-1代表这个状态通过c无法转移到任何状态

	mini_dfa(dfa& dfa_o):dfa_G(dfa_o.dfa_G) {
		initial();
		generate_mini_dfa();
	}
	void initial();
	void generate_mini_dfa();
	void set_to_dfa_o();
	void split();
	void updateTable();
	void doSplit(int c, int index);
	bool insertASet(set<int>& s);
	bool canSplit(int c, set<int>s);
};

class RegExp

{
private:
	vector<int>infix;
	//vector<int>infix_duplicate;
	vector<int> prefix;
	vector<vector<int>> priorityTable;//存放优先级表，记得释放
	mini_dfa* autoMa;//最终结果是得到一个最小化dfa作为自动机，并利用match方法进行匹配(堆上分配，析构时记得释放)
	bool isMatch(int& index, int chi);
	bool isFinal(int index);
	
public:
	static vector<set<int>> chSet;//存放字符集eg:[aeiou]
	RegExp(string s) {
		priorityTable = *(new vector<vector<int>>{ {0,0,0,0,0,0,1,0},{0,0,0,0,0,0,1,0},{0,0,0,0,0,0,1,0},{1,1,1,0,1,1,1,0},{0,0,0,0,0,0,1,0},{1,1,1,0,1,0,1,0},{1,1,1,1,1,1,1,0},{-1,-1,-1,-1,-1,-1,-1,-1} });
		generate_prefix(s);
		nfa nfa_o(prefix);
		dfa dfa_o(nfa_o);
		mini_dfa* mini_dfa_o = new mini_dfa(dfa_o);//理解一下对象指针
		this->autoMa = mini_dfa_o;
	}
	void generate_prefix(string s);
	bool RegExp_match(const string& s);
	bool RegExp_match(const char* ch);
	static int isConstructor(char ch);
	static bool isConstructor(int chi);
	static bool isMetacharacter(int chi);//返回是否为元字符
	static int isMetacharacter(char ch);//返回元字符编号
	static bool isNumber(char ch);
	static bool isNormal(int chi);
	
};


#endif

