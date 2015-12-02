#include "StdAfx.h"
#include "TwoIntIncompHandler.h"


TwoIntIncompHandler::TwoIntIncompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs, 
	FACE *_inter_face, std::unordered_set<OvmFaH> *_constant_fhs_ptr, 
	std::hash_map<OvmVeH, std::vector<OvmEgH> > *_vh_adj_ehs_mapping, std::vector<OvmVeH> &_int_vhs)
	: OneSimpleHandlerBase (_mesh, _input_ehs)
{
	inter_face = _inter_face;
	constant_fhs_ptr = _constant_fhs_ptr;
	vh_adj_ehs_mapping = _vh_adj_ehs_mapping;
	int_vhs = _int_vhs;
}


TwoIntIncompHandler::~TwoIntIncompHandler(void)
{
}

std::unordered_set<OvmCeH> TwoIntIncompHandler::get_hexa_set ()
{
	hexa_set.clear ();
	//���interface�ϵ�������
	get_fhs_on_interface ();
	//����input_ehs��fhs_on_interface���зֿ��
	JC::get_separate_fhs_patches (mesh, interface_fhs, input_ehs, separated_fhs_patches);
	//���separated_fhs_patchesֻ��һ�飬��˵��input_ehs�����Խ�ָ�����ı����漯�зֿ����������������Ч��
	if (separated_fhs_patches.size () == 4){
		last_err_str = QObject::tr("������߲��ܽ��������з֣�");
		return hexa_set;
	}

	//��ÿ��Խ����Ż����漯���ⲿ���漯����������ϳ��˲��ɱߵ��Լ��������ϵ��ı�����
	get_optimize_allowed_fhs ();

	//��������ı�����������mid_patchΪ�м�������������ֱ������Խ����ڣ�ͬʱpatch1��int_vhs[0]���ڣ�patch2��int_vhs[1]����
	std::unordered_set<OvmFaH> mid_patch, diagonal_quad_patch1, diagonal_quad_patch2;
	get_diagonal_quad_patches (mid_patch, diagonal_quad_patch1, diagonal_quad_patch2);
	if (mid_patch.empty () || diagonal_quad_patch1.empty () || diagonal_quad_patch2.empty ()){
		last_err_str = QObject::tr ("��loop�޷������Ч�ĶԽ��ı��μ��ϣ�");
		return hexa_set;
	}

	//������������弯��
	std::unordered_set<OvmCeH> mid_hexa_set, diagonal_hexa_set;
	JC::get_direct_adjacent_hexas (mesh, mid_patch, mid_hexa_set);
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch1, diagonal_hexa_set);
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch2, diagonal_hexa_set);

	//�����������㸽���漯�ϵ�ӳ���
	CrossFaceSetPairs cross_face_set_pairs;
	get_cross_faces_pairs (diagonal_quad_patch1, diagonal_quad_patch2, cross_face_set_pairs);

	//����·�������õ��ӵ�һ�����㵽�ڶ��������·�����Լ��������·�����ĸ������弯��
	search_constrained_cross_hexa_sets (cross_face_set_pairs);

	//���������Ľ���·���ϵ������弯��ͨ���ж��Ƿ�������ҵ����������������������Ǽ��뵽��������������弯����
	foreach (auto &one_hexa_set, cross_hexas){
		if (JC::intersects (one_hexa_set, mid_hexa_set)){
			foreach (auto &ch, one_hexa_set)
				mid_hexa_set.insert (ch);
			continue;
		}
		if (JC::intersects (one_hexa_set, diagonal_hexa_set)){
			foreach (auto &ch, one_hexa_set)
				diagonal_hexa_set.insert (ch);
			continue;
		}
	}//end foreach (auto &hexa_set, cross_hexas){...

	//�Ա����ı��ν����Ż�
	optimize_diagonal_hexa_set_boundary_quads (mid_hexa_set, diagonal_hexa_set);

	//�Ի�õ����������弯�Ͻ����Ż�
	optimize_diagonal_hexa_sets (mid_hexa_set, diagonal_hexa_set);

	//��ÿ�����sheet Inflation���ı����漯
	hexa_set = mid_hexa_set;
	foreach (auto &ch, diagonal_hexa_set)
		hexa_set.insert (ch);

	return hexa_set;
}

std::unordered_set<OvmFaH> TwoIntIncompHandler::get_inflation_fhs ()
{
	inflation_fhs.clear ();
	get_inflation_fhs_from_hexa_set (hexa_set);
	return inflation_fhs;
}

DualSheet * TwoIntIncompHandler::inflate_new_sheet ()
{
	std::unordered_set<OvmEgH> intersect_path_set;
	JC::vector_to_unordered_set (intersect_path, intersect_path_set);
	auto new_sheets = JC::one_simple_sheet_inflation (mesh, inflation_fhs, hexa_set, intersect_path_set);
	return new_sheets.front ();
}