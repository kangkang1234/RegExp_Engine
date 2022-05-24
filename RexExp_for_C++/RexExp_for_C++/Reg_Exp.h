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
	stack<int>chS;//����nfaʱ����Ҫ֪�����빹���㷨�Ľڵ�λ�ã�ջ�д�����λ��()
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
	map<set<int>, int>sets;//������м���
	queue<set<int>>q;//������ȱ���
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
	set<int>begin;//ִ���Ӽ�����ʱ������״̬֮һ
	set<int>final;//ִ���Ӽ�����ʱ����״̬֮һ
	int start;//��־״̬���
	set<int> end;//��־�ս�״̬
	map<int, set<int>>sets;
	int setsIndex = 0;//�������Լ��Ͻ��б�ʶ
	vector<int>stateToSetTable;//��������ĳ��״̬������һ������
	vector<int>stateSetTransferTable;//��������ĳ��״̬ͨ��c����ת������״̬�����ĸ����ϣ�-1�������״̬ͨ��c�޷�ת�Ƶ��κ�״̬

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
	vector<vector<int>> priorityTable;//������ȼ����ǵ��ͷ�
	mini_dfa* autoMa;//���ս���ǵõ�һ����С��dfa��Ϊ�Զ�����������match��������ƥ��(���Ϸ��䣬����ʱ�ǵ��ͷ�)
	bool isMatch(int& index, int chi);
	bool isFinal(int index);
	
public:
	static vector<set<int>> chSet;//����ַ���eg:[aeiou]
	RegExp(string s) {
		priorityTable = *(new vector<vector<int>>{ {0,0,0,0,0,0,1,0},{0,0,0,0,0,0,1,0},{0,0,0,0,0,0,1,0},{1,1,1,0,1,1,1,0},{0,0,0,0,0,0,1,0},{1,1,1,0,1,0,1,0},{1,1,1,1,1,1,1,0},{-1,-1,-1,-1,-1,-1,-1,-1} });
		generate_prefix(s);
		nfa nfa_o(prefix);
		dfa dfa_o(nfa_o);
		mini_dfa* mini_dfa_o = new mini_dfa(dfa_o);//���һ�¶���ָ��
		this->autoMa = mini_dfa_o;
	}
	void generate_prefix(string s);
	bool RegExp_match(const string& s);
	bool RegExp_match(const char* ch);
	static int isConstructor(char ch);
	static bool isConstructor(int chi);
	static bool isMetacharacter(int chi);//�����Ƿ�ΪԪ�ַ�
	static int isMetacharacter(char ch);//����Ԫ�ַ����
	static bool isNumber(char ch);
	static bool isNormal(int chi);
	
};


#endif

