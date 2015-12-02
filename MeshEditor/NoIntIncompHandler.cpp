#include "StdAfx.h"
#include "NoIntIncompHandler.h"


NoIntIncompHandler::NoIntIncompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs,
	FACE *_inter_face, std::unordered_set<OvmFaH> *_constant_fhs_ptr)
	: OneSimpleHandlerBase (_mesh, _input_ehs)
{
	inter_face = _inter_face;
	constant_fhs_ptr = _constant_fhs_ptr;
}


NoIntIncompHandler::~NoIntIncompHandler(void)
{
}

std::unordered_set<OvmCeH> NoIntIncompHandler::get_hexa_set ()
{
	hexa_set.clear ();
	//���interface�ϵ�������
	get_fhs_on_interface ();
	//����input_ehs��fhs_on_interface���зֿ��
	JC::get_separate_fhs_patches (mesh, interface_fhs, input_ehs, separated_fhs_patches);
	//���separated_fhs_patchesֻ��һ�飬��˵��input_ehs�����Խ�ָ�����ı����漯�зֿ����������������Ч��
	if (separated_fhs_patches.size () == 1){
		last_err_str = QObject::tr("������߲��ܽ��������з֣�");
		return hexa_set;
	}

	//��ÿ��Խ����Ż����漯���ⲿ���漯����������ϳ��˲��ɱߵ��Լ��������ϵ��ı�����
	get_optimize_allowed_fhs ();

	//������벻�����㣬��ô��Ҫ��separated_fhs_patches���ҳ�һ�������е�input_ehs���ڵ�patch��Ȼ�����pillowing
	auto best_patch = get_best_patch ();
	if (best_patch.empty ()){
		last_err_str = QObject::tr("�Ҳ�����������������ڵ��漯��");
		return hexa_set;
	}

	////��ú�best_patchֱ�����ڵ������弯�ϣ��ⲿ�������弯����shrink set�б��������
	//JC::get_direct_adjacent_hexas (mesh, best_patch, hexa_set);

	////����ⲿ�������弯�ϵı����ı��Σ�Ȼ������Ż�
	//foreach (auto &ch, hexa_set){
	//	auto hfh_vec = mesh->cell (ch).halffaces ();
	//	foreach (auto &hfh, hfh_vec){
	//		auto fh = mesh->face_handle (hfh);
	//		if (mesh->is_boundary (fh) && !JC::contains (best_patch, fh)) 
	//			best_patch.insert (fh);
	//	}
	//}
	//JC::optimize_quad_patch (mesh, best_patch, optimize_allowed_fhs);

	////�ٴλ���µ������弯��
	//hexa_set.clear ();
	//JC::get_direct_adjacent_hexas (mesh, best_patch, hexa_set);

	////�Ż���������弯��
	//JC::optimize_shrink_set (mesh, hexa_set, optimize_allowed_fhs);
	hexa_set.clear ();
	std::unordered_set<OvmCeH> rest_hexa_set;
	depth_control (best_patch, hexa_set, rest_hexa_set);
	JC::get_direct_adjacent_hexas (mesh, *constant_fhs_ptr, rest_hexa_set);

	JC::optimize_shrink_set_test (mesh, hexa_set, rest_hexa_set);

	return hexa_set;
}

void NoIntIncompHandler::depth_control (std::unordered_set<OvmFaH> &best_patch, std::unordered_set<OvmCeH> &min_hexa_set,
	std::unordered_set<OvmCeH> &rest_hexa_set)
{
	std::unordered_set<OvmFaH> rest_patch;
	foreach (auto &fh, interface_fhs){
		if (!JC::contains (best_patch, fh))
			rest_patch.insert (fh);
	}

	int depth = 0;
	auto fGetVertices = [&] (std::unordered_set<OvmFaH> &fhs, 
		std::unordered_set<OvmVeH> &inner_vhs, std::unordered_set<OvmVeH> &all_vhs){
			std::hash_map<OvmEgH, int> ehs_count;
			foreach (auto &fh, fhs){
				auto heh_vec = mesh->face (fh).halfedges ();
				foreach (auto &heh, heh_vec){
					auto eh = mesh->edge_handle (heh);
					ehs_count[eh]++;
				}
			}
			std::unordered_set<OvmVeH> bound_vhs;
			foreach (auto &p, ehs_count){
				auto vh1 = mesh->edge (p.first).from_vertex (),
					vh2 = mesh->edge (p.first).to_vertex ();
				all_vhs.insert (vh1); all_vhs.insert (vh2);
				if (p.second == 1){
					bound_vhs.insert (vh1); bound_vhs.insert (vh2);
				}
			}
			foreach (auto &vh, all_vhs){
				if (!JC::contains (bound_vhs, vh))
					inner_vhs.insert (vh);
			}
	};
	auto cur_patch = best_patch;
	while (depth++ < min_depth){
		std::unordered_set<OvmVeH> inner_vhs, all_vhs;
		fGetVertices (cur_patch, inner_vhs, all_vhs);
		foreach (auto &fh, cur_patch){
			auto hfh = mesh->halfface_handle (fh, 0);
			auto ch = mesh->incident_cell (hfh);
			if (ch != mesh->InvalidCellHandle)
				min_hexa_set.insert (ch);
			hfh =  mesh->halfface_handle (fh, 1);
			ch = mesh->incident_cell (hfh);
			if (ch != mesh->InvalidCellHandle)
				min_hexa_set.insert (ch);
		}
		foreach (auto &vh, inner_vhs){
			std::unordered_set<OvmCeH> adj_chs;
			JC::get_adj_hexas_around_vertex (mesh, vh, adj_chs);
			foreach (auto &ch, adj_chs)
				min_hexa_set.insert (ch);
		}

		inner_vhs.clear (); all_vhs.clear ();
		fGetVertices (rest_patch, inner_vhs, all_vhs);
		foreach (auto &vh, all_vhs){
			std::unordered_set<OvmCeH> adj_chs;
			JC::get_adj_hexas_around_vertex (mesh, vh, adj_chs);
			foreach (auto &ch, adj_chs){
				if (!JC::contains (min_hexa_set, ch))
					rest_hexa_set.insert (ch);
			}
		}

		std::unordered_set<OvmFaH> tmp_fhs, bound_fhs1, bound_fhs2;
		JC::collect_boundary_element (mesh, min_hexa_set, NULL, NULL, &tmp_fhs);
		foreach (auto &fh, tmp_fhs){
			if (!mesh->is_boundary (fh)) bound_fhs1.insert (fh);
		}
		tmp_fhs.clear ();
		JC::collect_boundary_element (mesh, rest_hexa_set, NULL, NULL, &tmp_fhs);
		foreach (auto &fh, tmp_fhs){
			if (!mesh->is_boundary (fh)) bound_fhs2.insert (fh);
		}
		cur_patch.clear ();
		foreach (auto &fh, bound_fhs1){
			if (!JC::contains (bound_fhs2, fh)) cur_patch.insert (fh);
		}
		rest_patch.clear ();
		foreach (auto &fh, bound_fhs2){
			if (!JC::contains (bound_fhs1, fh)) rest_patch.insert (fh);
		}
	}
}

std::unordered_set<OvmFaH> NoIntIncompHandler::get_inflation_fhs ()
{
	inflation_fhs.clear ();
	//��ÿ�����sheet Inflation���ı����漯
	get_inflation_fhs_from_hexa_set (hexa_set);
	return inflation_fhs;
}

DualSheet * NoIntIncompHandler::inflate_new_sheet ()
{
	auto new_sheets = JC::one_simple_sheet_inflation (mesh, inflation_fhs, hexa_set);
	return new_sheets.front ();
}

std::unordered_set<OvmFaH> NoIntIncompHandler::get_best_patch ()
{
	std::unordered_set<OvmFaH> best_patch;
	int patch_fhs_count = std::numeric_limits<int>::max ();
	foreach (auto &patch, separated_fhs_patches){
		bool is_patch_ok = true;
		foreach (auto &eh, input_ehs){
			std::unordered_set<OvmFaH> adj_fhs;
			JC::get_adj_boundary_faces_around_edge (mesh, eh, adj_fhs);
			if (!JC::intersects (adj_fhs, patch)){
				is_patch_ok = false; break;
			}
		}
		if (is_patch_ok){
			if (patch.size () < patch_fhs_count){
				best_patch = patch; patch_fhs_count = patch.size ();
			}
		}
	}
	return best_patch;
}