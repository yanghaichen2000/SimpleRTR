#pragma once
#include "global.h"
#include "OBJ_loader.h"
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class mesh_triangle {
public:
	// OBJLoader�Ķ�ȡ���
	vector<objl::Mesh> mesh_list;

public:
	mesh_triangle(const char file_name[]) {

		objl::Loader loader;
		loader.LoadFile(file_name);

		// ��ȡmesh�б�
		// һ��mesh��Ӧblender��һ���������������а���һϵ�ж����������Ϣ
		mesh_list = loader.LoadedMeshes;
	}

	// ���ض�������
	int buffer_data(GLuint VAO) {

		// ���ݽṹ��
		// triangle1 {
		// position1.x position1.y position1.z normal1.x normal1.y normal1.z uv1.x uv1.y
		// position2.x position2.y position2.z normal2.x normal2.y normal2.z uv2.x uv2.y
		// position3.x position3.y position3.z normal3.x normal3.y normal3.z uv3.x uv3.y
		// }
		// triangle2 {
		// ...
		// }
		// ...

		// shader���룺
		// layout(location = 0) in vec3 position;
		// layout(location = 1) in vec3 normal;
		// layout(location = 2) in vec2 uv;


		// ���㶥�����
		int num_vertex = 0;
		for (objl::Mesh mesh : mesh_list) {
			num_vertex += mesh.Vertices.size();
		}

		// Ϊ������Ϣ����ռ�
		float* attribute_array = new float[num_vertex * 8];

		// ��������Ϣ����attribute_array
		int vertex_index = 0; // ������
		for (objl::Mesh mesh : mesh_list) {
			for (const objl::Vertex &vertex : mesh.Vertices) {
				attribute_array[vertex_index * 8 + 0] = -vertex.Position.Z; // �����λ�ý�����һЩ�任
				attribute_array[vertex_index * 8 + 1] = vertex.Position.Y;
				attribute_array[vertex_index * 8 + 2] = vertex.Position.X;
				attribute_array[vertex_index * 8 + 3] = -vertex.Normal.Z; // �����λ�ý�����һЩ�任
				attribute_array[vertex_index * 8 + 4] = vertex.Normal.Y;
				attribute_array[vertex_index * 8 + 5] = vertex.Normal.X;
				attribute_array[vertex_index * 8 + 6] = vertex.TextureCoordinate.X;
				attribute_array[vertex_index * 8 + 7] = vertex.TextureCoordinate.Y;
				vertex_index++;
			}
		}

		// ���������θ���
		int num_triangle = 0;
		for (objl::Mesh mesh : mesh_list) {
			num_triangle += mesh.Indices.size() / 3;
		}

		// Ϊ����������Ϣ����ռ�
		GLuint* index_array = new GLuint[num_triangle * 3];

		// ������������Ϣ����index_array
		int index_bias = 0; // ƫ��������Ҫʹ��������ΪOBJloader��ÿ��mesh�Ķ��������������
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

		// ����VAO
		glBindVertexArray(VAO);
		{
			// ������buffer VBO
			GLuint VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 8, attribute_array, GL_STATIC_DRAW);

			// ������buffer EBO
			GLuint EBO;
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_triangle * 3, index_array, GL_STATIC_DRAW);
		
			// ���ö�������ָ��
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