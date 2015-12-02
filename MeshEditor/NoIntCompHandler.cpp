#include "StdAfx.h"
#include "NoIntCompHandler.h"
#include "mesh_min_cut.h"
#include "hoopsview.h"

NoIntCompHandler::NoIntCompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs)
	: OneSimpleHandlerBase (_mesh, _input_ehs)
{
}


NoIntCompHandler::~NoIntCompHandler(void)
{
}

std::unordered_set<OvmCeH> NoIntCompHandler::get_hexa_set ()
{
	//hexa_set.clear ();

	//get_fhs_on_boundary ();
	////����input_ehs��fhs_on_interface���зֿ��
	//JC::get_separate_fhs_patches (mesh, boundary_fhs, input_ehs, separated_fhs_patches);

	////���separated_fhs_patchesֻ��һ�飬��˵��input_ehs�����Խ�ָ�����ı����漯�зֿ����������������Ч��
	//if (separated_fhs_patches.size () != 2){
	//	last_err_str = QObject::tr("������߲��ܽ���������ֳ����ݣ�");
	//	return hexa_set;
	//}

	//std::vector<std::unordered_set<OvmCeH> > hexa_sets;
	//hexa_sets.resize (2);
	//for (int i = 0; i != 2; ++i){
	//	JC::get_direct_adjacent_hexas (mesh, separated_fhs_patches[i], hexa_sets[i]);
	//}

	//auto group = new VolumeMeshElementGroup (mesh, "tmp", "hexa set 1");
	//auto cut_fhs = get_volume_mesh_min_cut (mesh, hexa_sets.front (), hexa_sets.back ());
	//group->fhs = cut_fhs;
	//MeshRenderOptions render_options1;
	//render_options1.face_color = "red";
	//hoopsview->render_mesh_group (group, render_options1);
	//hoopsview->show_mesh_faces (false);
	return hexa_set;

	//auto best_patch = separated_fhs_patches.front ().size () < separated_fhs_patches.back ().size ()?
	//	separated_fhs_patches.front () : separated_fhs_patches.back ();

	////��ú�best_patchֱ�����ڵ������弯�ϣ��ⲿ�������弯����shrink set�б��������
	//JC::get_direct_adjacent_hexas (mesh, best_patch, hexa_set);

	////�Ż���������弯��
	//JC::optimize_shrink_set_test (mesh, hexa_set, std::unordered_set<OvmCeH> ());
	//return hexa_set;
}

std::unordered_set<OvmFaH> NoIntCompHandler::get_inflation_fhs ()
{
	inflation_fhs.clear ();
	//get_inflation_fhs_from_hexa_set (hexa_set);
	//return inflation_fhs;

	get_fhs_on_boundary ();
	//����input_ehs��fhs_on_interface���зֿ��
	JC::get_separate_fhs_patches (mesh, boundary_fhs, input_ehs, separated_fhs_patches);

	//���separated_fhs_patchesֻ��һ�飬��˵��input_ehs�����Խ�ָ�����ı����漯�зֿ����������������Ч��
	if (separated_fhs_patches.size () != 2){
		last_err_str = QObject::tr("������߲��ܽ���������ֳ����ݣ�");
		return inflation_fhs;
	}

	std::vector<std::unordered_set<OvmCeH> > hexa_sets;
	hexa_sets.resize (2);
	for (int i = 0; i != 2; ++i){
		JC::get_direct_adjacent_hexas (mesh, separated_fhs_patches[i], hexa_sets[i]);
	}

	auto cut_fhs = get_volume_mesh_min_cut (mesh, hexa_sets.front (), hexa_sets.back ());
	inflation_fhs = cut_fhs;

	hexa_set = hexa_sets.front ().size () < hexa_sets.back ().size ()? hexa_sets.front () : hexa_sets.back ();

	auto seed_ch = *(hexa_set.begin ());
	std::queue<OvmCeH> spread_set;
	std::unordered_set<OvmCeH> visied_chs, adj_chs;
	visied_chs.insert (seed_ch);
	auto hfhs = mesh->cell (seed_ch).halffaces ();
	foreach (auto hfh, hfhs){
		auto fh = mesh->face_handle (hfh);
		if (JC::contains (cut_fhs, fh)) continue;
		auto oppo_hfh = mesh->opposite_halfface_handle (hfh);
		auto inci_ch = mesh->incident_cell (oppo_hfh);
		if (inci_ch != mesh->InvalidCellHandle){
			visied_chs.insert (inci_ch);
			hexa_set.insert (inci_ch);
			spread_set.push (inci_ch);
		}
	}

	while (!spread_set.empty ()){
		auto front_ch = spread_set.front ();
		spread_set.pop ();
		hfhs = mesh->cell (front_ch).halffaces ();
		foreach (auto hfh, hfhs){
			auto fh = mesh->face_handle (hfh);
			if (JC::contains (cut_fhs, fh)) continue;
			auto oppo_hfh = mesh->opposite_halfface_handle (hfh);
			auto inci_ch = mesh->incident_cell (oppo_hfh);
			if (inci_ch == mesh->InvalidCellHandle) continue;
			if (JC::contains (visied_chs, inci_ch)) continue;

			visied_chs.insert (inci_ch);
			hexa_set.insert (inci_ch);
			spread_set.push (inci_ch);
		}
	}

	return inflation_fhs;
}

DualSheet * NoIntCompHandler::inflate_new_sheet ()
{
	auto new_sheets = JC::one_simple_sheet_inflation (mesh, inflation_fhs, hexa_set);
	return new_sheets.front ();
}