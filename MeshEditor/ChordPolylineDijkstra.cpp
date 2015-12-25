#include "StdAfx.h"
#include "ChordPolylineDijkstra.h"


ChordPolylineDijkstra::ChordPolylineDijkstra(VolumeMesh *_mesh, OvmVeH _start_vh, std::unordered_set<OvmVeH> *_end_vhs,
	const std::vector<std::set<OvmEgH> > *_candi_interval_ehs,
	std::unordered_set<OvmEgH> *_rest_ehs,
	ChordSet _matched_chords)
	: MeshDijkstra (_mesh, _start_vh, _end_vhs), candi_interval_ehs(_candi_interval_ehs), 
	rest_ehs (_rest_ehs), matched_chords(_matched_chords)
{
	consider_topology_only = true;
	search_boundary = true;
	search_inner = false;
	searchable_ehs = *rest_ehs;
	min_valence = 0;
}


ChordPolylineDijkstra::~ChordPolylineDijkstra(void)
{
}

void ChordPolylineDijkstra::get_adj_nodes (DijkstraVVPathNode *node, std::vector<DijkstraVVPathNode*> &adj_nodes)
{
	adj_nodes.clear ();
	auto temp_adj_nodes = adj_nodes;
	MeshDijkstra::get_adj_nodes (node, temp_adj_nodes);

	//下面对初步得到的temp_adj_nodes中的节点进行判断，剔除那些不符合相交序列的节点
	foreach (auto temp_adj_node, temp_adj_nodes){
		//temp_adj_node->parent_vh = node->vh;
		if (check_int_graph_partially (temp_adj_node)){
			adj_nodes.push_back (temp_adj_node);
		}else{
			delete temp_adj_node;
		}
	}
}

bool ChordPolylineDijkstra::reach_the_end (DijkstraVVPathNode *node)
{
	return (MeshDijkstra::reach_the_end (node) && check_int_graph_completely (node));
}

bool ChordPolylineDijkstra::check_int_graph_partially (DijkstraVVPathNode *node)
{
	std::vector<OvmEgH> traced_ehs;
	std::vector<OvmVeH> traced_vhs;
	traced_vhs.push_back (node->vh);
	while (node && node->parent_vh != mesh->InvalidVertexHandle){
		auto eh = mesh->edge_handle (mesh->halfedge (node->vh, node->parent_vh));
		traced_ehs.push_back (eh);
		traced_vhs.push_back (node->parent_vh);
		node = S_set.retrieve (node->parent_vh);
	}
	//将traced_ehs反向一下
	std::reverse (traced_ehs.begin (), traced_ehs.end ());
	std::reverse (traced_vhs.begin (), traced_vhs.end ());

	auto E_CHORD_PTR = mesh->request_edge_property<unsigned int>("chordptr");
	std::vector<OvmEgH> traced_matched_ehs;
	std::set<OvmVeH> all_vhs;
	all_vhs.insert (traced_vhs.front ());
	for (int i = 0; i != traced_ehs.size (); ++i){
		auto vh = traced_vhs[i + 1];
		auto chord = (DualChord*)E_CHORD_PTR[traced_ehs[i]];
		if (JC::contains (matched_chords, chord))
			traced_matched_ehs.push_back (traced_ehs[i]);

		if (JC::contains (all_vhs, vh))
			traced_matched_ehs.push_back (mesh->InvalidEdgeHandle);
		all_vhs.insert (vh);
	}
	//然后依次检查traced_ehs上的边是否按照一致的先后顺序在candi_interval_ehs中
	if (traced_matched_ehs.size () > candi_interval_ehs->size ())
		return false;
	for (int i = 0; i != traced_matched_ehs.size (); ++i){
		auto eh = traced_matched_ehs[i];
		if (eh == mesh->InvalidEdgeHandle){
			if (!(*candi_interval_ehs)[i].empty ())
				return false;
		}else{
			if (!JC::contains ((*candi_interval_ehs)[i], eh))
				return false;
		}

	}
	return true;
}

bool ChordPolylineDijkstra::check_int_graph_completely (DijkstraVVPathNode *node)
{
	std::vector<OvmEgH> traced_ehs;
	std::vector<OvmVeH> traced_vhs;
	traced_vhs.push_back (node->vh);
	while (node && node->parent_vh != mesh->InvalidVertexHandle){
		auto eh = mesh->edge_handle (mesh->halfedge (node->vh, node->parent_vh));
		traced_ehs.push_back (eh);
		traced_vhs.push_back (node->parent_vh);
		node = S_set.retrieve (node->parent_vh);
	}
	//将traced_ehs反向一下
	std::reverse (traced_ehs.begin (), traced_ehs.end ());
	std::reverse (traced_vhs.begin (), traced_vhs.end ());

	auto E_CHORD_PTR = mesh->request_edge_property<unsigned int>("chordptr");
	std::vector<OvmEgH> traced_matched_ehs;
	std::set<OvmVeH> all_vhs;
	all_vhs.insert (traced_vhs.front ());
	for (int i = 0; i != traced_ehs.size (); ++i){
		auto vh = traced_vhs[i + 1];
		auto chord = (DualChord*)E_CHORD_PTR[traced_ehs[i]];
		if (JC::contains (matched_chords, chord))
			traced_matched_ehs.push_back (traced_ehs[i]);

		if (JC::contains (all_vhs, vh))
			traced_matched_ehs.push_back (mesh->InvalidEdgeHandle);
		all_vhs.insert (vh);
	}

	//然后依次检查traced_ehs上的边是否按照一致的先后顺序在candi_interval_ehs中
	if (traced_matched_ehs.size () != candi_interval_ehs->size ())
		return false;
	for (int i = 0; i != traced_matched_ehs.size (); ++i){
		auto eh = traced_matched_ehs[i];
		if (eh == mesh->InvalidEdgeHandle){
			if (!(*candi_interval_ehs)[i].empty ())
				return false;
		}else{
			if (!JC::contains ((*candi_interval_ehs)[i], eh))
				return false;
		}

	}
	return true;
}

bool ChordPolylineDijkstra::need_update (DijkstraVVPathNode *old_node, DijkstraVVPathNode *test_node)
{
	return MeshDijkstra::need_update (old_node, test_node);
}
