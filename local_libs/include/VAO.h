#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"
#include "EBO.h"

class VAO {
public:
	GLuint ID;
	VAO();

	void LinkVBO(VBO VBO, GLuint layout, int size = 3, int stride = 0, int offset = 0);
	void Bind();
	void Unbind();
	void Delete();
};

#endif // !VAO_CLASS_H
