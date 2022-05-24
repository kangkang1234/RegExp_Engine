#ifndef GRAPH_H
#define GRAPH_H
#include<vector>
#include<set>
using namespace std;

struct Edge
{
	set<int> dest; //存放边的集合
	int nextVertex = 0; //下一个顶点
	Edge* link = nullptr; //下一条边
	Edge() {}; //默认构造函数
	Edge(int c, Edge* p, int num) :link(p), nextVertex(num) { dest.insert(c); }//有参构造加初始化列表
	Edge(set<int> c, Edge* p, int num) :link(p), nextVertex(num) { dest=c; }
	void insertDest(int c);
	void insertDest(set<int>c);
};

struct Vertex1 //邻接表头
{
	int data;//表头数据，等于index
	int fianl = 0; //是否为终结态
	Edge* adj = 0; //初始指针为null
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
	bool insertEdge(int v1,int v2,int c);//插入一条v1到v2的边，值为c
	bool insertEdge(int v1, int v2, set<int>c);
};


#endif 
