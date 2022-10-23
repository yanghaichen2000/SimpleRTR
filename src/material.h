#pragma once

#include "texture.h"


class material {
public:
	vector<shared_ptr<texture>> color_map_list;
	vector<shared_ptr<texture>> shadow_map_list;
	vector<shared_ptr<texture>> normal_map_list;
	vector<shared_ptr<texture>> metallic_map_list;
	vector<shared_ptr<texture>> roughness_map_list;
	vector<shared_ptr<texture>> ao_map_list;

	
	material() {
		color_map_list = vector<shared_ptr<texture>>();
		shadow_map_list = vector<shared_ptr<texture>>();
		normal_map_list = vector<shared_ptr<texture>>();
		metallic_map_list = vector<shared_ptr<texture>>();
		roughness_map_list = vector<shared_ptr<texture>>();
		ao_map_list = vector<shared_ptr<texture>>();
	}


	// 添加纹理。这里都是原始版本的纹理，而映射等操作放在shader中进行

	void add_color_map(shared_ptr<texture> color_map_ptr) {
		color_map_list.push_back(color_map_ptr);
	}

	void add_shadow_map(shared_ptr<texture> shadow_map_ptr) {
		shadow_map_list.push_back(shadow_map_ptr);
	}

	void add_normal_map(shared_ptr<texture> normal_map_ptr) {
		normal_map_list.push_back(normal_map_ptr);
	}
	void add_metallic_map(shared_ptr<texture> metallic_map_ptr) {
		metallic_map_list.push_back(metallic_map_ptr);
	}
	void add_roughness_map(shared_ptr<texture> roughness_map_ptr) {
		roughness_map_list.push_back(roughness_map_ptr);
	}
	void add_ao_map(shared_ptr<texture> ao_map_ptr) {
		ao_map_list.push_back(ao_map_ptr);
	}


	// 将纹理加载到shader中

	void make_uniform(Shader shader) {
		shader.Use();

		int texture_index = 0;

		// 传入color_map
		// color_map在shader中的变量声明：
		// uniform sampler2D color_map_0;
		// uniform sampler2D color_map_1;
		// ...
		int color_map_index = 0;
		char color_map_name[] = "color_map_x";
		for (shared_ptr<texture> color_map_ptr : color_map_list) {
			color_map_name[10] = '0' + color_map_index; // 确定shader中的变量名
			color_map_ptr->make_uniform(shader, color_map_name, texture_index);
			color_map_index++;
			texture_index++;
		}

		// 传入shadow_map
		// shadow_map在shader中的变量声明:
		// uniform sampler2D shadow_map_0;
		// uniform sampler2D shadow_map_1;
		// ...
		int shadow_map_index = 0;
		char shadow_map_name[] = "shadow_map_x";
		for (shared_ptr<texture> shadow_map_ptr : shadow_map_list) {
			shadow_map_name[11] = '0' + shadow_map_index; // 确定shader中的变量名
			shadow_map_ptr->make_uniform(shader, shadow_map_name, texture_index);
			shadow_map_index++;
			texture_index++;
		}

		// 传入normal_map
		int normal_map_index = 0;
		char normal_map_name[] = "normal_map_x";
		for (shared_ptr<texture> normal_map_ptr : normal_map_list) {
			normal_map_name[11] = '0' + normal_map_index; // 确定shader中的变量名
			normal_map_ptr->make_uniform(shader, normal_map_name, texture_index);
			normal_map_index++;
			texture_index++;
		}

		// 传入metallic_map
		int metallic_map_index = 0;
		char metallic_map_name[] = "metallic_map_x";
		for (shared_ptr<texture> metallic_map_ptr : metallic_map_list) {
			metallic_map_name[13] = '0' + metallic_map_index; // 确定shader中的变量名
			metallic_map_ptr->make_uniform(shader, metallic_map_name, texture_index);
			metallic_map_index++;
			texture_index++;
		}

		// 传入roughness_map
		int roughness_map_index = 0;
		char roughness_map_name[] = "roughness_map_x";
		for (shared_ptr<texture> roughness_map_ptr : roughness_map_list) {
			roughness_map_name[14] = '0' + roughness_map_index; // 确定shader中的变量名
			roughness_map_ptr->make_uniform(shader, roughness_map_name, texture_index);
			roughness_map_index++;
			texture_index++;
		}

		// 传入ao_map
		int ao_map_index = 0;
		char ao_map_name[] = "ao_map_x";
		for (shared_ptr<texture> ao_map_ptr : ao_map_list) {
			ao_map_name[7] = '0' + ao_map_index; // 确定shader中的变量名
			ao_map_ptr->make_uniform(shader, ao_map_name, texture_index);
			ao_map_index++;
			texture_index++;
		}
	}
};
