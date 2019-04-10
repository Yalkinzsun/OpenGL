## Клавиатура и мышь 

##### Использование клавиатуры и мыши для перемещения камеры

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

Код этого задания будет мало отличаться от предыдущих. Главное отличие 
состоит в том, что теперь мы будем вычислять MVP матрицу не единожды, а в каждом кадре. Код главного цикла:

```c++
do{

    // ...

    // Вычислить MVP-матрицу в зависимости от положения мыши и нажатых клавиш
    computeMatricesFromInputs();
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    // ...
}
```

**computeMatricesFromInputs()** вычисляет Проекционную и Видовую  матрицы в зависимости от текущего ввода. Это та функция, где происходит  основная работа.

**getProjectionMatrix()** просто возвращает вычисленную Проекционную матрицу.

**getViewMatrix()** просто возвращает вычисленную Видовую матрицу.

#### Основной код:

```c++
// позиция
glm::vec3 position = glm::vec3( 0, 0, 5 );
// горизонтальный угол
float horizontalAngle = 3.14f;
// вертикальный угол
float verticalAngle = 0.0f;
// поле обзора
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;
```

**FoV** - это “уровень зума”. 80 = очень широкий угол обзора, сильные  деформации. Значение от 60 и до 45 является стандартным. 20 - это  сильный зум.

В первую очередь мы будем вычислять позицию, горизонтальный и  вертикальный углы, а также FoV опираясь на ввод, после чего вычислим  Видовую и проекционную матрицы.

#### Чтение позиции мыши

```c++
// Получить позицию мыши
int xpos, ypos;
glfwGetMousePos(&xpos, &ypos);
```

однако, важно не забыть о перемещении курсора обратно в центр экрана, чтобы он не выходил за границы окна:

```c++
// Сбросить позицию мыши для следующего кадра
glfwSetMousePos(1024/2, 768/2);
```

Вычисление углов:
```c++
horizontalAngle += mouseSpeed * deltaTime * float(1024/2 - xpos );
verticalAngle   += mouseSpeed * deltaTime * float( 768/2 - ypos );
```

**Вектор в Мировом пространстве, который будет указывать направление взгляда:**

```c++
// Направление
glm::vec3 direction(
    cos(verticalAngle) * sin(horizontalAngle),
    sin(verticalAngle),
    cos(verticalAngle) * cos(horizontalAngle)
);
```

Вектор, указывающий направление вправо от камеры:

```c++
glm::vec3 right = glm::vec3(
    sin(horizontalAngle - 3.14f/2.0f),
    0,
    cos(horizontalAngle - 3.14f/2.0f)
);
```

Вектор, указывающий направление вверх относительно камеры:

```c++
glm::vec3 up = glm::cross( right, direction );
```

#### Позиция:

```c++
// Движение вперед
if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS){
    position += direction * deltaTime * speed;
}
// Движение назад
if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS){
    position -= direction * deltaTime * speed;
}
// Стрэйф вправо
if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS){
    position += right * deltaTime * speed;
}
// Стрэйф влево
if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS){
    position -= right * deltaTime * speed;
}
```

Единственная непонятная вещь в этом коде - это deltaTime. Если мы  просто умножим вектор на скорость, то получим неприятные эффекты:

- Если у вас быстрый компьютер и приложение работает с частотой  кадров 60, то вы будете передвигаться со скоростью 60 юнитов в секунду.
- Если же у вас медленный компьютер и частота кадров = 20, то вы будете передвигаться со скоростью 20 юнитов в секунду.

Таким образом тот, кто имеет быстрый компьютер будет двигаться  быстрее, поэтому мы вводим переменную, в которую заносим время,  прошедшее с последнего кадра. С помощью GLFW оно вычисляется так:

```c++
double currentTime = glfwGetTime();
float deltaTime = float(currentTime - lastTime);
```

#### Вычисление матриц

```c++
/ Проекционная матрица: Поле обзора = FoV, отношение сторон 4 к 3, плоскости отсечения 0.1 и 100 юнитов
ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
// Матрица камеры
ViewMatrix       = glm::lookAt(
    position,           // Позиция камеры
    position+direction, // Направление камеры
    up                  // Вектор "Вверх" камеры
);
```

#### Результат выполнения программы:

![](https://github.com/Yalkinzsun/OpenGL/blob/master/screenshots/6.png)

