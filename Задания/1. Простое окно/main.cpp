#include <stdio.h>
#include <stdlib.h>

// GLEW - библиотека, упрощающая запрос и загрузку расширений OpenGL
#include <GL/glew.h>

// GLFW - библиотека для создания и открытия окон, создания OpenGL контекста и управления вводом
#include <glfw3.h>
GLFWwindow* window;

// GLM - библиотека трехмерной математики
#include <glm/glm.hpp>
using namespace glm; //перевод пространства имен в glm (“vec3” вместо “glm::vec3”)


int main( void )
{
	// Инициализация GLFW
	if( !glfwInit() )
	{
        fprintf( stderr, "Ошибка при инициализации GLFW :(" );
		return -1;
	}
 
    
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); // Использование OpenGL 2.1
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


	// Установка размеров окна, заголовка окна
	window = glfwCreateWindow( 300, 300, "Первое окно", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Невозможно открыть окно GLFW :(" );
		return -1;
	}
    
	glfwMakeContextCurrent(window);

	// Инициализация GLEW
	if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Ошибка при инициализации GLFW :(");
		return -1;
	}

	// Установка цвета окна
	glClearColor(0.1f, 0.6f, 0.0f, 0.0f);

	do{
		// Очистка экрана
		glClear( GL_COLOR_BUFFER_BIT );

		// Сбрасывание буферов
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
    
    // Проверка нажатия клавиши Escape или закрытия окна
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	return 0;
}

