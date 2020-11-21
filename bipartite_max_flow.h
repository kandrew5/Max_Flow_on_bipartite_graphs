#include <boost/graph/breadth_first_search.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/array.hpp>
#include <limits>
#include <array>
#include <vector>
#include <queue>
#include <iterator>
#include <iostream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>


using namespace boost;


// Select the graph type we wish to use
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperties, EdgeProperties> BGraph;
typedef boost::graph_traits<BGraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<BGraph>::edge_descriptor arc;
typedef boost::graph_traits<BGraph>::edge_iterator arcIt;
typedef boost::graph_traits<BGraph>::vertex_iterator vertexIt;
typedef boost::graph_traits<BGraph>::out_edge_iterator outArcIt;
typedef boost::graph_traits<BGraph>::in_edge_iterator inArcIt;

struct admissible_Arc {
	arc re_arc;
	bool found;
};

 // function that iterates over the graph and finds all the active nodes 
 // (all the nodes that have positive excess except the sink)
bool search_active_nodes(BGraph& g, int(N), std::queue<int>& active_queue, int exc[])
{
	for (int i = 1; i < N; i++)
	{
		if (exc[i] > 0 && i != N - 1) //find active nodes
		{
			active_queue.push(i);
		}
	}
	if (active_queue.empty())
	{
		return false;
	}
	return true;
}

//function returns an admissible arc (or an null pointer if there is no admissible arc) from node i.
// for node i, admissible arc (i,j) is an arc for which distance(i) = distance(j) + 1.
// Checks for inverse arcs too
admissible_Arc check_admissible_arcs(BGraph& g, Vertex i, std::vector<int>& distances)
{
	property_map<BGraph, int EdgeProperties::*>::type residual_a = get(&EdgeProperties::res_cap, g);
	property_map<BGraph, int EdgeProperties::*>::type max_residual_a = get(&EdgeProperties::max_res_cap, g);

	outArcIt oei, oei_end;
	for (boost::tie(oei, oei_end) = out_edges(i, g); oei != oei_end; ++oei) {
		Vertex w_2 = target(*oei, g);
		if (residual_a[*oei] > 0 && (distances[i] == distances[w_2] + 1))  //arc in residual graph (not sink node)
		{
			return { *oei, true };
		}
	}
	inArcIt iei, iei_end;
	for (boost::tie(iei, iei_end) = in_edges(i, g); iei != iei_end; ++iei) {
		Vertex w_3 = source(*iei, g);
		if (residual_a[*iei] < max_residual_a[*iei] && (distances[i] == distances[w_3] + 1))  //arc in residual graph
		{
			return { *iei, true };
		}
	}
	return { *oei , false };
}

//function that applies relabel to vertex j and thus replacing its distance
//with the minimum distance of its neighbours + 1 
void relabel(BGraph& g, Vertex v, std::vector<int>& distances)
{
	property_map<BGraph, int EdgeProperties::*>::type
		residual_a = get(&EdgeProperties::res_cap, g);
	property_map<BGraph, int EdgeProperties::*>::type
		max_residual_a = get(&EdgeProperties::max_res_cap, g);
	int new_label = INT_MAX;
	outArcIt oei, oei_end;
	for (boost::tie(oei, oei_end) = out_edges(v, g); oei != oei_end; ++oei) {
		Vertex w_2 = target(*oei, g);
		if (residual_a[*oei] > 0 && distances[w_2] != 0)  //arc in residual graph (not sink node)
		{
			if ((distances[w_2] + 1) < new_label)
			{
				new_label = distances[w_2] + 1;
			}
		}
	}
	inArcIt iei, iei_end;
	for (boost::tie(iei, iei_end) = in_edges(v, g); iei != iei_end; ++iei) {
		Vertex w_3 = source(*iei, g);
		if (residual_a[*iei] < max_residual_a[*iei])  //arc in residual graph
		{
			if ((distances[w_3] + 1) < new_label)
			{
				new_label = distances[w_3] + 1;
			}
		}
	}
	distances[v] = new_label;
}

//function that pushes flow from node i to node k through to consecutive edges
bool double_push(BGraph& g, int flow_to_push, arc ij, arc jk, bool reverse_1, bool reverse_2)
{
	property_map<BGraph, int EdgeProperties::*>::type
		residual_a = get(&EdgeProperties::res_cap, g);
	if (reverse_1)
	{
		residual_a[ij] += flow_to_push;
	}
	else
	{
		residual_a[ij] -= flow_to_push;
	}
	if (reverse_2)
	{
		residual_a[jk] += flow_to_push;
	}
	else
	{
		residual_a[jk] -= flow_to_push;
	}
	return true;
}

// function that pushes flow either to the source or the sink
bool single_push(BGraph& g, int flow_to_push, arc ij, bool reverse_1)
{
	property_map<BGraph, int EdgeProperties::*>::type residual_a = get(&EdgeProperties::res_cap, g);

	if (reverse_1)
	{
		residual_a[ij] += flow_to_push;
	}
	else
	{
		residual_a[ij] -= flow_to_push;
	}
	return true;
}


int bipartite_max_flow(BGraph& g, const int& s, std::vector<int>& res_array)
{
	//********************* QUICK, ONLY BOOST TEST *********************

	//enum { s, r, v, u, t, k, N };
	//const char* name = "rstuvwxy";
	//typedef std::pair <int, int >E;
	//const int n_edges = 7;
	//E edge_array[] = { E(s, r), E(s, v), E(s, u), E(r, t), E(v, t), E(u, t), E(t, k)};
	//int res[n_edges] = { 3,3,4,4,5,5,7 };
	//int exc[N] = {};
	//ÂGraph g(edge_array, edge_array + n_edges, N);

	//********************* QUICK, ONLY BOOST TEST *********************


	const int N = 3000;
	long flow;
	int exc[N] = {};
	boost::property_map<BGraph, int EdgeProperties::*>::type
		residual_a = get(&EdgeProperties::res_cap, g);
	boost::property_map<BGraph, int EdgeProperties::*>::type
		max_residual_a = get(&EdgeProperties::max_res_cap, g);

	// ********************* PREPROCESS *********************

	std::vector<int> distances(N);
	boost::array<int, N> distance_boost{ {0} };

	boost::breadth_first_search(g, vertex(s, g),
		boost::visitor(
			boost::make_bfs_visitor(
				boost::record_distances(distance_boost.begin(),
					boost::on_tree_edge{}))));

	int n1 = 0;
	for (int i = 1; i < N; i++)
	{
		if (distance_boost[i] % 2 == 1)
		{
			n1 += 1;
		}
		distances[i] = distance_boost[N - 1] - distance_boost[i];
	}
	distances[0] = 2 * n1 + 1;


	graph_traits<BGraph>::edge_iterator ei, ei_end;
	int indexing = 0;
	for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei, ++indexing)
	{
		residual_a[*ei] = res_array[indexing];
		max_residual_a[*ei] = res_array[indexing];
	}

	typedef property_map<BGraph, vertex_index_t>::type IndexMap;

	IndexMap index = get(vertex_index, g);
	Vertex src = s;
	outArcIt oei, oei_end;
	for (boost::tie(oei, oei_end) = out_edges(src, g); oei != oei_end; ++oei) {
		Vertex v = source(*oei, g);
		Vertex w = target(*oei, g);
		exc[w] = residual_a[*oei];
		residual_a[*oei] -= residual_a[*oei];
	}

	// ********************* bipartite push/relabel *********************

	std::queue<int> active_queue;
	int res_jk = 0;
	int res_ij = 0;
	bool reverse_1 = false;
	bool reverse_2 = false;

	while (search_active_nodes(g, N, active_queue, exc))
	{
		while (!active_queue.empty())
		{
			Vertex i = active_queue.front();
			active_queue.pop();
			bool success = false;
			bool success_2 = false;
			reverse_1 = false;
			reverse_2 = false;
			admissible_Arc admissible_ij = check_admissible_arcs(g, i, distances);
			arc ij = admissible_ij.re_arc;
			Vertex j = target(ij, g);
			if (admissible_ij.found) {
				res_ij = residual_a[ij];
				if (j == i) // inverse edge
				{
					reverse_1 = true;
					j = source(ij, g);
					res_ij = max_residual_a[ij] - residual_a[ij];
				}
				if (j == 0 || j == N - 1)
				{
					int flow_to_single_push = std::min({ res_ij, exc[i] });
					success_2 = single_push(g, flow_to_single_push, ij, reverse_1);
					exc[i] -= flow_to_single_push;
					exc[j] += flow_to_single_push;
					//std::cout << "SUPER_PUSH" << flow_to_single_push << "\n";
					break;
				}
				admissible_Arc ad_jk = check_admissible_arcs(g, j, distances);
				arc jk = ad_jk.re_arc;
				Vertex k = target(jk, g);
				if (ad_jk.found)
				{
					res_jk = residual_a[jk];
					if (k == j) // inverse edge
					{
						reverse_2 = true;
						k = source(jk, g);
						res_jk = max_residual_a[jk] - residual_a[jk];
					}
					int flow_to_push = std::min({ res_ij, res_jk, exc[i] });
					success = double_push(g, flow_to_push, ij, jk, reverse_1, reverse_2);
					reverse_1 = false;
					reverse_2 = false;
					if (success)
					{
						success = false;
						exc[i] -= flow_to_push;
						if (exc[k] == 0)
						{
							active_queue.push(k);
						}
						exc[k] += flow_to_push;
					}
					//std::cout << i << "-" << j << "-" << k << " PUSH" << flow_to_push << "\n";
				}
				else
				{
					//std::cout << "RELABEL-i\n";
					if (j == 0) //source can't be relabeled
					{
						relabel(g, i, distances);
					}
					else
					{
						relabel(g, j, distances);
					}
				}
			}
			else
			{
				//std::cout << "RELABEL-j\n";
				relabel(g, i, distances);
			}
		}

		//for (tie(ei, ei_end) = edges(g); ei != ei_end; ei++) {
		//	std::cout << "(" << index[source(*ei, g)] << " " << index[target(*ei, g)] << ") with res " << residual_a[*ei] << "\n";
		//}
	/*	std::cout << "excess\n";
		for (int i = 0; i < N; i++)
		{
			std::cout <<i <<"--" << exc[i] << "\n";
		}*/

	/*	std::cout << "distances\n";
		for (int i = 0; i < N; i++)
		{
			std::cout <<i <<"--"<< distances[i] << "\n";
		}*/
	}
	flow = exc[N - 1];
	//std::cout << "SUCCESS!!!!! PUSHED:" << flow;
	return flow;
}

