#pragma once

// vertices array for test
float triangleVertices[] = {
	-0.5f,	-0.5f,	0.0f,
	 0.5f,		-0.5f,	0.0f,
	 0.0f,		 0.5f,		0.0f
};

float rectVertices[] = {
	0.5f, 0.5f, 0.0f,   // 右上角
	0.5f, -0.5f, 0.0f,  // 右下角
	-0.5f, -0.5f, 0.0f, // 左下角
	-0.5f, 0.5f, 0.0f   // 左上角
};


unsigned int indices[] = {
	0, 1, 3, // 第一个三角形
	1, 2, 3  // 第二个三角形
};
