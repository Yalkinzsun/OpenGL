# Normal Mapping

В задании 8, где говорилось про освещение и про то, как можно сделать затенение поверхностей с помощью нормалей треугольника. Была лишь проблема в том, что каждая вершина могла иметь лишь одну нормаль: внутри самого треугольника нормали интерполируются из нормалей трех окружающих вершин, и мы ничего не могли с этим поделать, а вот цветом всегда можно было управлять с помощью текстур. Идея normal mapping в том, чтобы дать нам такой же контроль над нормалями.

### Вычисление касательных и бикасательных

Так как нам для наших расчетов нужны касательные и бикасательные, нам придется посчитать их для всего меша. Давайте вынесем этот расчет в отдельную функцию:
```c++
void computeTangentBasis(
   // входные параметры
   std::vector<glm::vec3> & vertices,
   std::vector<glm::vec2> & uvs,
   std::vector<glm::vec3> & normals,
   // выходные параметры
   std::vector<glm::vec3> & tangents,
   std::vector<glm::vec3> & bitangents
){
```

Для каждой грани нам необходимо подсчитать сторону(deltaPos) и deltaUV:
```c++
for ( int i=0; i<vertices.size(); i+=3){
   // копируем значения в переменные
   glm::vec3 & v0 = vertices[i+0];
   glm::vec3 & v1 = vertices[i+1];
   glm::vec3 & v2 = vertices[i+2];
   // копируем значения в переменные
   glm::vec2 & uv0 = uvs[i+0];
   glm::vec2 & uv1 = uvs[i+1];
   glm::vec2 & uv2 = uvs[i+2];
   // стороны треугольника
   glm::vec3 deltaPos1 = v1-v0;
   glm::vec3 deltaPos2 = v2-v0;
   // дельта UV
   glm::vec2 deltaUV1 = uv1-uv0;
   glm::vec2 deltaUV2 = uv2-uv0;
```

А теперь можно применить выведенную ранее формулу для вычисления касательной и бикасательной:
```c++
float r = 1.0f / (deltaUV1.x \deltaUV2.y - deltaUV1.y \deltaUV2.x);
glm::vec3 tangent = (deltaPos1 \ deltaUV2.y   - deltaPos2 \ deltaUV1.y)\r;
glm::vec3 bitangent = (deltaPos2 \ deltaUV1.x   - deltaPos1\ deltaUV2.x)\r;
```

И наконец, заполняем буферы касательных и бикасательных. Так как эти буферы еще не индексированы, то каждая вершина будет иметь свою собственную копию:
```c++
// Установим одну и ту же касательную для всех вершин треугольника.
// мы их объединим позже в  vboindexer.cpp
tangents.push_back(tangent);
tangents.push_back(tangent);
tangents.push_back(tangent);
// То же самое и для бинормалей
bitangents.push_back(bitangent);
bitangents.push_back(bitangent);
bitangents.push_back(bitangent);
}
```

### Индексация

Индексация этого буфера почти такая же, как и та, которую мы уже делали, но с одним отличием.Если мы нашли одинаковые вершины(одна и та же позиция, одна и та же нормаль и текстурные координаты), нам не нужно применять на неё такую же нормаль, нам нужно усреднить нормали этих вершин. Поэтому возьмем наш старый код и слегка его подправим:
```c++
// Пробуем найти одинаковую вершину
unsigned int index;
bool found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i],     out_vertices, out_uvs, out_normals, index);

if ( found ){ // Одинаковая вершина уже в VBO, используем её !
   out_indices.push_back( index );
   // Усредняем нормаль и бинормаль
   out_tangents[index] += in_tangents[i];

   out_bitangents[index] += in_bitangents[i];
}else{ // Если не нашли, то добавляем
   //  Все как и раньше
   [...]
}
```

### Шейдер

**Дополнительные буферы и константы**

Нам нужно два дополнительных буфера: один для касательных, а второй для бикасательных:
```c++
GLuint tangentbuffer;
glGenBuffers(1, &tangentbuffer);
glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() \* sizeof(glm::vec3),
&indexed_tangents[0], GL_STATIC_DRAW);
GLuint bitangentbuffer;
glGenBuffers(1, &bitangentbuffer);
glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() \* sizeof
glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
```

Так же необходим еще один uniform для текстуры нормали:
```c++
[...]
GLuint NormalTexture = loadTGA_glfw("normal.tga");
[...]
GLuint NormalTextureID  = glGetUniformLocation(programID,
"NormalTextureSampler");
```

А еще один для матрицы ModelView 3*3. Теоретически без последнего можно было бы и обойтись, но с ней легче, но об этом позже. Нам необходима будет лишь верхняя левая часть этой матрицы, так как мы будем делать лишь умножение направлений:
```c++
GLuint ModelView3x3MatrixID = glGetUniformLocation(programID, "MV3x3");
```

Полный код будет выглядеть так:

```c++
// Очищаем экран
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// Включаем шейдер
glUseProgram(programID);
// Вычисляем MVP матрицу
computeMatricesFromInputs();
glm::mat4 ProjectionMatrix = getProjectionMatrix();
glm::mat4 ViewMatrix = getViewMatrix();
glm::mat4 ModelMatrix = glm::mat4(1.0);
glm::mat4 ModelViewMatrix = ViewMatrix \* ModelMatrix;
glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix); // Берем левую верхнюю часть матрицы ModelViewMatrix
glm::mat4 MVP = ProjectionMatrix \* ViewMatrix \* ModelMatrix;
// Отправляем трансформацию в текущий шейдер, в uniform ”**MPV**”
glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[**0][0]);
glm::vec3 lightPos = glm::vec3(0,0,4);
glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
// Биндим диффузную текстуру в слот 0
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
// Устанавливаем "**DiffuseTextureSampler**" сэмплер на слот 0
glUniform1i(DiffuseTextureID, 0);
// Биндим текстуру нормалей на слот 1
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, NormalTexture);
// Устанавливаем "**Normal  TextureSampler**" сэмплер на слот 1
glUniform1i(NormalTextureID, 1);
// первый буферный атрибут - вершины
glEnableVertexAttribArray(0);
glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
glVertexAttribPointer(
   0,                  // атрибут
   3,                  // размер
   GL**_**FLOAT**,           // тип
   GL**_**FALSE**,           // нормализировано ли?
   0,                  // шаг
   (void\*)0            // смещение в буфере
);

// второй атрибут : UV координаты
glEnableVertexAttribArray(1);
glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
glVertexAttribPointer(
   1,                                // атрибут
   2,                                // размер
   GL**_**FLOAT**,                         // тип
   GL**_**FALSE**,                         // нормализировано ли?
   0,                                // шаг
   (void\*)0                          // смещение
);

// третий атрибут - нормали
glEnableVertexAttribArray(2);
glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
glVertexAttribPointer(
   2,                                // атрибут
   3,                                // размер
   GL**_**FLOAT**,                        // тип
   GL**_**FALSE**,                        // нормализировано ли?
   0,                                // шаг
   (void\*)0                         // смещение в буфере
);

// Четвертый атрибут - касательные
glEnableVertexAttribArray(3);
glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
glVertexAttribPointer(
   3,                                // атрибут
   3,                                // размер
   GL**_**FLOAT**,                         // тип
   GL**_**FALSE**,                         // нормализировано ли?
   0,                                // шаг
   (void\*)0                          // смещение
);

// Пятый атрибут - Бикасательные
glEnableVertexAttribArray(4);
glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
glVertexAttribPointer(
   4,                         // атрибут
   3,                         // размер
   GL_FLOAT,                 // тип
   GL_FALSE,                 // нормализировано ли**?
   0,                         
    (void\*)0                 // смещение
);
// Индексный буфер
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

// Рисуем треугольники !
glDrawElements(
   GL_TRIANGLES,      // режим
   indices.size(),    // количество
   GL_UNSIGNED_INT,   // тип
   (void\*)0           // смещение
);
glDisableVertexAttribArray(0);
glDisableVertexAttribArray(1);
glDisableVertexAttribArray(2);
glDisableVertexAttribArray(3);
glDisableVertexAttribArray(4);
// переключаем буферы(показываем на экран)
glfwSwapBuffers();
```

### Вершинный шейдер

Будем делать всё в пространстве камеры, так как тут гораздо проще доставать позиции фрагментов. Именно поэтому мы умножаем наши векторы T,B,N на матрицу ModelView.

```c++
vertexNormal_cameraspace = MV3x3 \* normalize(vertexNormal_modelspace);
vertexTangent_cameraspace = MV3x3 \* normalize(vertexTangent_modelspace);
vertexBitangent_cameraspace = MV3x3 \* normalize
vertexBitangent_modelspace);
```
Эти три вектора определяют TBN матрицу которая конструируется таким образом:
```c++
mat3 TBN = transpose(mat3(
       vertexTangent_cameraspace,
       vertexBitangent_cameraspace,
       vertexNormal_cameraspace
   ));
```
Можно использовать векторное произведение вместо построения этой матрицы и транспонирования её.

Эта матрица поможет вам переходить от пространства камеры в тангенциальное пространство(Такая же матрица, но с XXX_modelspace может помочь в переходе от пространства модели в тангенциальное пространство). Мы можем использовать её для того, чтобы вычислять направление света и взгляда в тангенциальном пространстве:
```c++
LightDirection_tangentspace = TBN \* LightDirection_cameraspace;
EyeDirection_tangentspace =  TBN \* EyeDirection_cameraspace;
```


### Фрагментный шейдер

Наши нормали в тангенциальном пространстве получить очень просто – берем их просто из текстуры:
```c++
// локальная нормаль в тангенциальном пространстве
vec3 TextureNormal_tangentspace = normalize(texture2D(NormalTextureSampler, UV ).rgb\*2.0 - 1.0);
```
#### Результат выполнения программы:

![](https://github.com/Yalkinzsun/OpenGL/blob/master/screenshots/13.png)

