#pragma once

// vertices array for test
float triangleVertices[] = {
	-0.5f,	-0.5f,	0.0f,
	 0.5f,		-0.5f,	0.0f,
	 0.0f,		 0.5f,		0.0f
};

float rectVertices[] = {
	0.5f, 0.5f, 0.0f,   // ���Ͻ�
	0.5f, -0.5f, 0.0f,  // ���½�
	-0.5f, -0.5f, 0.0f, // ���½�
	-0.5f, 0.5f, 0.0f   // ���Ͻ�
};


unsigned int indices[] = {
	0, 1, 3, // ��һ��������
	1, 2, 3  // �ڶ���������
};
