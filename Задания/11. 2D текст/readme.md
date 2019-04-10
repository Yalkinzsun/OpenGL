# 2D текст

Наложение 2D-текста (таймера) поверх 3D-контента

Будем реализовывать следующий интерфейс(который объявлен в common\text2D.h):

```c++
void initText2D(const char \* texturePath);
void printText2D(const char \* text, int x, int y, int size);
void cleanupText2D();*
```

**initText2D** просто читает текстуру и необходимые для шрифта шейдеры

Текстура, используемая в данной программе для шрифта была сгенерирована с помощью утилиты CBFG

Чтобы начать рисование цифр, необходимо заполнить следующие буферы:
```c++
std::vector<glm::vec2> vertices;
std::vector<glm::vec2> UVs;
```
Для каждого символа рассчитываются координаты четырех вершин, которые будут определять прямоугольник этого символа, но так как с прямоугольниками OpenGL не работает, то добавляем два треугольника:
```c++
for ( unsigned int i=0 ; i<length ; i++ ){
    glm::vec2 vertex_up_left    = glm::vec2( x+i\*size, y+size );
    glm::vec2 vertex_up_right   = glm::vec2( x+i\*size+size, y+size );
    glm::vec2 vertex_down_right = glm::vec2( x+i\*size+size, y);
    glm::vec2 vertex_down_left  = glm::vec2( x+i\*size, y);
    vertices.push_back(vertex_up_left);
    vertices.push_back(vertex_down_left);
    vertices.push_back(vertex_up_right);
    vertices.push_back(vertex_down_right);
    vertices.push_back(vertex_up_right);
    vertices.push_back(vertex_down_left);
```

Теперь рассчитываем UV координаты. Левая верхняя координата каждого символа рассчитывается так:
```c++
char character = text[i];
float uv_x = (character%16)/16.0f;
float uv_y = (character/16)/16.0f;
```

Это работает, так как в таблице ASCII кодов у символа А код 65.

65%16 = 1, поэтому А находится в столбце 1(начало в 0).

65/16 = 4, поэтому А на строчке 4(это у нас целочисленное деление, поэтому тут не будет 4, 0625)

Результирующее значение делится на 16.0 чтобы впихнуть его в интервал [0.0 – 1.0] для UV координат.

А теперь нужно сделать почти то же, но для вершин:
```c++
glm::vec2 uv_up_left    = glm::vec2( uv_x, 1.0f - uv_y );
glm::vec2 uv_up_right   = glm::vec2( uv_x+1.0f/16.0f, 1.0f-uv_y);
glm::vec2 uv_down_right = glm::vec2( uv_x+1.0f/16.0f, 1.0f-(uv_y + 1.0f/16.0f));
glm::vec2 uv_down_left  = glm::vec2( uv_x, 1.0f - (uv_y + 1.0f/16.0f));
UVs.push_back(uv_up_left   );
UVs.push_back(uv_down_left );
UVs.push_back(uv_up_right  );
UVs.push_back(uv_down_right);
UVs.push_back(uv_up_right);
UVs.push_back(uv_down_left);
}
```
Все остальное как всегда, забиндить буфера, заполнить их, выставить шейдер, забиндить текстуру, включить/забиндить/сконфигурировать атрибуты вершин, включить блендинг и вызвать glDrawArrays. И Вуаля! Мы все сделали.



Важное замечание – координаты генерируются в интервале [0,800][0,600]. Другими словами, нам не нужны никакие матрицы тут. Вершинный шейдер должен просто выставить его в интервал [-1,1][-1,1] выполнив простые арифметические действия(но при желании их можно сделать в С++):
```c++
void main(){
// Результирующая позиция в пространстве отсечения
// нужно преобразовать [0..800][0..600] в [-1..1][-1..1]
vec2 vertexPosition_homoneneousspace = vertexPosition_screenspace - vec2(400,300); // [0..800][0..600] -> [-400..400][-300..300]
vertexPosition_homoneneousspace /= vec2(400,300);
gl_Position =  vec4(vertexPosition_homoneneousspace,0,1);
// UV текстурные координаты. Никаких особых преобразований тут не нужно.
UV = vertexUV;
}
```
Фрагментный шейдер тоже не делает ничего выдающегося:
```c++
void main(){
color = texture( myTextureSampler, UV );
}
```
#### *Результат выполнения программы:*

![](https://github.com/Yalkinzsun/OpenGL/blob/master/screenshots/11.png)

