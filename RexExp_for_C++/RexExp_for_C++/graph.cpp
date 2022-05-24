#include "graph.h"

bool Graph::insertVertex()
{
	Vertex1 temp;
	temp.data = numVertices;
	NodeTable.push_back(temp);
	numVertices++;
	return true;
}

bool Graph::insertEdge(int v1, int v2, int c)
{
	Edge* p = NodeTable[v1].adj;
	while (p != nullptr) {
		if (p->nextVertex == v2) {
			p->insertDest(c);
			return true;
		}
		p = p->link;
	}
	p = new Edge(c, NodeTable[v1].adj, v2);
	NodeTable[v1].adj = p;
	numEdges++;
	return true;
}

bool Graph::insertEdge(int v1, int v2, set<int>c) {
	Edge* p = NodeTable[v1].adj;
	while (p != nullptr) {
		if (p->nextVertex == v2) {
			p->insertDest(c);
			return true;
		}
		p = p->link;
	}
	p = new Edge(c, NodeTable[v1].adj, v2);
	NodeTable[v1].adj = p;
	numEdges++;
	return true;
}

void Edge::insertDest(int c)
{
	dest.insert(c);
}

void Edge::insertDest(set<int> c)
{
	dest.insert(c.begin(), c.end());
}
