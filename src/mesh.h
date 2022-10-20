#pragma once
#include "global.h"
#include "OBJ_loader.h"
#include <vector>
#include <memory>
#include "material.h"

class mesh {
public:
	objl::Mesh mesh_data;
	shared_ptr<material> mat_ptr;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	int num_indices = 0; // 索引数量，同时可以用于检测数据是否buffer过

public:
	mesh(const objl::Mesh &mesh_data_init, shared_ptr<material> mat_ptr_init) {
		mesh_data = mesh_data_init;
		mat_ptr = mat_ptr_init;
	}

	void buffer_data() {
		// 数据结构：
		// triangle1 {
		// position1.x position1.y position1.z normal1.x normal1.y normal1.z uv1.x uv1.y
		// position2.x position2.y position2.z normal2.x normal2.y normal2.z uv2.x uv2.y
		// position3.x position3.y position3.z normal3.x normal3.y normal3.z uv3.x uv3.y
		// }
		// triangle2 {
		// ...
		// }
		// ...

		// shader输入：
		// layout(location = 0) in vec3 position;
		// layout(location = 1) in vec3 normal;
		// layout(location = 2) in vec2 uv;


		// 计算顶点个数
		int	num_vertex = mesh_data.Vertices.size();

		// 为属性信息分配空间
		float* attribute_array = new float[num_vertex * 8];

		// 将属性信息放入attribute_array
		int index_vertex = 0; // 顶点编号
		for (const objl::Vertex& vertex : mesh_data.Vertices) {
			attribute_array[index_vertex * 8 + 0] = -vertex.Position.Z; // 这里的位置进行了一些变换
			attribute_array[index_vertex * 8 + 1] = vertex.Position.Y;
			attribute_array[index_vertex * 8 + 2] = vertex.Position.X;
			attribute_array[index_vertex * 8 + 3] = -vertex.Normal.Z; // 这里的位置进行了一些变换
			attribute_array[index_vertex * 8 + 4] = vertex.Normal.Y;
			attribute_array[index_vertex * 8 + 5] = vertex.Normal.X;
			attribute_array[index_vertex * 8 + 6] = vertex.TextureCoordinate.X;
			attribute_array[index_vertex * 8 + 7] = vertex.TextureCoordinate.Y;
			index_vertex++;
		}

		// 计算索引个数（是三角形个数的三倍）
		num_indices = mesh_data.Indices.size();;

		// 为顶点索引信息分配空间
		GLuint* index_array = new GLuint[num_indices];

		// 将顶点索引信息放入index_array
		int index_triangle = 0;
		int num_triangles_in_this_mesh = mesh_data.Indices.size() / 3;
		for (int i = 0; i < num_triangles_in_this_mesh; i++) {
			index_array[index_triangle * 3] = mesh_data.Indices[i * 3];
			index_array[index_triangle * 3 + 1] = mesh_data.Indices[i * 3 + 1];
			index_array[index_triangle * 3 + 2] = mesh_data.Indices[i * 3 + 2];
			index_triangle++;
		}
		
		// 创建并配置VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		{
			// 创建并buffer VBO
			GLuint VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 8, attribute_array, GL_STATIC_DRAW);

			// 创建并buffer EBO
			GLuint EBO;
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, index_array, GL_STATIC_DRAW);

			// 设置顶点数据指针
			// position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			// normal
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			// uv
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		glBindVertexArray(0);

		delete[] attribute_array;
		delete[] index_array;
	}

	// 调用前注意使用shader.Use()
	void draw(Shader shader) {
		if (num_indices > 0) {
			// 绑定VAO
			glBindVertexArray(VAO);

			// 加载材质
			mat_ptr->make_uniform(shader);

			// 绘制
			glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);

			// 解绑VAO
			glBindVertexArray(0);
		}
		else {
			cout << "mesh::draw(): mesh data is not buffered\n";
		}
	}
};
