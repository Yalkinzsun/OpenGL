# Billboards

Эти элементы часто используется в компьютерных играх, например, в виде показателя уровня здоровья персонажа. Они расположены в определенном месте, но их ориентация автоматически рассчитывается так, чтобы они всегда были обращены к камере.

Для реализации необходимо выровнять сетку относительно камеры, даже когда камера движется:

![](https://github.com/Yalkinzsun/OpenGL/blob/master/%D0%97%D0%B0%D0%B4%D0%B0%D0%BD%D0%B8%D1%8F/18.%20Billboards/2a.gif)

Каждый угол billboard находится в центральной позиции, смещенной на верхний и правый векторы камеры:

![](https://github.com/Yalkinzsun/OpenGL/blob/master/%D0%97%D0%B0%D0%B4%D0%B0%D0%BD%D0%B8%D1%8F/18.%20Billboards/principle.png)

Известна только центральную позиция билборда в мировом пространстве, поэтому также нужны векторы вверх / вправо камеры.

Вектор вверх камеры равен (0,1,0). Чтобы получить это в мировом пространстве, необходимо умножить это на матрицу, которая идет от пространства камеры до мирового пространства.

Более простой способ выразить ту же математику:
```c++
CameraRight_worldspace = {ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]}
CameraUp_worldspace = {ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]}
```
Вычисление положения последней вершины:
```c++
vec3 vertexPosition_worldspace = particleCenter_wordspace + CameraRight_worldspace * squareVertices.x * BillboardSize.x + CameraUp_worldspace * squareVertices.y * BillboardSize.y;
```

- particleCenter_worldspace является центральной позицией билборда. Это указывается с помощью единого vec3.
- squareVertices - это оригинальная сетка. squareVertices.x равен -0,5 для левых вершин, которые, таким образом, перемещаются влево к камере (из-за * CameraRight_worldspace)
- BillboardSize - это размер билборда в мировых единицах

#### Результат выполнения программы:

![](https://github.com/Yalkinzsun/OpenGL/blob/master/screenshots/18_1.png)

