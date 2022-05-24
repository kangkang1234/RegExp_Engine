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

void RegExp::generate_prefix(string s)//ȱ��һ�������ķ���ʽ���Ĳ��֣�������Ҫ�����﷨�����бȶԣ�֮���ϡ���׺���ʽ�����ɺ���Ĭ�ϴ�����ַ�������ȷ�ĸ�ʽ
{
	int in = 0;//��������׺���ʽ
	int size = s.size();
	while (in < size) {
		int temp;
		if (temp = isConstructor(s[in])) { //�������
			infix.push_back(temp);
			in++;
		}
		else if (s[in] == '.') {
			infix.push_back(128);
			in++;
		}
		else if (s[in] == '\\') { //����ת���ַ�
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
		else if (s[in] == '{') { //���������ǰһ���ַ�Ϊ),�������ڵ����ݶ����ƣ���ǰһ���ַ�Ϊascii�ַ��������ַ�����
			int front = in - 1;//ǰһ���ַ����
			int frontch;//ǰһ���ַ�
			int chsNum = 0;//����[a-z]���������Ҫ��chS��ѹ���ַ���
			int duplicateNum = 0;//��������[a-z]�����

			vector<int> content; //����ظ�����
			in++;
			string temp;//���{}�е�����
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
			if (temp.find(',')==string::npos) {//û��,�����
				int n = stoi(temp);
				vector<int>tempV = content;
				for (int i = 1; i < n; i++) {
					content.insert(content.end(),tempV.begin(), tempV.end());
				}
				duplicateNum = chsNum * (n - 1);
				infix.insert(infix.end(), content.begin(), content.end());
			}
			else {
				int pos = temp.find(',');//�ж��ŵ��������e{3,5},�ᱻת��Ϊeeee?e?
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
			if (chsNum) { //ͬ��chSet���ϣ�����ÿ���ַ���133���ж�Ӧ���ַ���
				vector<set<int>>tempV;
				for (int i = chSet.size() - chsNum; i < chSet.size(); i++) {
					tempV.push_back(chSet[i]);
				}
				for (int i = 0; i < duplicateNum; i++) {
					chSet.insert(chSet.end(), tempV.begin(), tempV.end());
				}
			}
		}
		else if (s[in] == '[') { //�����ַ���
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
				if (t[i] == '\\'&&i+1<t.size()) { //������Ҫ����'-'�����
					temp.insert(t[i+1]);
					i++;
				}
				else if (t[i] == '-'&&i>0&&i+1<t.size()) {//��һ���ִ���[a-z]֮������
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
			infix.push_back(s[in++] + 0); //������ͨ�ַ�
		}
	}
	//�������ӷ�
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

	//��ʼ���ɺ�׺���ʽ
	stack<int>opS;
	int i = 1;
	opS.push(infix[0]);
	while (!opS.empty()) {
		int cur = infix[i];
		if (isConstructor(cur)) {
			int top = opS.top();
			int cindex = cur - 141;
			int tindex = top - 141;
			while (!priorityTable[tindex][cindex]) {//����ǰ�ַ���ջ���ַ����ȼ�С��ʱ��ջ����ջ
				if (top == 147 && cur == 148) {//���ŵ�������
					opS.pop();
					break;
				}
				prefix.push_back(top);
				opS.pop();
				top = opS.top();
				tindex = top - 141;
			}
			if (cur != 148) {//�����ǰ�ַ�Ϊ)�򲻽�ջ�����򽫵�ǰ�ַ�ѹ�����ջ
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


int RegExp::isConstructor(char ch)//�ж��Ƿ��ǹ�����������ع�������
{
	int result = 0;
	switch (ch)
	{
	case '*':result = 141; break;
	case '+':result = 142; break;
	case '?':result = 143; break;
	case '|':result = 144; break;
	case '��':result = 146; break;
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

int RegExp::isMetacharacter(char ch)//�ж��Ƿ���Ԫ�ַ�
{
	int result = 0;
	switch (ch)
	{
	//case '.':result = 128; break; \.��ʾ. ��.����Ԫ�ַ�
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
			case 145:break; //�ظ��Ѵ���
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
			set<int>temp = { 9,10,11,32 };//�ո� �Ʊ� ����
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
		}//�ݲ�֧��132����Ϊ\b�����ռλ��,֮���ٴ���
		case 133: {
			nfa_G.insertEdge(a,b,RegExp::chSet[0]);
			RegExp::chSet.erase(RegExp::chSet.begin());
			break;
		}
		case 160: {
			nfa_G.insertEdge(a, b, 160);//�����
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

void dfa::generateDfa()//ͨ��nfa����dfa
{
	set<int>t;
	epsilon_closure(start,t);
	sets.insert(make_pair(t,dfa_G.numVertices));
	q.push(t);
	dfa_G.insertVertex();//��ʼ״̬����0��״̬
	dfa_G.NodeTable[0].start = 1;
	if (t.find(end) != t.end()) {
		dfa_G.NodeTable[dfa_G.numVertices - 1].fianl = 1;//��ʼ״̬Ҳ�п�������ֹ״̬
	}
	while (!q.empty()) {
		t = q.front();//��ǰ����
		q.pop();
		map<int, set<int>>tranSet;//��ǰ���ϵ�״̬ͨ������һ������ת�Ƶ���״̬���ϵļ���
		for (auto it = t.begin(); it != t.end(); it++) {//������ǰ�������е�Ԫ�أ���Ӧ����nfa�е�ĳ��״̬
			Edge* p = nfa_G.NodeTable[*it].adj;
			while (p != NULL) {//����ÿһ��״̬ͨ��ĳ�����ܹ�ת�Ƶ�״̬����
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
			if (sets.find(temp) == sets.end()) {//������µļ���
				sets.insert(make_pair(temp, dfa_G.numVertices));
				q.push(temp);
				dfa_G.insertVertex();
				dfa_G.insertEdge(sets[t], dfa_G.numVertices - 1, it->first);
				if (temp.find(end) != temp.end()) {
					dfa_G.NodeTable[dfa_G.numVertices - 1].fianl = 1;//��������һ����ֹ״̬
				}
			}
			else {//������ϼ���
				dfa_G.insertEdge(sets[t], sets[temp], it->first);
			}
		}
	}
}

void dfa::epsilon_closure(int index, set<int>& s)
{
	s.insert(index);//��Ҫ��������
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
	int formalNum = 0;//ִ��һ�λ���ǰ���ϴ�С
	insertASet(begin);
	insertASet(final);
	updateTable();
	int nowNum = sets.size();//ִ��һ�λ��ֺ󼯺ϴ�С��������ǰ�󼯺ϴ�Сһ��˵���㷨���������ϻ������
	while (nowNum != formalNum) {//hopcroft�㷨
		formalNum = nowNum;
		updateTable();
		split();
		nowNum = sets.size();
	}
	set_to_dfa_o();
}

void mini_dfa::set_to_dfa_o()//�ѻ��ֺõļ���ת��Ϊmini_dfa,������mini_dfa�������յ㼯��
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

void mini_dfa::updateTable()//����״̬���϶�Ӧ��,�������ڵ�Ŀ���Ǳ��ڼ��ϻ���
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
	for (auto it = temp.begin(); it != temp.end(); it++) {//*itÿ��״̬
		auto i = splitedSet.find(stateSetTransferTable[*it]);//stateSetTransferTable[*it]��ʾÿ��״̬ͨ��cת�Ƶļ���
		if (i == splitedSet.end()) {//���״̬ת�Ƶ�һ���µļ���
			set<int>s;
			s.insert(*it);
			splitedSet.insert(make_pair(stateSetTransferTable[*it], s));
		}
		else {
			i->second.insert(*it);
		}
	}
//	if (splitedSet.begin()->first == -1) {����������ʵ���ϣ����һ��״̬aͨ��ch��ת�Ƶ���һ��״̬k������һ��״̬bͨ��ch����ת�Ƶ��κ�״̬����ch���Ի���a��b����a��b���ܴ�����ͬһ������֮��
//		auto it = splitedSet.begin();
//		it++;
//		set<int>noTransfer = splitedSet[-1];
//		it->second.insert(noTransfer.begin(), noTransfer.end());//���޷�ͨ��cת��������״̬��״̬���Ϻ���������һ�����Ϻϲ�
//		splitedSet.erase(-1);
//	}
	sets.erase(index);
	for (auto it = splitedSet.begin(); it != splitedSet.end(); it++) {
		insertASet(it->second);//���ָ�ļ��ϲ����ȥ
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

bool mini_dfa::canSplit(int c, set<int> s)//��ĳ��״̬ͨ��c�޷�ת�Ƶ��κ�״̬��ҲӦ�ñ���Ϊת�Ƶ���ͬ״̬
{
	stateSetTransferTable = vector<int>(dfa_G.numVertices, -1);//��ʼֵΪ-1����ʾ�޷�ͨ��cת�Ƶ��κ�״̬
	if (s.size() <= 1) {
		return false;
	}
	for (auto it = s.begin(); it != s.end(); it++) {
		int ind = *it;
		Edge* p = dfa_G.NodeTable[ind].adj;
		while (p != nullptr) {
			if (p->dest.find(c) != p->dest.end()) {
				stateSetTransferTable[*it] = stateToSetTable[p->nextVertex];//sstt��С��ֵ��Ӧ��λ128
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
