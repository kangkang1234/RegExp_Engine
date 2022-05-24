#include "Reg_Exp.h"
using namespace std;

vector<set<int>> RegExp::chSet;

bool RegExp::RegExp_match(const string& s)
{
	int index = autoMa->start;
	for (auto it = s.begin(); it != s.end(); it++) {
		int chi = *it;
		if (!isMatch(index, chi)) {
			return false;
		}
	}
	if (!isFinal(index)) {
		return false;
	}
	return true;
}

bool RegExp::RegExp_match(const char* ch)
{
	return false;
}

bool RegExp::isMatch(int& index, int chi)
{
	Edge* p = autoMa->mini_dfa_G.NodeTable[index].adj;
	while (p != nullptr) {
		if (p->dest.find(chi) != p->dest.end()) {
			index = p->nextVertex;
			return true;
		}
		p = p->link;
	}
	return false;
}

bool RegExp::isFinal(int index)
{
	if (autoMa->end.find(index) != autoMa->end.end()) {
		return true;
	}
	return false;
}

void RegExp::generate_prefix(string s)//缺少一个正则文法格式检测的部分，理论上要生成语法树进行比对，之后补上。中缀表达式的生成函数默认传入的字符串是正确的格式
{
	int in = 0;//先生成中缀表达式
	int size = s.size();
	while (in < size) {
		int temp;
		if (temp = isConstructor(s[in])) { //处理构造符
			infix.push_back(temp);
			in++;
		}
		else if (s[in] == '.') {
			infix.push_back(128);
			in++;
		}
		else if (s[in] == '\\') { //处理转义字符
			if (++in < size) {
				if (temp = isMetacharacter(s[in])) {
					infix.push_back(temp);
				}
				else {
					infix.push_back(s[in] + 0);

				}
				in++;
			}
			else {
				break;
			}
		}
		else if (s[in] == '{') { //两种情况，前一个字符为),将括号内的内容都复制；若前一个字符为ascii字符，将该字符复制
			int front = in - 1;//前一个字符序号
			int frontch;//前一个字符
			int chsNum = 0;//处理[a-z]的情况，需要往chS中压入字符集
			int duplicateNum = 0;//辅助处理[a-z]的情况

			vector<int> content; //存放重复内容
			in++;
			string temp;//存放{}中的内容
			while (s[in] != '}') {
				temp.push_back(s[in]);
				in++;
			}
			in++;
			if (front < 0) {
				continue;
			}
			frontch = infix.back();
			if (isNormal(frontch)||isMetacharacter(frontch)) {
				content.push_back(frontch);
				if (frontch == 133) {
					chsNum = 1;
				}
			}
			else if (frontch == 148) {
				auto it = infix.end();
				it--;
				while (*it != 147) {
					if (*it == 133) {
						chsNum++;
					}
					it--;
				}
				content.assign(it, infix.end());
			}
			if (temp.find(',')==string::npos) {//没有,的情况
				int n = stoi(temp);
				vector<int>tempV = content;
				for (int i = 1; i < n; i++) {
					content.insert(content.end(),tempV.begin(), tempV.end());
				}
				duplicateNum = chsNum * (n - 1);
				infix.insert(infix.end(), content.begin(), content.end());
			}
			else {
				int pos = temp.find(',');//有逗号的情况比如e{3,5},会被转化为eeee?e?
				int m = stoi(temp.substr(0, pos));
				int n = stoi(temp.substr(pos + 1, temp.size() - pos - 1));
				if (m > n) {
					int te = m;
					m = n;
					n = te;
				}
				vector<int>tempV = content;
				content = {};
				for (int i = 1; i < m; i++) {
					content.insert(content.end(), tempV.begin(), tempV.end());
				}
				for (int i = 0; i < n-m; i++) {
					content.insert(content.end(), tempV.begin(), tempV.end());
					content.push_back(143);
				}
				duplicateNum = chsNum * (n - 1);
				infix.insert(infix.end(), content.begin(), content.end());
			}
			if (chsNum) { //同步chSet集合，这样每个字符类133都有对应的字符集
				vector<set<int>>tempV;
				for (int i = chSet.size() - chsNum; i < chSet.size(); i++) {
					tempV.push_back(chSet[i]);
				}
				for (int i = 0; i < duplicateNum; i++) {
					chSet.insert(chSet.end(), tempV.begin(), tempV.end());
				}
			}
		}
		else if (s[in] == '[') { //处理字符类
			in++;
			set<int>temp;
			string t;
			int begin = in;
			int i = 0;
			while (s[in] != ']') {
				i++;
				in++;
			}
			t = s.substr(begin, i); 
			for (int i = 0; i < t.size(); i++) {
				if (t[i] == '\\'&&i+1<t.size()) { //处理需要插入'-'的情况
					temp.insert(t[i+1]);
					i++;
				}
				else if (t[i] == '-'&&i>0&&i+1<t.size()) {//这一部分处理[a-z]之类的情况
					for (int j = t[i - 1] + 1; j < t[i + 1]; j++) {
						temp.insert(j);
					}
				}
				else {
					temp.insert(t[i]);
				}
			}
			in++;
			infix.push_back(133);
			chSet.push_back(temp);
		}
		else if (s[in] == '(') {
			infix.push_back(147);
			in++;
		}
		else if (s[in] == ')') {
			infix.push_back(148);
			in++;
		}
		else {
			infix.push_back(s[in++] + 0); //处理普通字符
		}
	}
	//插入连接符
	vector<int>temp;
	temp.push_back(147);
	for (int i = 0; i < infix.size(); i++) {
		int chi = infix[i];
		if (i > 0 && (isMetacharacter(chi) || isNormal(chi) || chi==147)) {
			if (infix[i - 1] != 144 && infix[i - 1] != 147) {
				temp.push_back(146);
			}
		}
		temp.push_back(infix[i]);
	}
	temp.push_back(148);
	infix = temp;

	//开始生成后缀表达式
	stack<int>opS;
	int i = 1;
	opS.push(infix[0]);
	while (!opS.empty()) {
		int cur = infix[i];
		if (isConstructor(cur)) {
			int top = opS.top();
			int cindex = cur - 141;
			int tindex = top - 141;
			while (!priorityTable[tindex][cindex]) {//当当前字符比栈顶字符优先级小的时候，栈顶出栈
				if (top == 147 && cur == 148) {//括号单独处理
					opS.pop();
					break;
				}
				prefix.push_back(top);
				opS.pop();
				top = opS.top();
				tindex = top - 141;
			}
			if (cur != 148) {//如果当前字符为)则不进栈，否则将当前字符压入符号栈
				opS.push(cur);
			}
			i++;
		}
		else {
			prefix.push_back(cur);
			i++;
		}
	}

}


int RegExp::isConstructor(char ch)//判断是否是构造符，并返回构造符序号
{
	int result = 0;
	switch (ch)
	{
	case '*':result = 141; break;
	case '+':result = 142; break;
	case '?':result = 143; break;
	case '|':result = 144; break;
	case '·':result = 146; break;
	case '(':result = 147; break;
	case ')':result = 148; break;
	default:
		break;
	}
	return result;
}

bool RegExp::isConstructor(int chi)
{
	if (chi >= 141 && chi <= 148) {
		return true;
	}
	return false;
}

bool RegExp::isMetacharacter(int chi)
{
	if (chi <= 140 && chi >= 128) {
		return true;
	}
	return false;
}

bool RegExp::isNormal(int chi)
{
	if (chi <= 127 && chi >= 0) {
		return true;
	}
	return false;
}

int RegExp::isMetacharacter(char ch)//判断是否是元字符
{
	int result = 0;
	switch (ch)
	{
	//case '.':result = 128; break; \.表示. 而.才是元字符
	case 'w':result = 129; break;
	case 's':result = 130; break;
	case 'd':result = 131; break;
	case 'b':result = 132; break;
	default: break;
	}
	return result;
}

bool RegExp::isNumber(char ch)
{
	if (ch <= '9' && ch >= '0') {
		return true;
	}
	return false;
}

void nfa::generateNfa()
{
	for (int i = 0; i < nfa_prefix.size(); i++) {
		if (RegExp::isNormal(nfa_prefix[i]) || RegExp::isMetacharacter(nfa_prefix[i])) {
			chS.push(nfa_G.numVertices);
			insertVertex();
			chS.push(nfa_G.numVertices);
			insertVertex();
			insertEdge(nfa_G.numVertices - 2, nfa_G.numVertices - 1, nfa_prefix[i]);//a->b
		}
		else {
			switch (nfa_prefix[i]) {
			case 141: {
				int b = chS.top();
				chS.pop();
				int a = chS.top();
				chS.pop();
				insertVertex();
				insertVertex();
				insertEdge(nfa_G.numVertices - 2, a, 160);
				insertEdge(nfa_G.numVertices - 2, nfa_G.numVertices - 1, 160);
				insertEdge(b, a, 160);
				insertEdge(b, nfa_G.numVertices - 1, 160);
				chS.push(nfa_G.numVertices - 2);
				chS.push(nfa_G.numVertices - 1);
				break;
			}
			case 142: {
				int b = chS.top();
				chS.pop();
				int a = chS.top();
				insertEdge(b, a, 160);
				chS.push(b);
				break;
			}
			case 143: {
				int b = chS.top();
				chS.pop();
				int a = chS.top();
				insertEdge(a, b, 160);
				chS.push(b);
				break;
			}
			case 144: {
				int d = chS.top();
				chS.pop();
				int c = chS.top();
				chS.pop();
				int b = chS.top();
				chS.pop();
				int a = chS.top();
				chS.pop();
				insertVertex();
				insertVertex();
				insertEdge(nfa_G.numVertices - 2, a, 160);
				insertEdge(nfa_G.numVertices - 2, c, 160);
				insertEdge(b, nfa_G.numVertices - 1, 160);
				insertEdge(d, nfa_G.numVertices - 1, 160);
				chS.push(nfa_G.numVertices - 2);
				chS.push(nfa_G.numVertices - 1);
				break;
			}
			case 145:break; //重复已处理
			case 146: {
				int d = chS.top();
				chS.pop();
				int c = chS.top();
				chS.pop();
				int b = chS.top();
				chS.pop();
				int a = chS.top();
				chS.pop();
				insertEdge(b, c, 160);
				chS.push(a);
				chS.push(d);
				break;
			}
			default:break;
			}
		}
	}
	end = chS.top();
	chS.pop();
	start = chS.top();
	nfa_G.NodeTable[end].fianl = 1;
}

void nfa::insertEdge(int a, int b, int chi)
{


	if (RegExp::isNormal(chi)) {
		nfa_G.insertEdge(a, b, chi);
	}
	else {
		switch (chi)
		{
		case 128: {
			set<int>temp;
			for (int i = 0; i < 127; i++) {
				if (i != 10) {
					temp.insert(i);
				}
			}
			nfa_G.insertEdge(a, b, temp);
			break;
		}
		case 129:{
			set<int>temp;
			for (int i = 48; i < 58; i++) {
				temp.insert(i);
			}
			for (int i = 65; i < 91; i++) {
				temp.insert(i);
			}
			for (int i = 97; i < 123; i++) {
				temp.insert(i);
			}
			temp.insert(95);
			nfa_G.insertEdge(a, b, temp);
			break;
		}
		case 130: {
			set<int>temp = { 9,10,11,32 };//空格 制表 换行
			nfa_G.insertEdge(a, b, temp);
			break;
		}
		case 131: {
			set<int>temp;
			for (int i = 48; i < 58; i++) {
				temp.insert(i);
			}
			nfa_G.insertEdge(a, b, temp);
			break;
		}//暂不支持132，因为\b这个是占位符,之后再处理
		case 133: {
			nfa_G.insertEdge(a,b,RegExp::chSet[0]);
			RegExp::chSet.erase(RegExp::chSet.begin());
			break;
		}
		case 160: {
			nfa_G.insertEdge(a, b, 160);//插入ε
			break;
		}
		default:
			break;
		}
	}
}

void nfa::insertVertex()
{
	nfa_G.insertVertex();
}

void dfa::generateDfa()//通过nfa生成dfa
{
	set<int>t;
	epsilon_closure(start,t);
	sets.insert(make_pair(t,dfa_G.numVertices));
	q.push(t);
	dfa_G.insertVertex();//起始状态就是0号状态
	dfa_G.NodeTable[0].start = 1;
	if (t.find(end) != t.end()) {
		dfa_G.NodeTable[dfa_G.numVertices - 1].fianl = 1;//起始状态也有可能是终止状态
	}
	while (!q.empty()) {
		t = q.front();//当前集合
		q.pop();
		map<int, set<int>>tranSet;//当前集合的状态通过任意一条边能转移到的状态集合的集合
		for (auto it = t.begin(); it != t.end(); it++) {//遍历当前集合所有的元素，对应的是nfa中的某个状态
			Edge* p = nfa_G.NodeTable[*it].adj;
			while (p != NULL) {//遍历每一个状态通过某条边能够转移的状态集合
				for (auto ch = p->dest.begin(); ch != p->dest.end(); ch++) {
					if (*ch != 160) {
						set<int>temp;
						epsilon_closure(p->nextVertex, temp);
						if (tranSet.find(*ch) != tranSet.end()) {
							tranSet[*ch].insert(temp.begin(), temp.end());
						}
						else {
							tranSet.insert(make_pair(*ch, temp));
						}
					}
				}
				p = p->link;
			}
		}
		for (auto it = tranSet.begin(); it != tranSet.end(); it++) {
			set<int>temp = it->second;
			if (sets.find(temp) == sets.end()) {//如果是新的集合
				sets.insert(make_pair(temp, dfa_G.numVertices));
				q.push(temp);
				dfa_G.insertVertex();
				dfa_G.insertEdge(sets[t], dfa_G.numVertices - 1, it->first);
				if (temp.find(end) != temp.end()) {
					dfa_G.NodeTable[dfa_G.numVertices - 1].fianl = 1;//代表这是一个终止状态
				}
			}
			else {//如果是老集合
				dfa_G.insertEdge(sets[t], sets[temp], it->first);
			}
		}
	}
}

void dfa::epsilon_closure(int index, set<int>& s)
{
	s.insert(index);//先要插入自身
	Edge* p = nfa_G.NodeTable[index].adj;
	while (p != NULL) {
		if (p->dest.find(160) != p->dest.end()) {
			s.insert(p->nextVertex);
			epsilon_closure(p->nextVertex, s);
		}
		p = p->link;
	}
}

void mini_dfa::initial()
{
	stateToSetTable.resize(dfa_G.numVertices, 0);
	stateSetTransferTable = vector<int>(dfa_G.numVertices, -1);
	for (int i = 0; i < dfa_G.NodeTable.size(); i++) {
		if (dfa_G.NodeTable[i].fianl) {
			final.insert(i);
		}
		else {
			begin.insert(i);
		}
	}
}

void mini_dfa::generate_mini_dfa()
{
	int formalNum = 0;//执行一次划分前集合大小
	insertASet(begin);
	insertASet(final);
	updateTable();
	int nowNum = sets.size();//执行一次划分后集合大小，当划分前后集合大小一致说明算法结束，集合划分完毕
	while (nowNum != formalNum) {//hopcroft算法
		formalNum = nowNum;
		updateTable();
		split();
		nowNum = sets.size();
	}
	set_to_dfa_o();
}

void mini_dfa::set_to_dfa_o()//把划分好的集合转化为mini_dfa,并设置mini_dfa的起点和终点集合
{
	int index = 0;
	for (auto it = sets.begin(); it != sets.end(); it++) {
		set<int>temp = it->second;
		for (auto it = temp.begin(); it != temp.end(); it++) {
			stateToSetTable[*it] = index;
		}
		index++;
	}
	for (int i = 0; i < sets.size(); i++) {
		mini_dfa_G.insertVertex();
	}
	for (auto it = sets.begin(); it != sets.end(); it++) {
		set<int>temp = it->second;
		for (auto it = temp.begin(); it != temp.end(); it++) {
			Edge* p = dfa_G.NodeTable[*it].adj;
			while (p != nullptr) {
				mini_dfa_G.insertEdge(stateToSetTable[*it], stateToSetTable[p->nextVertex], p->dest);
				p = p->link;
			}
			if (dfa_G.NodeTable[*it].start) {
				mini_dfa_G.NodeTable[stateToSetTable[*it]].start = 1;
				start = stateToSetTable[*it];
			}
			else if (dfa_G.NodeTable[*it].fianl) {
				mini_dfa_G.NodeTable[stateToSetTable[*it]].fianl = 1;
				end.insert(stateToSetTable[*it]);
			}
		}
	}
}

void mini_dfa::split()
{
	for (int i = 0; i < 128; i++) {//foreach(character c)
		for (auto it = sets.begin(); it != sets.end(); it++) {
			set<int>temp = it->second;
			if (canSplit(i, temp)) {
				doSplit(i, it->first);
				return;
			}
		}
	}
}

void mini_dfa::updateTable()//更新状态集合对应表,这个表存在的目的是便于集合划分
{
	for (auto it = sets.begin(); it != sets.end(); it++) {
		set<int>temp = it->second;
		for (auto i = temp.begin(); i != temp.end(); i++) {
			stateToSetTable[*i] = it->first;
		}
	}
}

void mini_dfa::doSplit(int c, int index)
{
	set<int>temp = sets[index];
	map<int, set<int>>splitedSet;
	for (auto it = temp.begin(); it != temp.end(); it++) {//*it每个状态
		auto i = splitedSet.find(stateSetTransferTable[*it]);//stateSetTransferTable[*it]表示每个状态通过c转移的集合
		if (i == splitedSet.end()) {//如果状态转移到一个新的集合
			set<int>s;
			s.insert(*it);
			splitedSet.insert(make_pair(stateSetTransferTable[*it], s));
		}
		else {
			i->second.insert(*it);
		}
	}
//	if (splitedSet.begin()->first == -1) {错误做法，实际上，如果一个状态a通过ch能转移到另一个状态k，而另一个状态b通过ch不能转移到任何状态，则ch可以划分a和b，即a和b不能存在于同一个集合之中
//		auto it = splitedSet.begin();
//		it++;
//		set<int>noTransfer = splitedSet[-1];
//		it->second.insert(noTransfer.begin(), noTransfer.end());//将无法通过c转换到任意状态的状态集合和其他任意一个集合合并
//		splitedSet.erase(-1);
//	}
	sets.erase(index);
	for (auto it = splitedSet.begin(); it != splitedSet.end(); it++) {
		insertASet(it->second);//将分割的集合插入进去
	}
}

bool mini_dfa::insertASet(set<int>& s)
{
	if (!s.empty()) {
		sets.insert(make_pair(setsIndex, s));
		setsIndex++;
		return true;
	}
	return false;
}

bool mini_dfa::canSplit(int c, set<int> s)//若某个状态通过c无法转移到任何状态，也应该被视为转移到不同状态
{
	stateSetTransferTable = vector<int>(dfa_G.numVertices, -1);//初始值为-1，表示无法通过c转移到任何状态
	if (s.size() <= 1) {
		return false;
	}
	for (auto it = s.begin(); it != s.end(); it++) {
		int ind = *it;
		Edge* p = dfa_G.NodeTable[ind].adj;
		while (p != nullptr) {
			if (p->dest.find(c) != p->dest.end()) {
				stateSetTransferTable[*it] = stateToSetTable[p->nextVertex];//sstt大小初值不应该位128
			}
			p = p->link;
		}
	}
	int fIndex = stateSetTransferTable[*s.begin()];
	for (auto it = s.begin()++; it != s.end(); it++) {
		if (stateSetTransferTable[*it] != fIndex) {
			return true;
		}
	}
	return false;
}
