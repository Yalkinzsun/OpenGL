# Индексирование VBO (основано на задании 8)

### Принцип индексации

Принцип индексации позволяет многократно использовать одну и ту же вершину. Это возможно благодаря **индексному буферу**

![](https://github.com/Yalkinzsun/OpenGL/blob/master/%D0%97%D0%B0%D0%B4%D0%B0%D0%BD%D0%B8%D1%8F/9.%20%D0%9F%D1%80%D0%B8%D0%BD%D1%86%D0%B8%D0%BF%20%D0%B8%D0%BD%D0%B4%D0%B5%D0%BA%D1%81%D0%B0%D1%86%D0%B8%D0%B8/indexing.png)

Буфер индекса содержит целые числа, по три на каждый треугольник в сетке, которые ссылаются на различные *атрибутные буферы* (положение, цвет, координаты UV, другие координаты UV, нормаль и т. д.). Это означает, что для разделения вершины между двумя треугольниками все атрибуты должны быть одинаковыми.

**Использование индексации** Сначала нужно создать дополнительный буфер, который заполняется индексами. Код такой же, как и раньше (задание 8), но теперь это ELEMENT_ARRAY_BUFFER, а не ARRAY_BUFFER.
```c++
std::vector <unsigned int> indices;

// Generate a buffer for the indices
GLuint elementbuffer;
glGenBuffers(1, &elementbuffer);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
```

чтобы нарисовать сетку, была проведена замена glDrawArrays на это:
```c++
// Index buffer
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

 // Draw the triangles !

 glDrawElements(
    GL_TRIANGLES,      // mode
    indices.size(),    // count
    GL_UNSIGNED_INT,   // type
    (void*)0           // element array buffer offset
 );
```
### Заполнение индексного буфера

Алгоритм заключается в следующем:
```c++
For each input vertex
    Try to find a similar ( = same for all attributes )         vertex between all those we already output
    If found :
       A similar vertex is already in the VBO, use it instead !
       If not found :
       No similar vertex found, add it to the VBO
```
#### Результат выполнения программы:

![](https://github.com/Yalkinzsun/OpenGL/blob/master/screenshots/9.png)

