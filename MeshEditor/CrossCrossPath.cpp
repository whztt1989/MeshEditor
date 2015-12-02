#include "StdAfx.h"
#include <limits>
#include <stack>
#include "CrossCrossPath.h"

CrossCrossPath::CrossCrossPath(VolumeMesh *_mesh,
	std::vector<std::pair<std::unordered_set<OvmFaH>, std::unordered_set<OvmFaH> > > &_fsps,
	OvmVeH _sv, OvmVeH _ev)
	: mesh (_mesh), cross_face_set_pairs (_fsps), start_vh (_sv), end_vh (_ev)
{
}

CrossCrossPath::CrossCrossPath (VolumeMesh *_mesh, std::vector<OvmEgH> &_start_ehs, 
	std::vector<std::unordered_set<OvmFaH> > &_start_cross_face_set, OvmVeH _start_vh, OvmVeH _end_vh)
	: mesh (_mesh), start_ehs (_start_ehs), start_cross_face_set (_start_cross_face_set), start_vh (_start_vh), end_vh (_end_vh)
{}


CrossCrossPath::~CrossCrossPath(void)
{
}

int CrossCrossPath::shortest_path (std::vector<OvmEgH> &path, CCPSHConf &cross_hexas)
{
	cross_hexas.clear ();
	cross_hexas.resize (cross_face_set_pairs.size ());

	//首先利用VertexVertexPath查找从起始交叉点到终止交叉点之间的最优路径
	VertexVertexPath vv_path (mesh, start_vh, end_vh);
	std::vector<OvmHaEgH> cross_vv_path;
	vv_path.shortest_path (cross_vv_path, 4);
	if (cross_vv_path.empty ())
		return -1;
	foreach (OvmHaEgH heh, cross_vv_path)
		path.push_back (mesh->edge_handle (heh));

	CCPSHConf hexa_conf, end_conf;
	for (int i = 0; i != cross_face_set_pairs.size (); ++i){
		std::unordered_set<OvmCeH> curr_start_chs, curr_end_chs;
		JC::get_direct_adjacent_hexas (mesh, cross_face_set_pairs[i].first, curr_start_chs);
		JC::get_direct_adjacent_hexas (mesh, cross_face_set_pairs[i].second, curr_end_chs);
		hexa_conf.push_back (curr_start_chs);
		end_conf.push_back (curr_end_chs);
	}

	OvmHaEgH last_heh = cross_vv_path.back ();
	std::stack<OvmHaEgH> visited_hehs_stack;
	OvmHaEgH curr_heh = cross_vv_path.front ();
	cross_vv_path.erase (cross_vv_path.begin ());
	visited_hehs_stack.push (curr_heh);
	
	std::stack<CCPStep*> ccpsteps_stack;
	CCPStep *new_step = new CCPStep (mesh, hexa_conf, mesh->halfedge (curr_heh).from_vertex (),
		mesh->edge_handle (curr_heh));
	ccpsteps_stack.push (new_step);

	while (true){
		CCPStep *curr_step = ccpsteps_stack.top ();
		OvmHaEgH curr_heh = visited_hehs_stack.top ();

		bool step_forward = false;
		while (curr_step->has_next_conf ()){
			hexa_conf = curr_step->get_next_conf ();
			if (curr_heh == last_heh){
				for (int i = 0; i != 4; ++i){
					foreach (auto &ch, end_conf[i])
						hexa_conf[i].insert (ch);
				}
				//到达最终的end_config
				if (valid_hexa_config (mesh, hexa_conf)){
					//ccpsteps_stack.pop ();
					while (!ccpsteps_stack.empty ()){
						CCPStep *track_back_step = ccpsteps_stack.top ();
						ccpsteps_stack.pop ();
						CCPSHConf saved_conf = track_back_step->get_saved_conf ();
						for (int i = 0; i != 4; ++i){
							foreach (auto &ch, saved_conf[i])
								hexa_conf[i].insert (ch);
						}
						delete track_back_step;
					}

					cross_hexas = hexa_conf;
					return path.size ();
				}
				else continue;
			}else{
				OvmHaEgH next_heh = cross_vv_path.front ();
				new_step = new CCPStep (mesh, hexa_conf, mesh->halfedge (curr_heh).to_vertex (),
					mesh->edge_handle (next_heh));
				ccpsteps_stack.push (new_step);

				cross_vv_path.erase (cross_vv_path.begin ());
				visited_hehs_stack.push (next_heh);
				step_forward = true;
				break;
			}
		}//end while (curr_step->has_next_conf ()){...
		if (!step_forward){
			delete curr_step;
			ccpsteps_stack.pop ();
			visited_hehs_stack.pop ();
			cross_vv_path.insert (cross_vv_path.begin (), curr_heh);
		}
	}
	return 0;
}

bool CrossCrossPath::unconstrained_shortest_path (std::vector<OvmEgH> &path, CCPSHConf &cross_hexas, 
	std::vector<std::pair<OvmEgH, OvmEgH> > &edge_pairs)
{
	cross_hexas.clear ();
	cross_hexas.resize (cross_face_set_pairs.size ());

	std::vector<OvmHaEgH> heh_path;
	JC::get_piecewise_halfedges_from_edges (mesh, path, true, heh_path);
	CCPSHConf hexa_conf;
	for (int i = 0; i != start_cross_face_set.size (); ++i){
		std::unordered_set<OvmCeH> curr_start_chs;
		JC::get_direct_adjacent_hexas (mesh, start_cross_face_set[i], curr_start_chs);
		hexa_conf.push_back (curr_start_chs);
	}

	OvmHaEgH last_heh = heh_path.back ();
	std::stack<OvmHaEgH> visited_hehs_stack;
	OvmHaEgH curr_heh = JC::pop_begin_element (heh_path);
	visited_hehs_stack.push (curr_heh);

	std::stack<CCPStep*> ccpsteps_stack;
	CCPStep *new_step = new CCPStep (mesh, hexa_conf, mesh->halfedge (curr_heh).from_vertex (),
		mesh->edge_handle (curr_heh));
	ccpsteps_stack.push (new_step);

	while (true){
		CCPStep *curr_step = ccpsteps_stack.top ();
		OvmHaEgH curr_heh = visited_hehs_stack.top ();
		OvmEgH curr_eh = mesh->edge_handle (curr_heh);

		bool step_forward = false;
		while (curr_step->has_next_conf ()){
			hexa_conf = curr_step->get_next_conf ();

			//到最后的半边
			if (curr_heh == last_heh){
				//到终点和起始点情况相同，需要对非结构的情况进行考虑，即最后一个hexa_conf可能并不直接和表面相接
				handle_end_vh (hexa_conf, edge_pairs);				

				//delete curr_step;
				//ccpsteps_stack.pop ();
				
				trace_back (hexa_conf, ccpsteps_stack);

				cross_hexas = hexa_conf;
				return path.size ();
			}//end if (curr_heh == last_heh){...

			OvmHaEgH next_heh = JC::pop_begin_element (heh_path);
			OvmEgH next_eh = mesh->edge_handle (next_heh);
			new_step = new CCPStep (mesh, hexa_conf, mesh->halfedge (curr_heh).to_vertex (), next_eh);
			ccpsteps_stack.push (new_step);

			visited_hehs_stack.push (next_heh);

			step_forward = true;
			break;
		}//end while (curr_step->has_next_conf ()){...
		if (!step_forward){
			delete curr_step;
			ccpsteps_stack.pop ();
			visited_hehs_stack.pop ();
			heh_path.insert (heh_path.begin (), curr_heh);
		}
	}
	return !visited_hehs_stack.empty ();
}

void CrossCrossPath::handle_end_vh (CCPSHConf &hexa_conf, std::vector<std::pair<OvmEgH, OvmEgH> > &edge_pairs)
{
	std::unordered_set<OvmCeH> hexas_adj_end_vh;
	CCPSHConf tmp_conf;
	tmp_conf.resize (hexa_conf.size ());
	JC::get_adj_hexas_around_vertex (mesh, end_vh, hexas_adj_end_vh);
	
	foreach (auto hs, hexa_conf){
		foreach (auto ch, hs)
			hexas_adj_end_vh.erase (ch);
	}

	while (!hexas_adj_end_vh.empty ()){
		OvmCeH test_ch = JC::pop_begin_element (hexas_adj_end_vh);
		for (int i = 0; i != hexa_conf.size (); ++i){
			if (JC::is_adjacent (mesh, test_ch, hexa_conf[i])){
				tmp_conf[i].insert (test_ch);
				hexas_adj_end_vh.erase (test_ch);
				break;
			}
		}
	}//end while (!hexas_adj_end_vh.empty ()){...

	for (int i = 0; i != hexa_conf.size (); ++i){
		foreach (OvmCeH ch, tmp_conf[i])
			hexa_conf[i].insert (ch);
	}

	std::vector<OvmEgH> boundary_ehs;
	std::vector<OvmFaH> boundary_fhs;
	JC::get_cw_boundary_edges_faces_around_vertex (mesh, end_vh, boundary_ehs, boundary_fhs);

	for (int i = 0; i != hexa_conf.size (); ++i){
		auto &prev_set = hexa_conf[(i - 1) % tmp_conf.size ()];
		auto &curr_set = hexa_conf[i];
		for (int j = 0; j != boundary_ehs.size (); ++j){
			std::unordered_set<OvmCeH> hexas_adj_eh;
			JC::get_adj_hexas_around_edge (mesh, boundary_ehs[j], hexas_adj_eh);
			if (JC::intersects (hexas_adj_eh, prev_set) && JC::intersects (hexas_adj_eh, curr_set)){
				edge_pairs.push_back (std::make_pair (start_ehs[i], boundary_ehs[j]));
				break;
			}
		}
	}
}

void CrossCrossPath::trace_back (CCPSHConf &hexa_conf, std::stack<CCPStep*> &ccpsteps_stack)
{
	while (!ccpsteps_stack.empty ()){
		CCPStep *track_back_step = ccpsteps_stack.top ();
		ccpsteps_stack.pop ();
		CCPSHConf saved_conf = track_back_step->get_saved_conf ();
		for (int i = 0; i != 4; ++i){
			foreach (auto &ch, saved_conf[i])
				hexa_conf[i].insert (ch);
		}
		delete track_back_step;
	}
}