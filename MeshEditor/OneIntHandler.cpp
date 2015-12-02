#include "StdAfx.h"
#include "OneIntHandler.h"
#include "MeshDijkstra.h"
#include <iterator>

OneIntHandler::OneIntHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs, 
	FACE *_inter_face, std::unordered_set<OvmFaH> *_constant_fhs_ptr, 
	std::hash_map<OvmVeH, std::vector<OvmEgH> > *_vh_adj_ehs_mapping, std::vector<OvmVeH> &_int_vhs)
	: OneSimpleHandlerBase (_mesh, _input_ehs)
{
	inter_face = _inter_face;
	constant_fhs_ptr = _constant_fhs_ptr;
	vh_adj_ehs_mapping = _vh_adj_ehs_mapping;
	int_vhs = _int_vhs;
}


OneIntHandler::~OneIntHandler(void)
{
}

std::unordered_set<OvmCeH> OneIntHandler::get_hexa_set ()
{
	all_hexa_set.clear ();
	//���interface�ϵ�������
	get_fhs_on_interface ();
	//����input_ehs��fhs_on_interface���зֿ��
	JC::get_separate_fhs_patches (mesh, interface_fhs, input_ehs, separated_fhs_patches);

	if (separated_fhs_patches.size () != 3){
		last_err_str = QObject::tr ("��loop�޷���������ı��μ��ϣ�");
		return all_hexa_set;
	}

	//��ÿ��Խ����Ż����漯���ⲿ���漯����������ϳ��˲��ɱߵ��Լ��������ϵ��ı�����
	get_optimize_allowed_fhs ();

	//����������ϵ������Խ��ı����漯
	get_diagonal_quad_patches ();

	//��ÿ�����Ϊ�ڶ�������ĵ㼯
	get_intersect_allowed_vhs ();

	//����·�������õ��ӵ�һ�����㵽�ڶ��������·�����Լ��������·�����ĸ������弯��
	search_unconstrained_cross_hexa_sets ();

	get_diagonal_hexa_sets ();

	//�Ա����ı��ν����Ż�
	optimize_diagonal_hexa_set_boundary_quads (diagonal_hexa_set1, diagonal_hexa_set2);

	//�Ի�õ����������弯�Ͻ����Ż�
	optimize_diagonal_hexa_sets (diagonal_hexa_set1, diagonal_hexa_set2);

	//��ÿ�����sheet Inflation���ı����漯
	all_hexa_set = diagonal_hexa_set1;
	foreach (auto &ch, diagonal_hexa_set2)
		all_hexa_set.insert (ch);

	//all_hexa_set = diagonal_hexa_set1;
	//foreach (auto &ch, diagonal_hexa_set2)
	//	all_hexa_set.insert (ch);

	//std::unordered_set<OvmCeH> barrier_chs;
	//for (int i = 0; i != 4; ++i){
	//	if (!JC::contains (diagonal_idx, i)){
	//		foreach (auto &ch, cross_hexas[i])
	//			barrier_chs.insert (ch);
	//	}
	//}

	//JC::get_direct_adjacent_hexas (mesh, interface_fhs, barrier_chs);

	//JC::get_direct_adjacent_hexas (mesh, *constant_fhs_ptr, barrier_chs);

	//JC::optimize_shrink_set_test (mesh, all_hexa_set, barrier_chs);

	return all_hexa_set;
}

std::unordered_set<OvmFaH> OneIntHandler::get_inflation_fhs ()
{
	inflation_fhs.clear ();
	get_inflation_fhs_from_hexa_set (all_hexa_set);
	return inflation_fhs;
}

DualSheet * OneIntHandler::inflate_new_sheet ()
{
	std::unordered_set<OvmEgH> intersect_path_set;
	JC::vector_to_unordered_set (intersect_path, intersect_path_set);
	auto new_sheets = JC::one_simple_sheet_inflation (mesh, inflation_fhs, all_hexa_set, intersect_path_set);
	return new_sheets.front ();
}

void OneIntHandler::get_diagonal_quad_patches ()
{
	//���������У���һ��������鶼ͨ�������ڣ�����Ϊ��������
	//������ͨ��Ψһ��һ�������ڣ���Ϊ��Ҫ����
	std::vector<std::set<OvmVeH> > vhs_on_patches;
	vhs_on_patches.resize (separated_fhs_patches.size ());
	for (int i = 0; i != separated_fhs_patches.size (); ++i){
		auto &patch = separated_fhs_patches[i];
		std::set<OvmVeH> vhs_on_patch;
		foreach (auto &fh, patch){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec)
				vhs_on_patches[i].insert (mesh->halfedge (heh).to_vertex ());
		}
	}

	for (int i = 0; i != separated_fhs_patches.size () - 1; ++i){
		auto &patch1 = separated_fhs_patches[i];
		auto &vhs_on_patch1 = vhs_on_patches[i];

		for (int j = i + 1; j != separated_fhs_patches.size (); ++j){
			auto &patch2 = separated_fhs_patches[j];
			auto &vhs_on_patch2 = vhs_on_patches[j];

			std::vector<OvmVeH> comm_vhs;
			std::set_intersection (vhs_on_patch1.begin (), vhs_on_patch1.end (), 
				vhs_on_patch2.begin (), vhs_on_patch2.end (), std::back_inserter (comm_vhs));

			if (comm_vhs.size () == 1){
				diagonal_quad_patch1 = patch1; diagonal_quad_patch2 = patch2;
				return;
			}
		}//end for (int j = i + 1; j != separated_fhs_patches.size (); ++j){...
	}//end for (int i = 0; i != separated_fhs_patches.size () - 1; ++i){...
}

bool OneIntHandler::search_unconstrained_cross_hexa_sets ()
{
	std::vector<OvmEgH> ehs;
	std::vector<std::unordered_set<OvmFaH> > fhs_sets;
	start_int_vh = *(int_vhs.begin ());
	std::vector<OvmEgH> cross_adj_ehs = (*vh_adj_ehs_mapping)[start_int_vh];
	JC::get_ccw_boundary_edges_faces_around_vertex (mesh, start_int_vh, cross_adj_ehs, ehs, fhs_sets);

	//�������ŵĵڶ����ཻ���λ��
	end_int_vh = mesh->InvalidVertexHandle;
	MeshDijkstra dijkstra (mesh, start_int_vh, &intersect_allowed_vhs);
	dijkstra.shortest_path (intersect_path, end_int_vh);
	if (end_int_vh == mesh->InvalidVertexHandle){
		last_err_str = QObject::tr("�����ڶ�������ʧ�ܣ�");
		return false;
	}

	//�����ڲ������Խ��ߵ�Cross�ֲ�
	CrossCrossPath cc_path (mesh, cross_adj_ehs, fhs_sets, start_int_vh, end_int_vh);
	cross_eh_pairs.clear ();
	//�����ǲ�ȫ�������ڵڶ�������λ��Cross�ֲ���ȷ���������unconstrained����
	cross_hexas.clear ();
	if (!cc_path.unconstrained_shortest_path (intersect_path, cross_hexas, cross_eh_pairs)){
		last_err_str = QObject::tr("��������·��ʧ�ܣ�");
		return false;
	}
	return true;
}

void OneIntHandler::get_diagonal_hexa_sets ()
{
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch1, diagonal_hexa_set1);
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch2, diagonal_hexa_set2);

	for (int i = 0; i != 4; ++i){
		if (JC::intersects (cross_hexas[i], diagonal_hexa_set1)){
			foreach (auto &ch, cross_hexas[i])
				diagonal_hexa_set1.insert (ch);
			diagonal_idx.insert (i);
			continue;
		}
		if (JC::intersects (cross_hexas[i], diagonal_hexa_set2)){
			foreach (auto &ch, cross_hexas[i])
				diagonal_hexa_set2.insert (ch);
			diagonal_idx.insert (i);
		}
	}
}