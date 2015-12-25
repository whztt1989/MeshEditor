#pragma once
#include "meshdijkstra.h"
class ChordPolylineDijkstra :
	public MeshDijkstra
{
public:
	ChordPolylineDijkstra(VolumeMesh *_mesh, OvmVeH _start_vh, std::unordered_set<OvmVeH> *_end_vhs,
		const std::vector<std::set<OvmEgH> > *_candi_interval_ehs,
		std::unordered_set<OvmEgH> *_rest_ehs,
		ChordSet _matched_chords);
	~ChordPolylineDijkstra(void);
private:
	void get_adj_nodes (DijkstraVVPathNode *node, std::vector<DijkstraVVPathNode*> &adj_nodes);
	bool need_update (DijkstraVVPathNode *old_node, DijkstraVVPathNode *test_node);
	bool reach_the_end (DijkstraVVPathNode *node);
	
	bool check_int_graph_partially (DijkstraVVPathNode *node);
	bool check_int_graph_completely (DijkstraVVPathNode *node);
	//���eh��candi_interval_ehs�д�ͷ��ʼ�ڼ������������candi_interval_ehs�У��򷵻�-1
	inline int get_int_index (OvmEgH eh){
		int idx = -1;
		for (int i = 0; i != candi_interval_ehs->size (); ++i){
			if (JC::contains ((*candi_interval_ehs)[i], eh)){
				idx = i;
				break;
			}
		}
		return idx;
	}
private:
	const std::vector<std::set<OvmEgH> > *candi_interval_ehs;
	//ehs_on_interfaces�г���candi_interval_ehs�Լ����ϱ��ϵ������ʣ��������
	std::unordered_set<OvmEgH> *rest_ehs;
	ChordSet matched_chords;
};

