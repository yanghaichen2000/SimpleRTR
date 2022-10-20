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
	int num_indices = 0; // ����������ͬʱ�������ڼ�������Ƿ�buffer��

public:
	mesh(const objl::Mesh &mesh_data_init, shared_ptr<material> mat_ptr_init) {
		mesh_data = mesh_data_init;
		mat_ptr = mat_ptr_init;
	}

	void buffer_data() {
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
		int	num_vertex = mesh_data.Vertices.size();

		// Ϊ������Ϣ����ռ�
		float* attribute_array = new float[num_vertex * 8];

		// ��������Ϣ����attribute_array
		int index_vertex = 0; // ������
		for (const objl::Vertex& vertex : mesh_data.Vertices) {
			attribute_array[index_vertex * 8 + 0] = -vertex.Position.Z; // �����λ�ý�����һЩ�任
			attribute_array[index_vertex * 8 + 1] = vertex.Position.Y;
			attribute_array[index_vertex * 8 + 2] = vertex.Position.X;
			attribute_array[index_vertex * 8 + 3] = -vertex.Normal.Z; // �����λ�ý�����һЩ�任
			attribute_array[index_vertex * 8 + 4] = vertex.Normal.Y;
			attribute_array[index_vertex * 8 + 5] = vertex.Normal.X;
			attribute_array[index_vertex * 8 + 6] = vertex.TextureCoordinate.X;
			attribute_array[index_vertex * 8 + 7] = vertex.TextureCoordinate.Y;
			index_vertex++;
		}

		// ���������������������θ�����������
		num_indices = mesh_data.Indices.size();;

		// Ϊ����������Ϣ����ռ�
		GLuint* index_array = new GLuint[num_indices];

		// ������������Ϣ����index_array
		int index_triangle = 0;
		int num_triangles_in_this_mesh = mesh_data.Indices.size() / 3;
		for (int i = 0; i < num_triangles_in_this_mesh; i++) {
			index_array[index_triangle * 3] = mesh_data.Indices[i * 3];
			index_array[index_triangle * 3 + 1] = mesh_data.Indices[i * 3 + 1];
			index_array[index_triangle * 3 + 2] = mesh_data.Indices[i * 3 + 2];
			index_triangle++;
		}
		
		// ����������VAO
		glGenVertexArrays(1, &VAO);
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
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, index_array, GL_STATIC_DRAW);

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
	}

	// ����ǰע��ʹ��shader.Use()
	void draw(Shader shader) {
		if (num_indices > 0) {
			// ��VAO
			glBindVertexArray(VAO);

			// ���ز���
			mat_ptr->make_uniform(shader);

			// ����
			glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);

			// ���VAO
			glBindVertexArray(0);
		}
		else {
			cout << "mesh::draw(): mesh data is not buffered\n";
		}
	}
};
