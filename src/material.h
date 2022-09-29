#pragma once

#include "texture.h"


class material {
public:
	vector<shared_ptr<texture>> color_map_list;
	vector<shared_ptr<texture>> shadow_map_list;
	
	material() {
		color_map_list = vector<shared_ptr<texture>>();
		shadow_map_list = vector<shared_ptr<texture>>();
	}

	void add_color_map(shared_ptr<texture> color_map_ptr) {
		color_map_list.push_back(color_map_ptr);
	}

	void add_shadow_map(shared_ptr<texture> shadow_map_ptr) {
		shadow_map_list.push_back(shadow_map_ptr);
	}

	void make_uniform(Shader shader) {
		shader.Use();

		int texture_index = 0;

		// ����color_map
		// color_map��shader�еı���������
		// uniform sampler2D color_map_0;
		// uniform sampler2D color_map_1;
		// ...
		int color_map_index = 0;
		char color_map_name[] = "color_map_x";
		for (shared_ptr<texture> color_map_ptr : color_map_list) {
			color_map_name[10] = '0' + color_map_index; // ȷ��shader�еı�����
			color_map_ptr->make_uniform(shader, color_map_name, texture_index);
			color_map_index++;
			texture_index++;
		}

		// ����shadow_map
		// shadow_map��shader�еı�������:
		// uniform sampler2D shadow_map_0;
		// uniform sampler2D shadow_map_1;
		// ...
		int shadow_map_index = 0;
		char shadow_map_name[] = "shadow_map_x";
		for (shared_ptr<texture> shadow_map_ptr : shadow_map_list) {
			shadow_map_name[11] = '0' + shadow_map_index; // ȷ��shader�еı�����
			shadow_map_ptr->make_uniform(shader, shadow_map_name, texture_index);
			shadow_map_index++;
			texture_index++;
		}
	}
};
