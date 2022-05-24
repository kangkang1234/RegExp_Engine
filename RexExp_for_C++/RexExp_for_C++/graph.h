#ifndef GRAPH_H
#define GRAPH_H
#include<vector>
#include<set>
using namespace std;

struct Edge
{
	set<int> dest; //��űߵļ���
	int nextVertex = 0; //��һ������
	Edge* link = nullptr; //��һ����
	Edge() {}; //Ĭ�Ϲ��캯��
	Edge(int c, Edge* p, int num) :link(p), nextVertex(num) { dest.insert(c); }//�вι���ӳ�ʼ���б�
	Edge(set<int> c, Edge* p, int num) :link(p), nextVertex(num) { dest=c; }
	void insertDest(int c);
	void insertDest(set<int>c);
};

struct Vertex1 //�ڽӱ�ͷ
{
	int data;//��ͷ���ݣ�����index
	int fianl = 0; //�Ƿ�Ϊ�ս�̬
	Edge* adj = 0; //��ʼָ��Ϊnull
	int start = 0;
};

class Graph
{
private:
	int numEdges;
public:
	vector<Vertex1>NodeTable;
	int numVertices;
	Graph()
	{
		numVertices = 0;
		numEdges = 0;
	}
	bool insertVertex();
	bool insertEdge(int v1,int v2,int c);//����һ��v1��v2�ıߣ�ֵΪc
	bool insertEdge(int v1, int v2, set<int>c);
};


#endif 
