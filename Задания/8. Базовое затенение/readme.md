# Basic shading

##### ! В качестве модели используется OBJ file, созданный в программе 3D-моделирования Blender 

Для работы программы необходимы следующие библиотеки:

1. GLEW (для упрощения запроса и загрузки расширений OpenGL)

2. GLFW (для создания и открытия окон, создания OpenGL контекста и управления вводом)

3. GLM (трехмерной математики)

   

Также необходимо подключить шейдер:
```c++
#include <common/shader.hpp>
```

**Шейдер** (англ. *shader* «затеняющий») — компьютерная программа, предназначенная для исполнения процессорами видеокарты (GPU)

Библиотеки должны быть проинициализированы. Например, для начала работы с библиотекой GLFW это можно сделать при помощи вызова **glfwInit()** 

```c++
if (!glfwInit())
    exit(-1);
```

При завершении работы с GLFW, следует вызывать функцию **glfwTerminate()**. Этот вызов закроет все открытые окна и освободит выделенные ресурсы.

Используется OpenGL версии **2.1**:

```c++
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
```

Для создания окна и контекста OpenGL в GLFW служит функция **glfwCreateWindow()**:

```c++
window = glfwCreateWindow( 300, 300, "Первое окно", NULL, NULL);
```

В аргументах данной функции указываются размеры окна в *px* и его заголовок 

Прежде чем использовать команды для рендеринга в созданное 
окно его нужно сделать его текущим контекстом при помощи функции **glfwMakeContextCurrent()**:

```c++
glfwMakeContextCurrent(window);
```

Цвет окна меняется с помощью функции **glClearColor()**:

```c++
glClearColor(0.1f, 0.6f, 0.0f, 0.0f);
```

Первый аргумент функции отвечает за красный цвет, второй за зелёный и третий за синий (RGB-модель: red, green, blue) 

Чтобы увидеть цвет на созданном окне необходимо в цикле **do {...}** Очистить буфер цвета *GL_COLOR_BUFFER_BIT*:
```с++
do{
		glClear( GL_COLOR_BUFFER_BIT );

	...
}

```
#### Нормали треугольника

**Нормаль плоскости** - это вектор длины 1, перпендикулярный этой плоскости.

Нормаль треугольника - это вектор длины 1, перпендикулярный этому треугольнику. Его легко вычислить, взяв перекрестное произведение двух его ребер, и нормализовать: его длина возвращается к 1
```c++
triangle ( v1, v2, v3 )
edge1 = v2-v1
edge2 = v3-v1
triangle.normal = cross(edge1, edge2).normalize()
```

#### Нормали вершин

**Нормаль вершины** - комбинация нормалей окружающих треугольников.

В вершинных шейдерах мы имеем дело с вершинами, а не с треугольниками, поэтому лучше иметь информацию о вершине. И в любом случае, не может быть информации о треугольниках в OpenGL:
```c++
vertex v1, v2, v3, ....
triangle tr1, tr2, tr3 // all share vertex v1
v1.normal = normalize( tr1.normal + tr2.normal + tr3.normal )
```
#### Использование вершинных нормалей 

Нормаль - это атрибут вершины, такой же, как ее положение, ее цвет, ее UV-координаты. Функция **loadOBJ** из Задания 7 уже читает их из файла OBJ:
```c++
GLuint normalbuffer;
glGenBuffers(1, **&**normalbuffer);
glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
glBufferData(GL_ARRAY_BUFFER, normals.size() ***** **sizeof**(glm**::**vec3), **&**normals[0], GL_STATIC_DRAW);
```

а также:
```c++
// 3rd attribute buffer : normals
 glEnableVertexAttribArray(2);
glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
glVertexAttribPointer(
2,                                     // attribute
     3,                                // size
     GL_FLOAT,                         // type
     GL_FALSE,                         // normalized?
     0,                                // stride
     (**void*******)0                  // array buffer offset
 );
```

Чтобы этот код работал, нужно несколько параметров (различных цветов и мощностей) и еще немного кода.

**MaterialDiffuseColor** просто выбирается из текстуры.

**LightColor** и **LightPower** устанавливаются в шейдере через униформу GLSL.

**cosTheta** зависит от n и l. Мы можем выразить их в любом месте, если они одинаковы для обоих. Мы выбираем пространство камеры, потому что легко вычислить положение источника света в этом пространстве:
```c++
// Normal of the computed fragment, in camera space
 vec3 n = normalize( Normal_cameraspace );
// Direction of the light (from the fragment to the light)
 vec3 1 = normalize( LightDirection_cameraspace );
```
с **Normal_cameraspace** и **LightDirection_cameraspac**e, вычисленными в шейдере Vertex и переданными фрагментному шейдеру:
```c++
// Output position of the vertex, in clip space : MVP \ position
gl_Position =  MVP * vec4*(vertexPosition_modelspace,1);

// Position of the vertex, in worldspace : M \ position
Position_worldspace = (Mvec4(vertexPosition_modelspace,1)).xyz;

// Vector that goes from the vertex to the camera, in camera space.
// In camera space, the camera is at the origin (0,0,0).
vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
EyeDirection_cameraspace = vec3 (0,0,0) - vertexPosition_cameraspace;

// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.*
vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

// Normal of the the vertex, in camera space
Normal_cameraspace **=** ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
```
#### Результат выполнения программы:

![](https://github.com/Yalkinzsun/OpenGL/blob/master/screenshots/8.png)

