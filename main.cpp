#include "copygraph.h"
#include "bipartite_max_flow.h"
#include <boost/graph/breadth_first_search.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/array.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/edge_array.h>
#include <LEDA/graph/node_array.h>
#include <LEDA/graph/graph_misc.h>
#include <LEDA/core/random_source.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/max_flow.h>
#include <LEDA/graph/templates/max_flow.h>
#include "LEDA/system/timer.h"
#include <ctime>
#include <limits>
#include <array>
#include <vector>
#include <queue>
#include <iterator>
#include <iostream>

using namespace boost;

typedef leda::node node;
typedef leda::edge Edge;

typedef leda::graph LGraph;
using leda::node_array;
using leda::edge_array;



int main()
{
	LGraph LG;
	leda::edge_array<int> res(LG);

	int random;
	leda::array<node> A(500);
	leda::array<node> B(500);

	int layers[] = { 110, 10, 40, 10, 10, 10, 10, 10, 110, 49, 220, 110, 330, 40, 50, 50, 20, 10, 30, 32, 21, 11, 31, 40, 10, 10, 14, 15, 16, 45, 30, 80, 24, 40, 50,
		10, 10, 40, 10, 10, 10, 10, 10, 10, 49, 20, 110, 30, 40, 150, 72, 10, 230, 32, 21, 11, 31, 40, 10, 10, 14, 15, 16, 45, 30, 80, 24, 40, 50, 1 };
	int length = sizeof(layers) / sizeof(layers[0]);;
	std::cout << "LENGTH = " << length <<"\n";
	const int N_con = 3000;
	node source_L = LG.new_node();
	node sink_L;
	node v_2;
	A.set(0, source_L);


	for (int i = 0; i < length; i++)
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < layers[i]; j++)
			{
				node v = LG.new_node();
				if (j == (layers[i] - 1) && i == (length - 1))
				{
					sink_L = v;
				}
				B.set(j, v);
				int k_max = layers[i - 1];
				if (i == 0)
				{
					k_max = 1;
				}
				for (int k = 0; k < k_max; k++)
				{
					LG.new_edge(A.get(k), v); //add if randomize 
				}
			}
		}
		else
		{
			for (int l = 0; l < layers[i]; l++)
			{
				v_2 = LG.new_node();
				if (l == (layers[i] - 1) && i == (length - 1))
				{
					sink_L = v_2;
				}
				A.set(l, v_2);
				for (int k_2 = 0; k_2 < layers[i - 1]; k_2++)
				{
					LG.new_edge(B.get(k_2), v_2); //add if randomize 
				}
			}
		}
	}
	
	res.init(LG);
	leda::random_source S(0, 500);
	Edge e;
	S.set_seed(43);
	forall_edges(e, LG)
	{
		res[e] = S();
	}

	Vertex source_B;
	std::vector< int > res_array;
	BGraph BG;
	copy_graph(LG, BG, res_array, res, source_B);


	//************************** Algorithm visualization****************************\

	//std::string filename = "graph_vis.txt";
	//std::ofstream ofs;
	//ofs.open(filename.c_str());

	//int i = 1;

	//leda::edge e1;
	//leda::node v;

	//leda::node_array<int> AA(LG);

	//forall_nodes(v, LG) AA[v] = i++;

	//ofs << "digraph LG{" << "\n";

	//forall_edges(e1, LG) {
	//	ofs << AA[LG.source(e1)] << " -> " << AA[LG.target(e1)] << ";" << "\n";
	//}

	//ofs << "}" << "\n";

	//ofs.close();


	leda::timer leda_max_flow;

	leda::edge_array<int> flow(LG);
	leda_max_flow.start();
	int flow_value = MAX_FLOW(LG, source_L, sink_L, res, flow);
	leda_max_flow.stop();
	std::cout << "LEDA time = " << leda_max_flow.elapsed_time() << "\n";
	std::cout << "LEDA FLOW = " << flow_value << "\n\n";
	
	//leda::edge e2;
	//forall_edges(e2, LG) {
	//	if (flow[e2] > 0) {
	//		LG.print_edge(e2);
	//		std::cout << " flow = " << flow[e2] << std::endl;
	//	}
	//}

	leda::timer my_max_flow;
	my_max_flow.start();
	int flow_2 = bipartite_max_flow(BG, source_B, res_array);
	my_max_flow.stop();
	std::cout << "BOOST time = " << my_max_flow.elapsed_time() << "\n";
	std::cout << "BOOST FLOW = " << flow_2;
	return EXIT_SUCCESS;
}

