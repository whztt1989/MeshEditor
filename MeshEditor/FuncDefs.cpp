#include "StdAfx.h"
#include "FuncDefs.h"
#include <OpenVolumeMesh/Attribs/StatusAttrib.hh>
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include <QMessageBox>
#include <unordered_set>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

namespace JC{


	VolumeMesh* load_volume_mesh (QString mesh_path)
	{
		VolumeMesh* hexa_mesh = new VolumeMesh ();
		OpenVolumeMesh::IO::FileManager file_manager;
		file_manager.readFile (mesh_path.toAscii ().data (), *hexa_mesh, false);
		return hexa_mesh;
	}
	VolumeMesh *form_new_mesh (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs)
	{
		VolumeMesh *new_mesh = new VolumeMesh ();
		auto old_new_vhs_mapping = new std::hash_map<OvmVeH, OvmVeH> ();
		auto all_vhs = new std::unordered_set<OvmVeH> ();
		foreach (auto &ch, chs){
			auto adj_vhs = get_adj_vertices_around_hexa (mesh, ch);
			foreach (auto &vh, adj_vhs)
				all_vhs->insert (vh);
		}
		foreach (auto &vh, *all_vhs){
			auto new_vh = new_mesh->add_vertex (mesh->vertex (vh));
			old_new_vhs_mapping->insert (std::make_pair (vh, new_vh));
		}

		foreach (auto &ch, chs){
			std::vector<OvmVeH> ch_vhs;
			for (auto hv_it = mesh->hv_iter (ch); hv_it; ++hv_it){
				auto old_vh = *hv_it;
				auto new_vh = (*old_new_vhs_mapping)[old_vh];
				ch_vhs.push_back (new_vh);
			}
			new_mesh->add_cell (ch_vhs);
		}

		delete all_vhs;
		delete old_new_vhs_mapping;
		return new_mesh;
	}
	void init_volume_mesh (VolumeMesh *mesh, BODY *body, double myresabs, std::unordered_set<OvmVeH> *left_vhs)
	{
		attach_mesh_elements_to_ACIS_entities (mesh, body, myresabs, left_vhs);
		//SheetSet sheet_set;
		//retrieve_sheets (mesh, sheet_set);
	}

	bool save_volume_mesh (VolumeMesh *mesh, QString mesh_path)
	{
		OpenVolumeMesh::IO::FileManager fileManager;
		//auto new_mesh = new VolumeMesh ();
		//
		//std::hash_map<OvmVeH, OvmVeH> vv_mapping;
		//for (auto v_it = mesh->vertices_begin (); 
		//	v_it != mesh->vertices_end (); ++v_it){
		//		auto vh = *v_it;
		//		auto new_vh = new_mesh->add_vertex (mesh->vertex (vh));
		//		vv_mapping.insert (std::make_pair (vh, new_vh));
		//}
		//for (auto c_it = mesh->cells_begin (); 
		//	c_it != mesh->cells_end (); ++c_it){
		//		std::vector<OvmVeH> vhs;
		//		for (auto hv_it = mesh->hv_iter (*c_it);
		//			hv_it; ++hv_it){
		//				auto vh = *hv_it;
		//				vhs.push_back (vv_mapping[vh]);
		//		}
		//		new_mesh->add_cell (vhs);
		//}

		//bool res = fileManager.writeFile (mesh_path.toStdString (), *new_mesh);
		//delete new_mesh;
		bool res = fileManager.writeFile (mesh_path.toStdString (), *mesh);
		
		return res;
	}

	BODY * load_acis_model (QString file_path)
	{
		FILE *f = fopen (file_path.toAscii ().data (), "r");
		if (!f){
			QMessageBox::critical (NULL, QObject::tr ("打开错误！"), QObject::tr("打开模型文件失败！"));
			return NULL;
		}
		ENTITY_LIST entities;
		api_restore_entity_list (f, TRUE, entities);
		for (int i = 0; i != entities.count (); ++i)
		{
			ENTITY *ent = entities[i];
			if (is_BODY (ent))
			{
				return (BODY *)ent;
			}
		}
		return NULL;
	}

	void save_acis_entity(ENTITY *entity, const char * file_name)
	{
		ENTITY_LIST elist;
		ENTITY *copyentity = NULL;
		api_deep_down_copy_entity (entity, copyentity);
		elist.add (copyentity);
		// Set the units and product_id.
		FileInfo fileinfo;
		fileinfo.set_units (1.0);
		fileinfo.set_product_id ("Example Application");
		outcome result = api_set_file_info((FileId | FileUnits), fileinfo);
		check_outcome(result); 

		// Also set the option to produce sequence numbers in the SAT file.
		result = api_set_int_option("sequence_save_files", 1);
		check_outcome(result);

		// Open a file for writing, save the list of entities, and close the file.
		FILE * save_file = fopen(file_name, "w");
		result = api_save_entity_list(save_file, TRUE, elist);
		fclose(save_file);
		check_outcome(result);
	}

	void save_acis_entity (ENTITY *entity, QString file_name)
	{
		save_acis_entity (entity, file_name.toAscii ().data ());
	}

	bool parse_xml_file (QString xml_path, QString &file_type, QString &data_name, std::vector<std::pair<QString, QString> > &path_pairs)
	{
		QDomDocument doc("mydocument");
		QFile file(xml_path);
		if (!file.open(QIODevice::ReadOnly))
			return false;
		if (!doc.setContent(&file)) {
			file.close();
			return false;
		}
		file.close();
		QDomElement root_elem = doc.documentElement();
		file_type = root_elem.tagName ();
		data_name = root_elem.attribute ("name");
		QDomNode n = root_elem.firstChild();
		std::pair<QString, QString> path_pair;

		if (file_type != "meshedit" && file_type != "meshmatch"){
			QMessageBox::warning (NULL, QObject::tr ("解析错误！"), QObject::tr ("这不是一个有效的文件！"));
			return false;
		}
		QFileInfo fileInfo (xml_path);
		QString xml_file_dir = fileInfo.absolutePath ();

		while (!n.isNull ()){
			QDomElement e = n.toElement();
			path_pair.first = xml_file_dir + "/" + e.attribute ("meshpath");
			path_pair.second = xml_file_dir + "/" + e.attribute ("modelpath");
			path_pairs.push_back (path_pair);
			n = n.nextSibling ();
		}

		return true;
	}

	void save_mesh_element_group (VolumeMeshElementGroup *group, QString group_name)
	{
		QString file_path = /*QString ("mesh element groups/") + */group_name + QString (".xml");
		save_mesh_element_group (group, file_path.toAscii ().data ());
	}

	void save_mesh_element_group (VolumeMeshElementGroup *group, const char *file_path)
	{
		QFile outf (file_path);
		outf.open (QIODevice::WriteOnly | QIODevice::Text);
		QXmlStreamWriter stream (&outf);
		stream.setAutoFormatting(true);
		stream.writeStartDocument();
		stream.writeStartElement("group");

		stream.writeStartElement("vertices");
		stream.writeAttribute("num", QString ("%1").arg (group->vhs.size ()));
		foreach (auto &vh, group->vhs){
			stream.writeTextElement("vh", QString ("%1").arg (vh.idx ()));
		}
		stream.writeEndElement(); // vertices


		stream.writeStartElement("edges");
		stream.writeAttribute("num", QString ("%1").arg (group->ehs.size ()));
		foreach (auto &eh, group->ehs){
			stream.writeTextElement("eh", QString ("%1").arg (eh.idx ()));
		}
		stream.writeEndElement(); // edges

		stream.writeStartElement("faces");
		stream.writeAttribute("num", QString ("%1").arg (group->fhs.size ()));
		foreach (auto &fh, group->fhs){
			stream.writeTextElement("fh", QString ("%1").arg (fh.idx ()));
		}
		stream.writeEndElement(); // faces

		stream.writeStartElement("cells");
		stream.writeAttribute("num", QString ("%1").arg (group->chs.size ()));
		foreach (auto &ch, group->chs){
			stream.writeTextElement("ch", QString ("%1").arg (ch.idx ()));
		}
		stream.writeEndElement(); // cells


		stream.writeEndElement();
		stream.writeEndDocument();
		outf.close ();
	}

	VolumeMeshElementGroup * load_mesh_element_group (QString group_name)
	{
		QString file_path = group_name + QString (".xml");
		VolumeMeshElementGroup *group = load_mesh_element_group (file_path.toAscii ().data ());
		return group;
	}

	VolumeMeshElementGroup * load_mesh_element_group (const char *file_path)
	{
		VolumeMeshElementGroup *group = new VolumeMeshElementGroup (NULL);
		QDomDocument doc("mydocument");
		QFile file(file_path);
		if (!file.open(QIODevice::ReadOnly))
			return false;
		if (!doc.setContent(&file)) {
			file.close();
			return false;
		}
		file.close();

		QDomElement root_elem = doc.documentElement();
		QDomElement vertex_elem = root_elem.firstChildElement ();
		QDomElement edge_elem = vertex_elem.nextSiblingElement ();
		QDomElement face_elem = edge_elem.nextSiblingElement ();
		QDomElement cell_elem = face_elem.nextSiblingElement ();

		int n_vertices = vertex_elem.attribute ("num").toInt ();
		QDomNode ver = vertex_elem.firstChild ();
		for (int i = 0; i != n_vertices; ++i){
			auto ele = ver.toElement ();
			int vh_idx = ele.text ().toInt ();
			group->vhs.insert (OvmVeH (vh_idx));
			ver = ver.nextSibling ();
		}
		int n_edges = edge_elem.attribute ("num").toInt ();
		QDomNode eg = edge_elem.firstChild ();
		for (int i = 0; i != n_edges; ++i){
			auto ele = eg.toElement ();
			auto val = ele.text ();
			int eh_idx = val.toInt ();
			group->ehs.insert (OvmEgH (eh_idx));
			eg = eg.nextSibling ();
		}
		int n_faces = face_elem.attribute ("num").toInt ();
		QDomNode f = face_elem.firstChild ();
		for (int i = 0; i != n_faces; ++i){
			auto ele = f.toElement ();
			int fh_idx = ele.text ().toInt ();
			group->fhs.insert (OvmFaH (fh_idx));
			f = f.nextSibling ();
		}
		int n_cells = cell_elem.attribute ("num").toInt ();
		QDomNode c = cell_elem.firstChild ();
		for (int i = 0; i != n_cells; ++i){
			auto ele = c.toElement ();
			int ch_idx = ele.text ().toInt ();
			group->chs.insert (OvmCeH (ch_idx));
			c = c.nextSibling ();
		}

		return group;
	}

	std::vector<OvmFaH> get_adj_faces_around_edge (VolumeMesh *mesh, OvmHaEgH heh, bool on_boundary)
	{
		std::vector<OvmFaH> fhs;
		for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it)
		{
			OvmFaH fh = mesh->face_handle (*hehf_it);
			if (fh == mesh->InvalidFaceHandle)
				continue;
			if (on_boundary && !mesh->is_boundary (fh))
				continue;
			fhs.push_back (fh);
		}
		return fhs;
	}

	std::vector<OvmFaH> get_adj_faces_around_edge (VolumeMesh *mesh, OvmEgH eh, bool on_boundary)
	{
		return get_adj_faces_around_edge (mesh, mesh->halfedge_handle (eh, 0), on_boundary);
	}

	void get_adj_faces_around_edge (VolumeMesh *mesh, OvmEgH eh, std::unordered_set<OvmFaH> &faces, bool on_boundary)
	{
		get_adj_faces_around_edge (mesh, mesh->halfedge_handle (eh, 0), faces, on_boundary);
	}

	void get_adj_faces_around_edge (VolumeMesh *mesh, OvmHaEgH heh, std::unordered_set<OvmFaH> &faces, bool on_boundary)
	{
		for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it)
		{
			OvmFaH fh = mesh->face_handle (*hehf_it);
			if (fh == mesh->InvalidFaceHandle)
				continue;
			if (on_boundary && !mesh->is_boundary (fh))
				continue;
			faces.insert (fh);
		}
	}

	void get_adj_vertices_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmVeH> &vertices)
	{
		for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it){
			vertices.insert (mesh->halfedge (*voh_it).to_vertex ());
		}
	}

	void get_adj_vertices_around_face (VolumeMesh *mesh, OvmFaH fh, std::vector<OvmVeH> &vertices)
	{
		OvmHaFaH hfh = mesh->halfface_handle (fh, 0);
		for (auto hfv_it = mesh->hfv_iter (hfh); hfv_it; ++hfv_it)
			vertices.push_back (*hfv_it);
	}

	void get_adj_vertices_around_face (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmVeH> &vertices)
	{
		OvmHaFaH hfh = mesh->halfface_handle (fh, 0);
		for (auto hfv_it = mesh->hfv_iter (hfh); hfv_it; ++hfv_it)
			vertices.insert (*hfv_it);
	}

	std::vector<OvmVeH> get_adj_vertices_around_face (VolumeMesh *mesh, OvmFaH fh)
	{
		std::vector<OvmVeH> vertices;
		get_adj_vertices_around_face (mesh, fh, vertices);
		return vertices;
	}

	void get_adj_vertices_around_cell (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmVeH> &vertices)
	{
		for (auto cv_it = mesh->cv_iter (ch); cv_it; ++cv_it)
			vertices.insert (*cv_it);
	}

	std::vector<OvmVeH> get_adj_vertices_around_cell (VolumeMesh *mesh, OvmCeH ch)
	{
		std::vector<OvmVeH> vertices;
		for (auto cv_it = mesh->cv_iter (ch); cv_it; ++cv_it)
			vertices.push_back (*cv_it);
		return vertices;
	}

	std::vector<OvmVeH> get_adj_vertices_around_hexa (VolumeMesh *mesh, OvmCeH ch)
	{
		std::vector<OvmVeH> vertices;
		for (auto cv_it = mesh->hv_iter (ch); cv_it; ++cv_it)
			vertices.push_back (*cv_it);
		return vertices;
	}

	void get_adj_boundary_vertices_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmVeH> &vertices)
	{
		for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it){
			OvmVeH vh = mesh->halfedge (*voh_it).to_vertex ();
			if (mesh->is_boundary (vh))
				vertices.insert (vh);
		}
	}

	void get_adj_hexas_around_edge (VolumeMesh *mesh, OvmHaEgH heh, std::unordered_set<OvmCeH> &hexas)
	{
		for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it){
			OvmCeH ch = mesh->incident_cell (*hehf_it);
			if (ch == mesh->InvalidCellHandle)
				continue;
			hexas.insert (ch);
		}
	}

	void get_adj_hexas_around_edge (VolumeMesh *mesh, OvmEgH eh, std::unordered_set<OvmCeH> &hexas)
	{
		OvmHaEgH heh = mesh->halfedge_handle (eh, 0);
		return get_adj_hexas_around_edge (mesh, heh, hexas);
	}

	void get_adj_faces_around_hexa (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmFaH> &faces)
	{
		auto hfhs = mesh->cell (ch).halffaces ();
		foreach (auto &hfh, hfhs){
			faces.insert (mesh->face_handle (hfh));
		}
	}

	void get_adj_hexas_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmCeH> &hexas)
	{
		for (auto vc_it = mesh->vc_iter (vh); vc_it; ++vc_it){
			OvmCeH ch = *vc_it;
			if (ch == mesh->InvalidCellHandle)
				continue;
			hexas.insert (ch);
		}
	}

	void get_adj_hexas_incident_face (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmCeH> &hexas)
	{
		OvmHaFaH hfh = mesh->halfface_handle (fh, 0);
		OvmCeH ch = mesh->incident_cell (hfh);
		if (ch != mesh->InvalidCellHandle)
			hexas.insert (ch);
		hfh = mesh->halfface_handle (fh, 1);
		ch = mesh->incident_cell (hfh);
		if (ch != mesh->InvalidCellHandle)
			hexas.insert (ch);
	}

	void get_adj_faces_around_face_edge (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmFaH> &faces)
	{
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			std::unordered_set<OvmFaH> adj_faces;
			get_adj_faces_around_edge (mesh, heh, adj_faces);
			foreach (auto &adj_fh, adj_faces){
				if (adj_fh != fh) faces.insert (adj_fh);
			}
		}
	}

	void get_adj_hexas_around_hexa (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmCeH> &hexas)
	{
		for (auto cc_it = mesh->cc_iter (ch); cc_it; ++cc_it){
			if (*cc_it != mesh->InvalidCellHandle)
				hexas.insert (*cc_it);
		}
	}

	void get_adj_edges_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmEgH> &edges)
	{
		for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it){
			edges.insert (mesh->edge_handle (*voh_it));
		}
	}

	void get_adj_edges_around_face (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmEgH> &edges)
	{
		auto hehs = mesh->face (fh).halfedges ();
		foreach (auto &heh, hehs){
			edges.insert (mesh->edge_handle (heh));
		}
	}

	void get_adj_edges_around_cell (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmEgH> &edges)
	{
		auto hfh_vec = mesh->cell (ch).halffaces ();
		foreach (auto &hfh, hfh_vec){
			auto heh_vec = mesh->halfface (hfh).halfedges ();
			foreach (auto &heh, heh_vec)
				edges.insert (mesh->edge_handle (heh));
		}
	}

	void get_adj_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmFaH> &faces)
	{
		for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it)
		{
			auto fhs = get_adj_faces_around_edge (mesh, *voh_it);
			foreach (OvmFaH fh, fhs)
				faces.insert (fh);
		}
	}

	void get_adj_boundary_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmFaH> &faces)
	{
		if (!mesh->is_boundary (vh))
			return;
		for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it)
		{
			if (!mesh->is_boundary (*voh_it))
				continue;
			auto fhs = get_adj_faces_around_edge (mesh, *voh_it);
			foreach (OvmFaH fh, fhs){
				if (mesh->is_boundary (fh))
					faces.insert (fh);
			}
		}
	}

	void get_adj_boundary_faces_around_edge (VolumeMesh *mesh, OvmEgH eh, std::unordered_set<OvmFaH> &faces)
	{
		if (!mesh->is_boundary (eh)) return;
		auto heh = mesh->halfedge_handle (eh, 0);
		for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it){
			auto fh = mesh->face_handle (*hehf_it);
			if (mesh->is_boundary (fh))
				faces.insert (fh);
		}
	}

	void get_adj_boundary_faces_around_face (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmFaH> &faces)
	{
		auto hfh = mesh->halfface_handle (fh, 0);
		if (!mesh->is_boundary (hfh))
			hfh = mesh->opposite_halfface_handle (hfh);
		for (auto bhfhf_it = mesh->bhfhf_iter (hfh); bhfhf_it; ++bhfhf_it)
			faces.insert (mesh->face_handle (*bhfhf_it));
	}

	void get_adj_boundary_edges_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmEgH> &edges)
	{
		for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it){
			auto eh = mesh->edge_handle (*voh_it);
			if (!mesh->is_boundary (eh)) continue;
			edges.insert (eh);
		}
	}

	void get_direct_adjacent_hexas (VolumeMesh *mesh, const std::unordered_set<OvmFaH> &patch, std::unordered_set<OvmCeH> &hexas)
	{
		std::hash_map<OvmEgH, int> edges_counts;
		//get inner edges
		foreach (OvmFaH fh, patch){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (OvmHaEgH heh, heh_vec){
				OvmEgH eh = mesh->edge_handle (heh);
				edges_counts[eh]++;
			}

			OvmHaFaH hfh = mesh->halfface_handle (fh, 0);
			OvmCeH ch = mesh->incident_cell (hfh);
			if (ch == mesh->InvalidCellHandle){
				hfh = mesh->opposite_halfface_handle (hfh);
				ch = mesh->incident_cell (hfh);
				assert (ch != mesh->InvalidCellHandle);
			}
			hexas.insert (ch);
		}

		for (auto it = edges_counts.begin (); it != edges_counts.end (); ++it)
		{
			//pass if the edge is on the boundary of patch
			if (it->second == 1)
				continue;
			auto heh = mesh->halfedge_handle (it->first, 0);
			for (auto hec_it = mesh->hec_iter (heh); hec_it; ++hec_it)
			{
				if (*hec_it != mesh->InvalidCellHandle)
					hexas.insert (*hec_it);
			}
		}
	}

	void collect_boundary_element (VolumeMesh *mesh, std::set<OvmCeH> &chs, 
		std::set<OvmVeH> *bound_vhs, std::set<OvmEgH> *bound_ehs, std::set<OvmHaFaH> *bound_hfhs)
	{
		std::hash_map<OvmFaH, int> fh_count;
		std::set<OvmHaFaH> all_hfhs;
		auto count_fh_on_cell = [&](OvmCeH ch){
			auto hfh_vec = mesh->cell (ch).halffaces ();
			foreach (auto &hfh, hfh_vec){
				fh_count[mesh->face_handle(hfh)]++;
				all_hfhs.insert (hfh);
			}
		};
		std::for_each (chs.begin (), chs.end (), count_fh_on_cell);

		bool newly_created_bound_hfhs = false;
		if (!bound_hfhs){
			bound_hfhs = new std::set<OvmHaFaH> ();
			newly_created_bound_hfhs = true;
		}

		auto collect_bound_hfh = [&](OvmHaFaH hfh){
			auto fh = mesh->face_handle (hfh);
			if (fh_count[fh] == 1)
				bound_hfhs->insert (hfh);
		};
		std::for_each (all_hfhs.begin (), all_hfhs.end (), collect_bound_hfh);

		if (bound_vhs || bound_ehs){
			auto collect_bound_vhs_ehs = [&](OvmHaFaH hfh){
				if (bound_vhs){
					for (auto hfv_iter = mesh->hfv_iter (hfh); hfv_iter; ++hfv_iter)
						bound_vhs->insert (*hfv_iter);
				}
				if (bound_ehs){
					auto heh_vec = mesh->halfface (hfh).halfedges ();
					foreach (auto &heh, heh_vec)
						bound_ehs->insert (mesh->edge_handle (heh));
				}
			};
			std::for_each (bound_hfhs->begin (), bound_hfhs->end (), collect_bound_vhs_ehs);
		}
		if (newly_created_bound_hfhs)
			delete bound_hfhs;
	}

	void collect_boundary_element (VolumeMesh *mesh, std::set<OvmCeH> &chs, 
		std::set<OvmVeH> *bound_vhs, std::set<OvmEgH> *bound_ehs, std::set<OvmFaH> *bound_fhs)
	{
		std::set<OvmHaFaH> bound_hfhs;
		collect_boundary_element (mesh, chs, bound_vhs, bound_ehs, &bound_hfhs);
		if (bound_fhs){
			foreach (OvmHaFaH hfh, bound_hfhs)
				bound_fhs->insert (mesh->face_handle (hfh));
		}
	}

	void collect_boundary_element (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs, 
		std::unordered_set<OvmVeH> *bound_vhs, std::unordered_set<OvmEgH> *bound_ehs, std::unordered_set<OvmHaFaH> *bound_hfhs)
	{
		std::hash_map<OvmFaH, int> fh_count;
		std::unordered_set<OvmHaFaH> all_hfhs;
		auto count_fh_on_cell = [&](OvmCeH ch){
			auto hfh_vec = mesh->cell (ch).halffaces ();
			foreach (auto &hfh, hfh_vec){
				fh_count[mesh->face_handle(hfh)]++;
				all_hfhs.insert (hfh);
			}
		};
		std::for_each (chs.begin (), chs.end (), count_fh_on_cell);

		bool newly_created_bound_hfhs = false;
		if (!bound_hfhs){
			bound_hfhs = new std::unordered_set<OvmHaFaH> ();
			newly_created_bound_hfhs = true;
		}

		auto collect_bound_hfh = [&](OvmHaFaH hfh){
			auto fh = mesh->face_handle (hfh);
			if (fh_count[fh] == 1)
				bound_hfhs->insert (hfh);
		};
		std::for_each (all_hfhs.begin (), all_hfhs.end (), collect_bound_hfh);

		if (bound_vhs || bound_ehs){
			auto collect_bound_vhs_ehs = [&](OvmHaFaH hfh){
				if (bound_vhs){
					for (auto hfv_iter = mesh->hfv_iter (hfh); hfv_iter; ++hfv_iter)
						bound_vhs->insert (*hfv_iter);
				}
				if (bound_ehs){
					auto heh_vec = mesh->halfface (hfh).halfedges ();
					foreach (auto &heh, heh_vec)
						bound_ehs->insert (mesh->edge_handle (heh));
				}
			};
			std::for_each (bound_hfhs->begin (), bound_hfhs->end (), collect_bound_vhs_ehs);
		}
		if (newly_created_bound_hfhs)
			delete bound_hfhs;
	}

	void collect_boundary_element (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs, 
		std::unordered_set<OvmVeH> *bound_vhs, std::unordered_set<OvmEgH> *bound_ehs, std::unordered_set<OvmFaH> *bound_fhs)
	{
		std::unordered_set<OvmHaFaH> bound_hfhs;
		collect_boundary_element (mesh, chs, bound_vhs, bound_ehs, &bound_hfhs);
		if (bound_fhs){
			foreach (OvmHaFaH hfh, bound_hfhs)
				bound_fhs->insert (mesh->face_handle (hfh));
		}
	}

	void collect_boundary_element (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, 
		std::unordered_set<OvmVeH> *bound_vhs, std::unordered_set<OvmEgH> *bound_ehs)
	{
		bool new_created_ehs = false;
		if (bound_ehs == NULL){
			bound_ehs = new std::unordered_set<OvmEgH> ();
			new_created_ehs = true;
		}

		foreach (auto &fh, fhs){
			auto hehs = mesh->face (fh).halfedges ();
			foreach (auto heh, hehs){
				auto eh = mesh->edge_handle (heh);
				if (contains (*bound_ehs, eh))
					bound_ehs->erase (eh);
				else
					bound_ehs->insert (eh);
			}
		}
		if (bound_vhs){
			foreach (auto &eh, *bound_ehs){
				auto eg = mesh->edge (eh);
				bound_vhs->insert (eg.from_vertex ());
				bound_vhs->insert (eg.to_vertex ());
			}
		}
		if (new_created_ehs) delete bound_ehs;
	}

	bool is_adjacent (VolumeMesh *mesh, OvmEgH eh1, OvmEgH eh2)
	{
		if (eh1 == eh2 || eh1 == mesh->InvalidEdgeHandle || eh2 == mesh->InvalidEdgeHandle)
			return false;
		OvmVeH vh11 = mesh->edge (eh1).from_vertex (),
			vh12 = mesh->edge (eh1).to_vertex (),
			vh21 = mesh->edge (eh2).from_vertex (),
			vh22 = mesh->edge (eh2).to_vertex ();
		if (vh11 == vh21 || vh11 == vh22 || vh12 == vh21 || vh12 == vh22)
			return true;
		return false;
	}

	bool is_adjacent (VolumeMesh *mesh, OvmCeH ch1, OvmCeH ch2)
	{
		auto hfh_vec1 = mesh->cell (ch1).halffaces (),
			hfh_vec2 = mesh->cell (ch2).halffaces ();
		for (int i = 0; i != hfh_vec1.size (); ++i){
			OvmFaH fh1 = mesh->face_handle (hfh_vec1[i]);
			for (int j = 0; j != hfh_vec2.size (); ++j){
				OvmFaH fh2 = mesh->face_handle (hfh_vec2[j]);
				if (fh1 == fh2)
					return true;
			}
		}
		return false;
	}

	bool is_adjacent (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmCeH> &chs)
	{
		std::unordered_set<OvmHaFaH> boundary_hfhs;
		collect_boundary_element (mesh, chs, NULL, NULL, &boundary_hfhs);
		std::set<OvmFaH> boundary_fhs;
		foreach (OvmHaFaH hfh, boundary_hfhs)
			boundary_fhs.insert (mesh->face_handle (hfh));

		auto hfh_vec = mesh->cell (ch).halffaces ();
		foreach (OvmHaFaH hfh, hfh_vec){
			auto fh = mesh->face_handle (hfh);
			if (contains (boundary_fhs, fh)) return true;
		}
		return false;
	}

	bool is_hexa_set_connected (VolumeMesh *mesh, const std::unordered_set<OvmCeH> &chs)
	{
		std::queue<OvmCeH> spread_set;
		std::unordered_set<OvmCeH> visited_chs;
		OvmCeH ch = *(chs.begin ());
		spread_set.push (ch);
		visited_chs.insert (ch);

		while (!spread_set.empty ()){
			ch = spread_set.front ();
			spread_set.pop ();
			for (auto cc_it = mesh->cc_iter (ch); cc_it; ++cc_it){
				auto test_ch = *cc_it;
				if (!contains (chs, test_ch) || contains (visited_chs, test_ch))
					continue;
				spread_set.push (test_ch);
				visited_chs.insert (test_ch);
			}
		}
		return equals (visited_chs, chs);
	}

	void get_ccw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh,
		std::vector<OvmEgH> &ehs, std::vector<OvmFaH> &fhs)
	{
		OvmHaEgH start_heh = mesh->InvalidHalfEdgeHandle;
		for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it)
		{
			if (mesh->is_boundary (*voh_it)){
				start_heh = *voh_it;
				break;
			}
		}
		assert (start_heh != mesh->InvalidHalfEdgeHandle);

		auto fGetPrevBoundHehAndFh = [&mesh](OvmHaEgH heh)->std::pair<OvmHaEgH, OvmFaH>{
			OvmHaFaH bound_hfh = mesh->InvalidHalfFaceHandle;
			for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it){
				if (mesh->is_boundary (*hehf_it)){
					bound_hfh = *hehf_it;
					break;
				}
			}
			assert (bound_hfh != mesh->InvalidHalfFaceHandle);
			std::pair<OvmHaEgH, OvmFaH> p;
			p.first = mesh->opposite_halfedge_handle (mesh->prev_halfedge_in_halfface (heh, bound_hfh));
			p.second = mesh->face_handle (bound_hfh);
			return p;
		};

		ehs.push_back (mesh->edge_handle (start_heh));
		auto p = fGetPrevBoundHehAndFh (start_heh);
		fhs.push_back (p.second);
		while (p.first != start_heh){
			ehs.push_back (mesh->edge_handle (p.first));
			p = fGetPrevBoundHehAndFh (p.first);
			fhs.push_back (p.second);
		}
	}

	void get_ccw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh,
		std::unordered_set<OvmEgH> &key_ehs, std::vector<OvmEgH> &ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_sets)
	{
		std::vector<OvmFaH> fhs;
		fhs_sets.resize (key_ehs.size ());

		std::vector<OvmEgH> boundary_ehs;
		get_ccw_boundary_edges_faces_around_vertex (mesh, vh, boundary_ehs, fhs);
		while (!contains (key_ehs, boundary_ehs.front ())){
			boundary_ehs.push_back (boundary_ehs.front ());
			boundary_ehs.erase (boundary_ehs.begin ());

			fhs.push_back (fhs.front ());
			fhs.erase (fhs.begin ());
		}
		ehs.push_back (boundary_ehs.front ());
		int idx = 0;
		for (int i = 0; i != boundary_ehs.size (); ++i){
			fhs_sets[idx].insert (fhs[i]);
			if ((i + 1) == boundary_ehs.size ())
				break;
			if (std::find (key_ehs.begin (), key_ehs.end (), boundary_ehs[i + 1]) != key_ehs.end ()){
				ehs.push_back (boundary_ehs[i + 1]);
				idx++;
			}
		}
	}

	void get_ccw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh,
		std::vector<OvmEgH> &key_ehs, std::vector<OvmEgH> &ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_sets)
	{
		std::unordered_set<OvmEgH> key_ehs_set;
		vector_to_unordered_set (key_ehs, key_ehs_set);
		get_ccw_boundary_edges_faces_around_vertex (mesh, vh, key_ehs_set, ehs, fhs_sets);
	}

	void get_cw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh,
		std::vector<OvmEgH> &ehs, std::vector<OvmFaH> &fhs)
	{
		get_ccw_boundary_edges_faces_around_vertex (mesh, vh, ehs, fhs);
		std::reverse (ehs.begin () + 1, ehs.end ());
		std::reverse (fhs.begin (), fhs.end ());
	}

	void get_cw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh, 
		std::unordered_set<OvmEgH> &key_ehs, std::vector<OvmEgH> &ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_sets)
	{
		std::vector<OvmFaH> fhs;
		fhs_sets.resize (key_ehs.size ());

		std::vector<OvmEgH> boundary_ehs;
		get_cw_boundary_edges_faces_around_vertex (mesh, vh, boundary_ehs, fhs);
		while (!contains (key_ehs, boundary_ehs.front ())){
			boundary_ehs.push_back (boundary_ehs.front ());
			boundary_ehs.erase (boundary_ehs.begin ());

			fhs.push_back (fhs.front ());
			fhs.erase (fhs.begin ());
		}
		ehs.push_back (boundary_ehs.front ());
		int idx = 0;
		for (int i = 0; i != boundary_ehs.size (); ++i){
			fhs_sets[idx].insert (fhs[i]);
			if ((i + 1) == boundary_ehs.size ())
				break;
			if (std::find (key_ehs.begin (), key_ehs.end (), boundary_ehs[i + 1]) != key_ehs.end ()){
				ehs.push_back (boundary_ehs[i + 1]);
				idx++;
			}
		}
	}

	void get_cw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh, 
		std::vector<OvmEgH> &key_ehs, std::vector<OvmEgH> &ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_sets)
	{
		std::unordered_set<OvmEgH> key_ehs_set;
		vector_to_unordered_set (key_ehs, key_ehs_set);
		get_cw_boundary_edges_faces_around_vertex (mesh, vh, key_ehs_set, ehs, fhs_sets);
	}

	OvmVeH get_other_vertex_on_edge (VolumeMesh *mesh, OvmEgH eh, OvmVeH vh)
	{
		OvmVeH vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		if (vh1 == vh) return vh2;
		else if (vh2 == vh) return vh1;
		else return mesh->InvalidVertexHandle;
	}

	OvmEgH get_opposite_edge_on_face (VolumeMesh *mesh, OvmFaH fh, OvmEgH eh)
	{
		auto heh_vec = mesh->face (fh).halfedges ();
		std::vector<OvmEgH> ehs;
		foreach (OvmHaEgH heh, heh_vec)
			ehs.push_back (mesh->edge_handle (heh));

		if (!contains (ehs, eh)) return mesh->InvalidEdgeHandle;
		while (true){
			if (ehs.front () == eh) break;
			auto front_eh = ehs.front ();
			ehs.erase (ehs.begin ());
			ehs.push_back (front_eh);
		}

		return ehs[2];
	}

	void get_boundary_elements_of_faces_patch (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, 
		std::unordered_set<OvmVeH> &bound_vhs, std::unordered_set<OvmEgH> &bound_ehs)
	{
		bound_vhs.clear (); bound_ehs.clear ();
		foreach (auto &fh, fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				auto eh = mesh->edge_handle (heh);
				if (bound_ehs.find (eh) == bound_ehs.end ()){
					bound_ehs.insert (eh);
				}else{
					bound_ehs.erase (eh);
				}
			}
		}

		foreach (auto &eh, bound_ehs){
			auto eg = mesh->edge (eh);
			bound_vhs.insert (eg.to_vertex ());
			bound_vhs.insert (eg.from_vertex ());
		}
	}

	int edge_valence_change (VolumeMesh *mesh, OvmEgH eh, OvmFaH fh1, OvmFaH fh2)
	{
		//assert (!mesh->is_boundary (eh) && fh1 != fh2);
		assert (fh1 != fh2);
		std::vector<OvmFaH> adj_fhs;
		for (auto hehf_it = mesh->hehf_iter (mesh->halfedge_handle (eh, 0)); hehf_it; ++hehf_it)
			adj_fhs.push_back (mesh->face_handle (*hehf_it));

		while (true){
			OvmFaH front_fh = adj_fhs.front ();
			if (front_fh == fh1)
				break;
			else{
				adj_fhs.erase (adj_fhs.begin ());
				adj_fhs.push_back (front_fh);
			}
		}
		auto locate = std::find (adj_fhs.begin (), adj_fhs.end (), fh2);
		assert (locate != adj_fhs.end ());
		int val1 = locate - adj_fhs.begin () + 1;
		int val2 = mesh->valence (eh) + 2 - val1;
		return std::abs(val1 + 1 - 4) + std::abs(val2 + 1 - 4);
	}

	int edge_valence_change (VolumeMesh *mesh, OvmEgH eh, OvmFaH fh)
	{
		assert (mesh->is_boundary (eh));
		std::vector<OvmFaH> adj_fhs;
		for (auto hehf_it = mesh->hehf_iter (mesh->halfedge_handle (eh, 0)); hehf_it; ++hehf_it)
			adj_fhs.push_back (mesh->face_handle (*hehf_it));

		while (true){
			OvmFaH front_fh = adj_fhs.front ();
			adj_fhs.erase (adj_fhs.begin ());
			if (mesh->is_boundary (front_fh) && !mesh->is_boundary (adj_fhs.front ())){
				adj_fhs.insert (adj_fhs.begin (), front_fh);
				break;
			}
			else
				adj_fhs.push_back (front_fh);
		}

		auto locate = std::find (adj_fhs.begin (), adj_fhs.end (), fh);
		assert (locate != adj_fhs.end ());

		int val1 = locate - adj_fhs.begin () + 1;
		int val2 = mesh->valence (eh) + 1 - val1;

		return std::abs(val1 + 1 - 3) + std::abs(val2 + 1 - 3);
	}

	int edge_valence_change (VolumeMesh *mesh, OvmEgH eh)
	{
		int val = mesh->valence (eh);
		assert (!mesh->is_boundary (eh) && val >= 4);
		return (val - 4);
	}

	OvmFaH get_common_face_handle (VolumeMesh *mesh, OvmCeH &ch1, OvmCeH &ch2)
	{
		OvmFaH com_fh = mesh->InvalidFaceHandle;
		auto hfhs_vec1 = mesh->cell (ch1).halffaces (),
			hfhs_vec2 = mesh->cell (ch2).halffaces ();

		std::set<OvmFaH> fhs_set1, fhs_set2;
		std::for_each (hfhs_vec1.begin (), hfhs_vec1.end (), [&](OvmHaFaH hfh){fhs_set1.insert (mesh->face_handle (hfh));});
		std::for_each (hfhs_vec2.begin (), hfhs_vec2.end (), [&](OvmHaFaH hfh){fhs_set2.insert (mesh->face_handle (hfh));});

		std::vector<OvmFaH> com_fhs;
		std::set_intersection (fhs_set1.begin (), fhs_set1.end (), fhs_set2.begin (), fhs_set2.end (),
			std::back_inserter (com_fhs));
		if (com_fhs.size () == 1)
			com_fh = com_fhs.front ();
		return com_fh;
	}

	OvmFaH get_common_face_handle (VolumeMesh *mesh, OvmEgH eh1, OvmEgH eh2)
	{
		OvmFaH com_fh = mesh->InvalidFaceHandle;
		auto heh1 = mesh->halfedge_handle (eh1, 0),
			heh2 = mesh->halfedge_handle (eh2, 0);
		for (auto hf_it1 = mesh->hehf_iter (heh1); hf_it1; ++hf_it1){
			auto fh1 = mesh->face_handle (*hf_it1);
			for (auto hf_it2 = mesh->hehf_iter (heh2); hf_it2; ++hf_it2){
				auto fh2 = mesh->face_handle (*hf_it2);
				if (fh1 == fh2){
					com_fh = fh1;
					break;
				}
			}
		}
		return com_fh;
	}

	OvmVeH get_common_vertex_handle (VolumeMesh *mesh, OvmEgH eh1, OvmEgH eh2)
	{
		OvmVeH v11 = mesh->edge (eh1).from_vertex (), v12 = mesh->edge (eh1).to_vertex ();
		OvmVeH v21 = mesh->edge (eh2).from_vertex (), v22 = mesh->edge (eh2).to_vertex ();
		if (v11 == v21 || v11 == v22)
			return v11;
		if (v12 == v21 || v12 == v22)
			return v12;
		return mesh->InvalidVertexHandle;
	}

	OvmEgH get_common_edge_handle (VolumeMesh *mesh, OvmFaH fh1, OvmFaH fh2)
	{
		auto heh1_vec = mesh->face (fh1).halfedges (),
			heh2_vec = mesh->face (fh2).halfedges ();
		foreach (auto &heh1, heh1_vec){
			auto eh1 = mesh->edge_handle (heh1);
			foreach (auto &heh2, heh2_vec){
				auto eh2 = mesh->edge_handle (heh2);
				if (eh1 == eh2) return eh1;
			}
		}
		return mesh->InvalidEdgeHandle;
	}

	void get_piecewise_halfedges_from_edges (VolumeMesh *mesh, std::vector<OvmEgH> &ehs, bool forward, std::vector<OvmHaEgH> &hehs)
	{
		auto heh = mesh->halfedge_handle (ehs.front (), 0);
		auto first_comm_vh = get_common_vertex_handle (mesh, ehs[0], ehs[1]);
		if (mesh->halfedge (heh).to_vertex () != first_comm_vh)
			heh = mesh->opposite_halfedge_handle (heh);

		hehs.clear ();
		hehs.push_back (heh);
		auto he = mesh->halfedge (heh);
		auto vh = he.to_vertex ();
		for (auto it = ehs.begin () + 1; it != ehs.end (); ++it)
		{
			heh = mesh->halfedge_handle (*it, 0);
			he = mesh->halfedge (heh);
			if (he.from_vertex () != vh)
			{
				heh = mesh->opposite_halfedge_handle (heh);
				he = mesh->halfedge (heh);
			}
			vh = he.to_vertex ();
			hehs.push_back (heh);
		}
		if (!forward){
			std::reverse (hehs.begin (), hehs.end ());
			for (int i = 0; i != hehs.size (); ++i)
				hehs[i] = mesh->opposite_halfedge_handle (hehs[i]);
		}
	}

	void get_piecewise_halfedges_from_edges (VolumeMesh *mesh, std::unordered_set<OvmEgH> &ehs, OvmHaEgH start_heh, bool forward,
		std::vector<OvmHaEgH> &hehs)
	{
		auto start_eh = mesh->edge_handle (start_heh);
		if (contains (ehs, start_eh)) ehs.erase (start_eh);

		hehs.push_back (start_heh);

		while (!ehs.empty ()){
			
		}
	}

	void get_separate_fhs_patches (VolumeMesh *mesh, std::unordered_set<OvmFaH> all_fhs, 
		std::unordered_set<OvmEgH> &seprate_ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_patches)
	{
		auto F_VISITED = mesh->request_face_property<bool> ("visited", false);
		auto fGetOppoFh = [&mesh, &all_fhs] (OvmEgH eh, OvmFaH fh)->OvmFaH{
			auto adj_fhs = get_adj_faces_around_edge (mesh, eh);
			foreach (OvmFaH test_fh, adj_fhs){
				if (test_fh != fh && contains (all_fhs, test_fh))
					return test_fh;
			}
			return mesh->InvalidFaceHandle;
		};
		while (!all_fhs.empty ()){
			OvmFaH seed_fh = pop_begin_element (all_fhs);
			F_VISITED[seed_fh] = true;
			std::unordered_set<OvmFaH> one_fhs_patch;
			one_fhs_patch.insert (seed_fh);

			std::queue<OvmFaH> spread_set;
			spread_set.push (seed_fh);			

			while (!spread_set.empty ()){
				OvmFaH test_fh = spread_set.front ();
				spread_set.pop ();
				auto heh_vec = mesh->face (test_fh).halfedges ();
				foreach (OvmHaEgH heh, heh_vec){
					OvmEgH eh = mesh->edge_handle (heh);
					//遇到输入边的边界，则不再继续扩散
					if (contains (seprate_ehs, eh)) continue;

					OvmFaH oppo_fh = fGetOppoFh (eh, test_fh);
					//如果遇到几何边界，也不再扩散
					if (oppo_fh == mesh->InvalidFaceHandle) continue;
					//如果该四边形已经访问过了，则不再处理
					if (F_VISITED[oppo_fh]) continue;
					one_fhs_patch.insert (oppo_fh);
					spread_set.push (oppo_fh);
					F_VISITED[oppo_fh] = true;
				}
			}//end while (!spread_set.empty ()){...

			fhs_patches.push_back (one_fhs_patch);
			foreach (OvmFaH fh, one_fhs_patch)
				all_fhs.erase (fh);
		}
	}

	void get_separate_chs_sets (VolumeMesh *mesh, std::unordered_set<OvmCeH> *all_chs, std::unordered_set<OvmFaH> *sep_fhs, 
		std::vector<std::unordered_set<OvmCeH> > *sep_chs_sets)
	{
		std::queue<OvmCeH> spread_set;
		while (!all_chs->empty ()){
			auto seed_ch = pop_begin_element (*all_chs);
			std::unordered_set<OvmCeH> visited_chs;
			visited_chs.insert (seed_ch);
			auto hfh_vec = mesh->cell (seed_ch).halffaces ();
			foreach (auto &hfh, hfh_vec){
				auto fh = mesh->face_handle (hfh);
				if (contains (*sep_fhs, fh)) continue;
				auto oppo_hfh = mesh->opposite_halfface_handle (hfh);
				auto inci_ch = mesh->incident_cell (oppo_hfh);
				if (inci_ch != mesh->InvalidCellHandle && contains (*all_chs, inci_ch)){
					spread_set.push (inci_ch);
					visited_chs.insert (inci_ch);
				}
			}
			while (!spread_set.empty ()){
				auto front_ch = spread_set.front ();
				spread_set.pop ();
				auto hfh_vec = mesh->cell (front_ch).halffaces ();
				foreach (auto &hfh, hfh_vec){
					auto fh = mesh->face_handle (hfh);
					if (contains (*sep_fhs, fh)) continue;
					
					auto oppo_hfh = mesh->opposite_halfface_handle (hfh);
					auto inci_ch = mesh->incident_cell (oppo_hfh);
					if (inci_ch != mesh->InvalidCellHandle && contains (*all_chs, inci_ch)){
						if (contains (visited_chs, inci_ch)) continue;
						spread_set.push (inci_ch);
						visited_chs.insert (inci_ch);
					}
				}
			}
			foreach (auto &ch, visited_chs)
				all_chs->erase (ch);
			sep_chs_sets->push_back (visited_chs);
		}
	}

	void get_separate_chs_sets (VolumeMesh *mesh, std::unordered_set<OvmFaH> *sep_fhs, std::vector<std::unordered_set<OvmCeH> > *sep_chs_sets)
	{
		auto all_chs = new std::unordered_set<OvmCeH> ();
		for (auto c_it = mesh->cells_begin (); c_it != mesh->cells_end (); ++c_it){
			all_chs->insert (*c_it);
		}
		get_separate_chs_sets (mesh, all_chs, sep_fhs, sep_chs_sets);
		delete all_chs;
	}

	void get_fhs_on_acis_faces (VolumeMesh *mesh, std::set<FACE *> acis_face, std::unordered_set<OvmFaH> &fhs)
	{
		fhs.clear ();
		//首先获得所有int_faces上的ENTITY的指针集合，包括点、边、面
		std::set<ENTITY*> all_entities;
		foreach (auto f, acis_face){
			all_entities.insert (f);
			ENTITY_LIST edges_list, vertices_list;
			api_get_edges (f, edges_list);
			for (int i = 0; i != edges_list.count (); ++i)
				all_entities.insert (edges_list[i]);

			api_get_vertices (f, vertices_list);
			for (int i = 0; i != vertices_list.count (); ++i)
				all_entities.insert (vertices_list[i]);
		}

		auto V_ENT_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");
		for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
			auto adj_vhs = JC::get_adj_vertices_around_face (mesh, *bf_it);
			bool is_on_inter = true;
			foreach (auto adj_vh, adj_vhs){
				ENTITY* cur_entity = (ENTITY*)(V_ENT_PTR[adj_vh]);
				if (all_entities.find (cur_entity) == all_entities.end ()){
					is_on_inter = false;
					break;
				}
			}
			if (is_on_inter)
				fhs.insert (*bf_it);
		}
	}

	std::unordered_set<OvmFaH> get_fhs_on_acis_faces (VolumeMesh *mesh, std::set<FACE *> acis_face)
	{
		std::unordered_set<OvmFaH> fhs;
		fhs.clear ();
		//首先获得所有int_faces上的ENTITY的指针集合，包括点、边、面
		std::set<ENTITY*> all_entities;
		foreach (auto f, acis_face){
			all_entities.insert (f);
			ENTITY_LIST edges_list, vertices_list;
			api_get_edges (f, edges_list);
			for (int i = 0; i != edges_list.count (); ++i)
				all_entities.insert (edges_list[i]);

			api_get_vertices (f, vertices_list);
			for (int i = 0; i != vertices_list.count (); ++i)
				all_entities.insert (vertices_list[i]);
		}

		auto V_ENT_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");
		for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
			auto adj_vhs = JC::get_adj_vertices_around_face (mesh, *bf_it);
			bool is_on_inter = true;
			foreach (auto adj_vh, adj_vhs){
				ENTITY* cur_entity = (ENTITY*)(V_ENT_PTR[adj_vh]);
				if (all_entities.find (cur_entity) == all_entities.end ()){
					is_on_inter = false;
					break;
				}
			}
			if (is_on_inter)
				fhs.insert (*bf_it);
		}
		return fhs;
	}

	void get_fhs_on_acis_face (VolumeMesh *mesh, FACE *acis_face, std::unordered_set<OvmFaH> &fhs)
	{
		assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");

		fhs.clear ();

		for (auto f_it = mesh->bf_iter (); f_it; f_it++){
			auto f = get_associated_geometry_face_of_boundary_fh (mesh, *f_it);
			if (f == acis_face) fhs.insert (*f_it);
		}
	}

	FACE* get_associated_geometry_face_of_boundary_fh (VolumeMesh *mesh, OvmFaH fh)
	{
		if (!mesh->vertex_property_exists<unsigned int> ("entityptr"))
			return NULL;
		if (!mesh->is_boundary (fh))
			return NULL;
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");
		auto adj_vhs = get_adj_vertices_around_face (mesh, fh);
		std::vector<std::set<FACE*> > all_adj_faces;
		foreach (auto vh, adj_vhs){
			ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
			assert (entity);
			if (is_FACE (entity))
				return (FACE*)entity;
			ENTITY_LIST face_list;
			api_get_faces (entity, face_list);
			std::set<FACE*> cur_adj_faces;
			for (int i = 0; i != face_list.count (); ++i){
				cur_adj_faces.insert ((FACE*)(face_list[i]));
			}
			all_adj_faces.push_back (cur_adj_faces);
		}
		assert (all_adj_faces.size () > 1);
		foreach (FACE *f, all_adj_faces.front ()){
			bool this_ok = true;
			for (int i = 1; i != all_adj_faces.size (); ++i){
				if (!contains (all_adj_faces[i], f)){
					this_ok = false;
					break;
				}
			}
			if (this_ok) return f;
		}
		return NULL;
	}


	EDGE* get_associated_geometry_edge_of_boundary_eh (VolumeMesh *mesh, OvmEgH eh, OpenVolumeMesh::VertexPropertyT<unsigned int> &V_ENTITY_PTR)
	{
		auto vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		ENTITY *entity1 = (ENTITY*)(V_ENTITY_PTR[vh1]),
			*entity2 = (ENTITY*)(V_ENTITY_PTR[vh2]);

		if (entity1 == NULL || entity2 == NULL) return NULL;
		if (is_FACE (entity1) || is_FACE (entity2)) return NULL;

		if (is_EDGE (entity1)){
			if (is_EDGE (entity2)){
				if (entity1 == entity2) return (EDGE*)entity1;
				else return NULL;
			}else if (is_VERTEX (entity2)){
				ENTITY_LIST adj_edges_list;
				api_get_edges (entity2, adj_edges_list);
				if (adj_edges_list.lookup (entity1) != -1) return (EDGE*)entity1;
				else return NULL;
			}
		}else if (is_VERTEX (entity1)){
			ENTITY_LIST adj_edges_list1;
			api_get_edges (entity1, adj_edges_list1);
			if (is_EDGE (entity2)){
				if (adj_edges_list1.lookup (entity2) != -1) return (EDGE*)entity2;
				else return NULL;
			}else if (is_VERTEX (entity2)){
				ENTITY_LIST adj_edges_list2;
				api_get_edges (entity2, adj_edges_list2);
				for (int i = 0; i != adj_edges_list1.count (); ++i){
					for (int j = 0; j != adj_edges_list2.count (); ++j){
						if (adj_edges_list1[i] == adj_edges_list2[j])
							return (EDGE*)(adj_edges_list1[i]);
					}
				}
			}
		}
		return NULL;
	}


	EDGE* get_associated_geometry_edge_of_boundary_eh (VolumeMesh *mesh, OvmEgH eh)
	{
		assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");

		auto vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		ENTITY *entity1 = (ENTITY*)(V_ENTITY_PTR[vh1]),
			*entity2 = (ENTITY*)(V_ENTITY_PTR[vh2]);

		if (entity1 == NULL || entity2 == NULL) return NULL;
		if (is_FACE (entity1) || is_FACE (entity2)) return NULL;

		if (is_EDGE (entity1)){
			if (is_EDGE (entity2)){
				if (entity1 == entity2) return (EDGE*)entity1;
				else return NULL;
			}else if (is_VERTEX (entity2)){
				ENTITY_LIST adj_edges_list;
				api_get_edges (entity2, adj_edges_list);
				if (adj_edges_list.lookup (entity1) != -1) return (EDGE*)entity1;
				else return NULL;
			}
		}else if (is_VERTEX (entity1)){
			ENTITY_LIST adj_edges_list1;
			api_get_edges (entity1, adj_edges_list1);
			if (is_EDGE (entity2)){
				if (adj_edges_list1.lookup (entity2) != -1) return (EDGE*)entity2;
				else return NULL;
			}else if (is_VERTEX (entity2)){
				ENTITY_LIST adj_edges_list2;
				api_get_edges (entity2, adj_edges_list2);
				for (int i = 0; i != adj_edges_list1.count (); ++i){
					for (int j = 0; j != adj_edges_list2.count (); ++j){
						if (adj_edges_list1[i] == adj_edges_list2[j])
							return (EDGE*)(adj_edges_list1[i]);
					}
				}
			}
		}
		return NULL;
	}

	int calc_manifold_edge_marks (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs)
	{
		std::hash_map<OvmEgH, std::set<OvmFaH> > edge_count_mapping;
		foreach (OvmFaH fh, fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (OvmHaEgH heh, heh_vec)
				edge_count_mapping[mesh->edge_handle (heh)].insert (fh);
		}

		//根据不同的边的类型调用不同的度数计算函数
		int total_valence_change = 0;
		foreach (auto &p, edge_count_mapping){
			if (p.second.size () == 1 && mesh->is_boundary (p.first))
				total_valence_change += edge_valence_change (mesh, p.first, *(p.second.begin ()));
			else if (p.second.size () == 2){
				std::vector<OvmFaH> fh_vec;
				set_to_vector (p.second, fh_vec);
				total_valence_change += edge_valence_change (mesh, p.first, fh_vec.front (), fh_vec.back ());
			}
			else
				total_valence_change += edge_valence_change (mesh, p.first);
		}
		return total_valence_change;
	}

	int calc_manifold_exact_inner_edge_marks (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs)
	{
		std::hash_map<OvmEgH, std::vector<OvmFaH> > edge_count_mapping;
		foreach (OvmFaH fh, fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (OvmHaEgH heh, heh_vec)
				edge_count_mapping[mesh->edge_handle (heh)].push_back (fh);
		}

		//根据不同的边的类型调用不同的度数计算函数
		int total_valence_change = 0;
		foreach (auto &p, edge_count_mapping){
			if (p.second.size () != 2)
				continue;
			total_valence_change += 
				edge_valence_change (mesh, p.first, p.second.front (), p.second.back ());
		}
		return total_valence_change;
	}

	bool is_manifold (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs)
	{
		std::hash_map<OvmEgH, int> edge_count_mapping;
		std::unordered_set<OvmVeH> vhs;
		//std::set<OvmVeH> all_vhs;
		foreach (OvmFaH fh, fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (OvmHaEgH heh, heh_vec){
				edge_count_mapping[mesh->edge_handle (heh)]++;
				vhs.insert (mesh->halfedge (heh).to_vertex ());
			}
		}

		//对于sheet生成的流形面集来说，不应出现自相交次数多于两次的情况
		foreach (auto &p, edge_count_mapping){
			if (p.second == 0 || p.second >= 3)
				return false;
		}

		//对面的连通性做检查
		std::queue<OvmFaH> fh_spread_set;
		fh_spread_set.push (*(fhs.begin ()));
		std::unordered_set<OvmFaH> tmp_fhs = fhs;
		tmp_fhs.erase (tmp_fhs.begin ());
		while (!fh_spread_set.empty ()){
			OvmFaH front_fh = fh_spread_set.front ();
			fh_spread_set.pop ();
			auto heh_vec = mesh->face (front_fh).halfedges ();
			foreach (OvmHaEgH test_heh, heh_vec){
				for (auto hehf_it = mesh->hehf_iter (test_heh); hehf_it; ++hehf_it){
					OvmFaH test_adj_fh = mesh->face_handle (*hehf_it);
					if (test_adj_fh == mesh->InvalidFaceHandle || test_adj_fh == front_fh)
						continue;
					auto locate = tmp_fhs.find (test_adj_fh);
					if (locate == tmp_fhs.end ())
						continue;
					tmp_fhs.erase (locate);
					fh_spread_set.push (test_adj_fh);
				}
			}//end foreach (OvmHaEgH test_heh, heh_vec){...
		}//end while (!fh_spread_set.empty ()){...

		//如果tmp_fhs里面还有剩余的面，则说明fhs里面的面并不都是通过边相邻的
		if (!tmp_fhs.empty ())
			return false;

		//对边做检查，如果一条边

		//下面对于点再做进一步的检查
		//对于流形体，每一个点所相邻的四边形都是通过边连接在一起的
		//如果不能够通过边相邻遍历完所有的面，则说明该点不是流形体上的点
		auto get_adj_faces_around_vertex_with_limitation = 
			[&mesh] (OvmVeH vh, std::unordered_set<OvmFaH> &limited_fhs)->std::unordered_set<OvmFaH>{
			std::unordered_set<OvmFaH> adj_fhs;
			for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it){
				for (auto hehf_it = mesh->hehf_iter (*voh_it); hehf_it; ++hehf_it){
					OvmFaH fh = mesh->face_handle (*hehf_it);
					if (fh != mesh->InvalidFaceHandle && contains (limited_fhs, fh))
						adj_fhs.insert (fh);
				}
			}
			return adj_fhs;
		};

		foreach (OvmVeH vh, vhs){
			auto adj_fhs = get_adj_faces_around_vertex_with_limitation (vh, fhs);
			std::queue<OvmFaH> spread_set;
			spread_set.push (*(adj_fhs.begin ()));
			while (!spread_set.empty ()){
				OvmFaH front_fh = spread_set.front ();
				spread_set.pop ();
				auto heh_vec = mesh->face (front_fh).halfedges ();
				foreach (OvmHaEgH heh, heh_vec){
					for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it){
						OvmFaH test_fh = mesh->face_handle (*hehf_it);
						if (test_fh == mesh->InvalidFaceHandle)	continue;
						if (adj_fhs.find (test_fh) == adj_fhs.end ()) continue;
						spread_set.push (test_fh); adj_fhs.erase (test_fh);
					}
				}//end foreach (OvmHaEgH heh, heh_vec){...
			}//end while (!spread_set.empty ()){...

			if (!adj_fhs.empty ()) return false;
		}//end foreach (OvmVeH vh, all_vhs){...
		return true;
	}


	bool is_manifold (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs)
	{
		std::unordered_set<OvmVeH> cells_set_bound_vhs, tmp_bound_vhs;
		std::unordered_set<OvmEgH> cells_set_bound_ehs, tmp_bound_ehs;
		std::unordered_set<OvmFaH> cells_set_bound_fhs, tmp_bound_fhs;
		collect_boundary_element (mesh, chs, &tmp_bound_vhs, &tmp_bound_ehs, &tmp_bound_fhs);
		foreach (auto &vh, tmp_bound_vhs){
			if (!mesh->is_boundary (vh))
				cells_set_bound_vhs.insert (vh);
		}
		foreach (auto &fh, tmp_bound_fhs){
			if (!mesh->is_boundary (fh))
				cells_set_bound_fhs.insert (fh);
		}
		foreach (auto &eh, tmp_bound_ehs){
			if (!mesh->is_boundary (eh))
				cells_set_bound_ehs.insert (eh);
		}

		auto fCellsConnected = [&] (std::unordered_set<OvmCeH> chs)->bool{
			std::queue<OvmCeH> spread_set;
			auto start_ch = pop_begin_element (chs);
			spread_set.push (start_ch);
			while (!spread_set.empty ()){
				auto curr_ch = spread_set.front ();
				spread_set.pop ();
				for (auto cc_it = mesh->cc_iter (curr_ch); cc_it; ++cc_it){
					auto test_ch = *cc_it;
					if (test_ch == mesh->InvalidCellHandle) continue;
					if (!contains (chs, test_ch)) continue;
					chs.erase (test_ch);
					spread_set.push (test_ch);
				}
			}
			return chs.empty ();
		};

		foreach (OvmVeH vh, cells_set_bound_vhs){
			std::unordered_set<OvmCeH> adj_chs;
			get_adj_hexas_around_vertex (mesh, vh, adj_chs);

			std::unordered_set<OvmCeH> adj_chs_of_vh_in_hs;
			foreach (OvmCeH ch, adj_chs){
				if (contains (chs, ch)) adj_chs_of_vh_in_hs.insert (ch);
			}

			if (!fCellsConnected (adj_chs_of_vh_in_hs)) return false;
		}

		foreach (OvmEgH eh, cells_set_bound_ehs){
			std::unordered_set<OvmCeH> adj_chs;
			get_adj_hexas_around_edge (mesh, eh, adj_chs);

			std::unordered_set<OvmCeH> adj_chs_of_vh_in_hs;
			foreach (OvmCeH ch, adj_chs){
				if (contains (chs, ch)) adj_chs_of_vh_in_hs.insert (ch);
			}

			if (!fCellsConnected (adj_chs_of_vh_in_hs)) return false;
		}
		return true;
	}

	double calc_interior_angle (const OvmVec3d &vec1, const OvmVec3d &vec2)
	{
		double angle_cos = (vec1 | vec2) / (vec1.length () * vec2.length ());
		return acos (angle_cos);
	}

	void attach_mesh_elements_to_ACIS_entities (VolumeMesh *mesh, BODY *body, double myresabs, std::unordered_set<OvmVeH> *left_vhs)
	{
		ENTITY_LIST vertices_list, edges_list, faces_list;
		api_get_vertices (body, vertices_list);
		api_get_edges (body, edges_list);
		api_get_faces (body, faces_list);

		SPAposition closest_pos;
		double dist = 0.0f;
		auto fOnWhichEntity = [&myresabs, &closest_pos, &dist](ENTITY_LIST &entity_list, SPAposition &pos)->ENTITY*{
			for (int i = 0; i != entity_list.count (); ++i){
				api_entity_point_distance (entity_list[i], pos, closest_pos, dist);
				if (dist < myresabs)
					return entity_list[i];
			}
			return NULL;
		};

		auto V_ENT_PTR = mesh->request_vertex_property<unsigned int> ("entityptr", 0);
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
		{
			if (!mesh->is_boundary (*v_it))
				continue;

			SPAposition spa_pos = POS2SPA(mesh->vertex (*v_it));
			ENTITY* on_this_vertex = fOnWhichEntity (vertices_list, spa_pos);
			if (on_this_vertex)
				V_ENT_PTR[*v_it] = (unsigned int)on_this_vertex;
			else{
				ENTITY* on_this_edge = fOnWhichEntity (edges_list, spa_pos);
				if (on_this_edge)
					V_ENT_PTR[*v_it] = (unsigned int)on_this_edge;
				else{
					ENTITY* on_this_face = fOnWhichEntity (faces_list, spa_pos);
					if (on_this_face)
						V_ENT_PTR[*v_it] = (unsigned int)on_this_face;
					else{
						/*QMessageBox::critical (NULL, QObject::tr("错误!"), QObject::tr("这个点找不到对应的ACIS Entity!"));
						assert (false);*/
						if (left_vhs)
							left_vhs->insert (*v_it);
					}
				}//end if (on_this_edge)
			}//end if (on_this_vertex)
		}
		mesh->set_persistent (V_ENT_PTR);
	}
}

