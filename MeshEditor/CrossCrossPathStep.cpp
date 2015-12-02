#include "stdafx.h"
#include "CrossCrossPathStep.h"

CCPStep::CCPStep (VolumeMesh *_mesh, CCPSHConf &_prev_conf, 
	OvmVeH _curr_vh, OvmEgH _next_eh)
	: mesh (_mesh), prev_conf (_prev_conf), curr_vh (_curr_vh), next_eh (_next_eh)
{
	JC::get_adj_hexas_around_vertex (mesh, curr_vh, hexas_around_vh);
	JC::get_adj_hexas_around_edge (mesh, next_eh, hexas_around_next_eh);
	JC::get_adj_edges_around_vertex (mesh, curr_vh, edges_around_vh);
	get_all_candidate ();
}

bool CCPStep::has_next_conf ()
{
	return !step_node_queue.empty ();
}

CCPSHConf CCPStep::get_next_conf ()
{
	saved_conf = step_node_queue.front ().hexa_conf;
	step_node_queue.erase (step_node_queue.begin ());
	CCPSHConf conf;
	conf.resize (4);
	for (int i = 0; i != 4; ++i){
		foreach (auto &ch, saved_conf[i]){
			if (JC::contains (hexas_around_next_eh, ch))
				conf[i].insert (ch);
		}
	}
	return conf;
}

CCPSHConf CCPStep::get_saved_conf ()
{
	return saved_conf;
}

void CCPStep::get_all_candidate ()
{
	//首先测试下输入的prev_conf是否就已经符合要求了，这个通常出现在cross路径第一轮循环，即刚开始的时候
	if (eligible_candidate (prev_conf)){
		CCPStepNode step_node;
		step_node.hexa_conf = prev_conf;
		step_node_queue.push_back (step_node);
		return;
	}

	std::set<CCPSHConf> tested_confs;
	std::queue<CCPSHConf> spread_set;
	tested_confs.insert (prev_conf);
	spread_set.push (prev_conf);

	while (!spread_set.empty ()){
		CCPSHConf front_conf = spread_set.front ();
		spread_set.pop ();

		auto hexas_in_front_conf = convert_conf_to_hexa_set (front_conf);
		std::unordered_set<OvmHaFaH> boundary_hfhs;
		JC::collect_boundary_element (mesh, hexas_in_front_conf, NULL, NULL, &boundary_hfhs);
		
		foreach (auto &hfh, boundary_hfhs){
			auto oppo_hfh = mesh->opposite_halfface_handle (hfh);
			auto outer_ch = mesh->incident_cell (oppo_hfh);
			if (outer_ch == mesh->InvalidCellHandle)
				continue;
			if (!JC::contains (hexas_around_vh, outer_ch))
				continue;
			auto inner_ch = mesh->incident_cell (hfh);
			auto new_conf = front_conf;
			for (int i = 0; i != 4; ++i){
				if (JC::contains (new_conf[i], inner_ch)){
					new_conf[i].insert (outer_ch);
					break;
				}
			}

			//测试一下是否是有效的配置格局
			if (valid_hexa_config (mesh, new_conf)){
				//看是否已经检查过了
				if (JC::contains (tested_confs, new_conf)) continue;
				else tested_confs.insert (new_conf);

				//如果该配置已经符合candidate的条件了，则放入step_node_queue
				//否则放入spread_set，继续扩展搜寻
				if (eligible_candidate (new_conf)){
					CCPStepNode step_node;
					step_node.hexa_conf = new_conf;
					step_node.penalty = calc_penalty (new_conf);
					step_node_queue.push_back (step_node);
				}else{
					spread_set.push (new_conf);
				}
			}//end if (valid_config (new_conf)){...
		}//end foreach (auto &hfh, bound...
	}//end while (!spread_set.emp...

	//然后对搜集到的所有有效的配置进行排序
	//首先计算各个有效配置的penalty
	for (int i = 0; i != step_node_queue.size (); ++i)
		step_node_queue[i].penalty = calc_penalty (step_node_queue[i].hexa_conf);
	//然后按照penalty进行从小到大排序
	auto fConfComp = [&] (const CCPStepNode &n1, const CCPStepNode &n2)->bool{
		return n1.penalty < n2.penalty;
	};
	std::sort (step_node_queue.begin (), step_node_queue.end (), fConfComp);
}

bool valid_hexa_config (VolumeMesh *mesh, CCPSHConf &hexa_conf)
{
	//要是一个有效的配置需要满足两个要求：
	//1：各个（一共4个）六面体集合本身内部需要是连通的；
	//2：对角线上的两个六面体集合之间只能以边相邻
	foreach (auto &hs, hexa_conf){
		if (!JC::is_hexa_set_connected (mesh, hs))
			return false;
	}

	auto fUnconnected = [&] (CCPSHSet hs1, const CCPSHSet &hs2)->bool{
		foreach (OvmCeH ch, hs2)
			hs1.insert (ch);
		return !JC::is_hexa_set_connected (mesh, hs1);
	};

	if (!fUnconnected (hexa_conf[0], hexa_conf[2]) ||
		!fUnconnected (hexa_conf[1], hexa_conf[3]))
		return false;

	return true;
}

bool CCPStep::eligible_candidate (CCPSHConf &hexa_conf)
{
	//要是一个有效的可选方案需要在满足有效的配置基础上再满足两个条件：
	//1：六面体集合总和必须等于围绕顶点v的六面体集合（即不能剩下）；
	//2：必须都在next_eh相邻的六面体中占得一席之地（即都步进到了next_eh）
	CCPSHSet all_hexas = convert_conf_to_hexa_set (hexa_conf);
	if (!JC::equals (all_hexas,hexas_around_vh))
		return false;

	foreach (auto &hs, hexa_conf){
		bool reach_next_eh = false;
		foreach (OvmCeH ch, hs){
			//判断该ch是否到达next_eh，同时为避免重复计算，加上对reach_next_eh的判断
			if (!reach_next_eh && JC::contains (hexas_around_next_eh, ch))
				reach_next_eh = true;
		}
		if (!reach_next_eh)
			return false;
	}	

	return true;
}

double CCPStep::calc_penalty (CCPSHConf &hexa_conf)
{
	double penalty = 0.0f, edge_valence = 0.0f;

	std::hash_map<OvmFaH, int> face_count_mapping;
	for (int i = 0; i != 4; ++i){
		auto hexas = hexa_conf[i];
		std::unordered_set<OvmFaH> bnd_fhs;
		JC::collect_boundary_element (mesh, hexas, NULL, NULL, &bnd_fhs);
		foreach (auto &bnd_fh, bnd_fhs){
			face_count_mapping[bnd_fh]++;
		}
	}

	std::hash_map<OvmEgH, std::set<OvmFaH> > edge_count_mapping;

	foreach (auto &p, face_count_mapping){
		if (p.second != 2) continue;
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_face (mesh, p.first, adj_ehs);
		foreach (auto &eh, adj_ehs)
			edge_count_mapping[eh].insert (p.first);
	}

	foreach (auto &p, edge_count_mapping){
		if (p.second.size () != 2) continue;

		std::vector<OvmFaH> adj_fhs;
		JC::set_to_vector (p.second, adj_fhs);
		edge_valence += JC::edge_valence_change (mesh, p.first, adj_fhs.front (), adj_fhs.back ());
	}

	//double edge_valence_weight = 5.0f;

	//std::unordered_set<OvmHaFaH> boundary_hfhs;
	//auto hexas = convert_conf_to_hexa_set (hexa_conf);
	//JC::collect_boundary_element (mesh, hexas, NULL, NULL, &boundary_hfhs);

	//std::hash_map<OvmEgH, std::set<OvmFaH> > edge_count_mapping;
	//foreach (auto &hfh, boundary_hfhs){
	//	auto heh_vec = mesh->halfface (hfh).halfedges ();
	//	foreach (auto &heh, heh_vec){
	//		auto eh = mesh->edge_handle (heh);
	//		if (!JC::contains (edges_around_vh, eh))
	//			continue;
	//		edge_count_mapping[eh].insert (mesh->face_handle (hfh));
	//	}
	//}

	//std::set<OvmFaH> tmp_fhs;
	//foreach (auto &p, edge_count_mapping){
	//	if (p.second.size () != 2)
	//		continue;
	//	std::vector<OvmFaH> fhs_vec;
	//	JC::set_to_vector (p.second, fhs_vec);
	//	edge_valence += JC::edge_valence_change (mesh, p.first, fhs_vec.front (), fhs_vec.back ());
	//	tmp_fhs.insert (fhs_vec.front ());	tmp_fhs.insert (fhs_vec.back ());
	//}

	//penalty = edge_valence * edge_valence_weight + tmp_fhs.size ();
	penalty = edge_valence;

	return penalty;
}

CCPSHSet CCPStep::convert_conf_to_hexa_set (CCPSHConf &hexa_conf)
{
	CCPSHSet all_hexas;
	foreach (auto &hs, hexa_conf){
		foreach (auto &ch, hs)
			all_hexas.insert (ch);
	}
	return all_hexas;
}