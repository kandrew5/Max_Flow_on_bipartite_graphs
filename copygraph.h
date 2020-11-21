#include <vector>
#include <iostream>
#include <string>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/random.hpp>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/edge_array.h>
#include <LEDA/graph/node_array.h>
#include <LEDA/graph/graph_misc.h>
#include <LEDA/core/random_source.h>
#include <LEDA/core/array.h>

using namespace std;

typedef leda::node node;
typedef leda::edge Edge;

struct EdgeProperties {
	int res_cap;
	int max_res_cap;
};

struct VertexProperties {
	int distance_label;
	int node_excess;
};
typedef leda::graph LGraph;
using leda::node_array;
using leda::edge_array;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperties, EdgeProperties>BGraph;
typedef boost::graph_traits<BGraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<BGraph>::edge_descriptor arc;
typedef boost::graph_traits<BGraph>::edge_iterator arcIt;
typedef boost::graph_traits<BGraph>::vertex_iterator vertexIt;
typedef boost::graph_traits<BGraph>::out_edge_iterator outArcIt;

void copy_graph(LGraph& LG, BGraph& BG, std::vector< int >& res_array, leda::edge_array<int>& res, Vertex& source_B);

void copy_graph(LGraph& LG, BGraph& BG, std::vector< int >& res_array, leda::edge_array<int>& res, Vertex& source_B)
{
	leda::node_array<Vertex> copy_in_BG(LG);
	arc a;
	Edge e2;
	node v;
	bool first_node = true;

	forall_nodes(v, LG)
	{
		copy_in_BG[v] = add_vertex(BG);
		if (first_node)
		{
			source_B = copy_in_BG[v];
			first_node = false;
		}
	}
	bool isAdded;
	forall_edges(e2, LG)
	{
		tie(a, isAdded) = add_edge(copy_in_BG[source(e2)], copy_in_BG[target(e2)], BG);
		res_array.push_back(res[e2]);
	}

}