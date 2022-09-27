#pragma once

#include "mesh.h"

class object {
public:

	vector<shared_ptr<mesh>> mesh_list;
	bool buffered = false;

	object(const char file_name[], const unordered_map<string, shared_ptr<material>> &material_dict) {

		// ��ȡģ��
		objl::Loader loader;
		loader.LoadFile(file_name);

		// ����mesh������ָ������mesh_list��
		for (const objl::Mesh& objl_mesh : loader.LoadedMeshes) {

			// ��Ҫ���ڹ���mesh��materialָ��
			shared_ptr<material> current_mat_ptr = nullptr;

			// ����mesh���Ʋ��Ҷ�Ӧ��material
			cout << "loading " << objl_mesh.MeshName << "   material found: ";
			auto itr = material_dict.find(objl_mesh.MeshName);
			if (itr == material_dict.end()) { // ���û��ָ����mesh��material��ʹ��nullptr
				cout << "false\n";
			}
			else { // �Ѿ�ָ���˸�mesh��material
				current_mat_ptr = itr->second;
				cout << "true\n";
			}

			// ���ɡ����mesh
			mesh_list.push_back(make_shared<mesh>(objl_mesh, current_mat_ptr));
		}
	}

	void buffer_data() {
		for (shared_ptr<mesh> mesh_ptr : mesh_list) {
			mesh_ptr->buffer_data();
		}
		buffered = true;
	}

	void draw(Shader shader) {
		if (buffered) {
			for (shared_ptr<mesh> mesh_ptr : mesh_list) {
				mesh_ptr->draw(shader);
			}
		}
		else {
			cout << "object::draw(): mesh data is not buffered\n";
		}
	}
};
