#pragma once
#include "global.h"
#include "OBJ_loader.h"
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class mesh_triangle {
public:
	// OBJLoader的读取结果
	vector<objl::Mesh> mesh_list;

public:
	mesh_triangle(const char file_name[]) {

		objl::Loader loader;
		loader.LoadFile(file_name);

		// 获取mesh列表
		// 一个mesh对应blender中一个独立的网格，其中包含一系列顶点和索引信息
		mesh_list = loader.LoadedMeshes;
	}

	// 返回顶点数量
	int buffer_data(GLuint VAO) {

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
		int num_vertex = 0;
		for (objl::Mesh mesh : mesh_list) {
			num_vertex += mesh.Vertices.size();
		}

		// 为属性信息分配空间
		float* attribute_array = new float[num_vertex * 8];

		// 将属性信息放入attribute_array
		int vertex_index = 0; // 顶点编号
		for (objl::Mesh mesh : mesh_list) {
			for (const objl::Vertex &vertex : mesh.Vertices) {
				attribute_array[vertex_index * 8 + 0] = -vertex.Position.Z; // 这里的位置进行了一些变换
				attribute_array[vertex_index * 8 + 1] = vertex.Position.Y;
				attribute_array[vertex_index * 8 + 2] = vertex.Position.X;
				attribute_array[vertex_index * 8 + 3] = -vertex.Normal.Z; // 这里的位置进行了一些变换
				attribute_array[vertex_index * 8 + 4] = vertex.Normal.Y;
				attribute_array[vertex_index * 8 + 5] = vertex.Normal.X;
				attribute_array[vertex_index * 8 + 6] = vertex.TextureCoordinate.X;
				attribute_array[vertex_index * 8 + 7] = vertex.TextureCoordinate.Y;
				vertex_index++;
			}
		}

		// 计算三角形个数
		int num_triangle = 0;
		for (objl::Mesh mesh : mesh_list) {
			num_triangle += mesh.Indices.size() / 3;
		}

		// 为顶点索引信息分配空间
		GLuint* index_array = new GLuint[num_triangle * 3];

		// 将顶点索引信息放入index_array
		int index_bias = 0; // 偏移量，需要使用它是因为OBJloader将每个mesh的顶点索引单独编号
		int index_triangle = 0;
		for (objl::Mesh mesh : mesh_list) {
			int num_triangle_in_this_mesh = mesh.Indices.size() / 3;
			for (int i = 0; i < num_triangle_in_this_mesh; i++) {
				index_array[index_triangle * 3] = mesh.Indices[i * 3] + index_bias;
				index_array[index_triangle * 3 + 1] = mesh.Indices[i * 3 + 1] + index_bias;
				index_array[index_triangle * 3 + 2] = mesh.Indices[i * 3 + 2] + index_bias;
				index_triangle++;
			}
			index_bias += mesh.Vertices.size();
		}

		// 配置VAO
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
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_triangle * 3, index_array, GL_STATIC_DRAW);
		
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

		return num_triangle * 3;
	}
};