#include <stdio.h>
#include <stdlib.h>

// GLEW - библиотека, упрощающая запрос и загрузку расширений OpenGL
#include <GL/glew.h>

// GLFW - библиотека для создания и открытия окон, создания OpenGL контекста и управления вводом
#include <glfw3.h>
GLFWindow* window;

// GLM - библиотека трехмерной математики
#include <glm/glm.hpp>
using namespace glm; //перевод пространства имен в glm (“vec3” вместо “glm::vec3”)


//Подключение шейдеров
#include <common/shader.hpp>

int main( void )
{
	if( !glfwInit() )
	{
		fprintf( stderr, "Ошибка инициализации GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


	window = glfwCreateWindow( 300, 300, "Треугольник", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Невозможно открыть окно!" );
		return -1;
	}
	glfwMakeContextCurrent(window);

    
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Ошибка инициализации GLEW\n");
		return -1;
	}


	// Установка цвета окна
	glClearColor(0.0f, 0.5f, 0.1f, 0.0f);

	// Создание и компиляция GLSL программ из шейдеров
	GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");

    // Массив 3 векторов, которые являются вершинами треугольника
	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

    // Идентификатор буфера вершин
	GLuint vertexbuffer;
    
    // Создание 1 буфера и помещение в переменную vertexbuffer его идентификатора
	glGenBuffers(1, &vertexbuffer);
    
    // Установка только что созданного буфера текущим
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    
    // Передача информации о вершинах в OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	do{

		// Очистка экрана
		glClear( GL_COLOR_BUFFER_BIT );

		// Использование шейдера
		glUseProgram(programID);

        // Указание: первым буфером атрибутов будут вершины
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID, // Атрибут, который необходимо изменить
			3,                  // Размер
			GL_FLOAT,           // тип
			GL_FALSE,           // Указывает, что значения не нормализованы
			0,                  // шаг
			(void*)0            // Смещение массива в буфере
		);

		// Вывод треугольника
		glDrawArrays(GL_TRIANGLES, 0, 3); // Начиная с вершины 0, всего 3 вершины -> один треугольник

		glDisableVertexAttribArray(vertexPosition_modelspaceID);

		// Очистка буферов
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
    // Проверка нажатия клавиши Escape или закрытия окна
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);

	glfwTerminate();

	return 0;
}

