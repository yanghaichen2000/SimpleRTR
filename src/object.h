#pragma once

#include "mesh.h"

class object {
public:

	vector<shared_ptr<mesh>> mesh_list;
	bool buffered = false;

	object(const char file_name[], const unordered_map<string, shared_ptr<material>> &material_dict) {

		// 读取模型
		objl::Loader loader;
		loader.LoadFile(file_name);

		// 生成mesh，并将指针存放在mesh_list中
		for (const objl::Mesh& objl_mesh : loader.LoadedMeshes) {

			// 将要用于构造mesh的material指针
			shared_ptr<material> current_mat_ptr = nullptr;

			// 根据mesh名称查找对应的material
			cout << "loading " << objl_mesh.MeshName << "   material found: ";
			auto itr = material_dict.find(objl_mesh.MeshName);
			if (itr == material_dict.end()) { // 如果没有指定该mesh的material，使用nullptr
				cout << "false\n";
			}
			else { // 已经指定了该mesh的material
				current_mat_ptr = itr->second;
				cout << "true\n";
			}

			// 生成、存放mesh
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
