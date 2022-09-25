#pragma once

#include "texture.h"


class material {
public:
	vector<shared_ptr<texture>> color_map_list;
	
	material() {
		color_map_list = vector<shared_ptr<texture>>();
	}

	void add_color_map(shared_ptr<texture> color_map_ptr) {
		color_map_list.push_back(color_map_ptr);
	}

	void make_uniform(Shader shader) {
		shader.Use();

		// 传入color_map
		int color_map_index = 0;
		char color_map_name[] = "color_map_x";

		// color_map在shader中的变量声明：
		// uniform sampler2D color_map_0;
		// uniform sampler2D color_map_1;
		// ...
		for (shared_ptr<texture> color_map_ptr : color_map_list) {
			color_map_name[10] = '0' + color_map_index; // 确定shader中的变量名
			color_map_ptr->make_uniform(shader, color_map_name, color_map_index);
			color_map_index++;
		}
	}
};
