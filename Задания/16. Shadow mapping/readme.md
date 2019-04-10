# Отображение теней

Основной алгоритм карты теней состоит из двух проходов. Во-первых, сцена визуализируется с точки зрения света. Только глубина каждого фрагмента вычисляется. Затем сцена визуализируется как обычно, но с дополнительным тестом, чтобы увидеть ее, текущий фрагмент находится в тени.

В этом задании рассматриваются только направленные источники света - источники света, которые расположены так далеко, что все световые лучи можно считать параллельными. Таким образом, рендеринг карты теней выполняется с помощью матрицы ортографической проекции. Ортогональная матрица похожа на обычную матрицу проекции перспективы, за исключением того, что перспектива не принимается во внимание - объект будет выглядеть одинаково, как далеко, так и рядом с камерой.

Используется 16-битная текстура глубины 1024x1024 для хранения карты теней. 16 бит обычно достаточно для карты теней. 
```c++
// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
 GLuint FramebufferName = 0;
 glGenFramebuffers(1, &FramebufferName);
 glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
 GLuint depthTexture;
 glGenTextures(1, &depthTexture);
 glBindTexture(GL_TEXTURE_2D, depthTexture);
 glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

 glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

 glDrawBuffer(GL_NONE); // No color buffer is drawn to.

// Always check that our framebuffer is ok
 if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
 return false;
```

Матрица MVP, используемая для визуализации сцены с точки зрения источника света, рассчитывается следующим образом:

- Матрица проекции - это орфографическая матрица, которая будет охватывать все, что находится в выровненных по оси осях (-10,10), (- 10,10), (- 10,20) по осям X, Y и Z соответственно. 
- Матрица просмотра вращает мир так, что в пространстве камеры направление света равно -Z 
```c++
glm::vec3 lightInvDir = glm::vec3(0.5f,2,2);

 // Compute the MVP matrix from the light's point of view
 glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
 glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
 glm::mat4 depthModelMatrix = glm::mat4(1.0);
 glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

 // Send our transformation to the currently bound shader,
 // in the "MVP" uniform
 glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0])
```

**Вершинный шейдер** - это сквозной шейдер, который просто вычисляет положение вершины в однородных координатах:

#version 330 core
```c++
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;

void main(){
 gl_Position =  depthMVP * vec4(vertexPosition_modelspace,1);
}
```
***Фрагментный шейдер** так же прост: он просто записывает глубину фрагмента в позиции 0
```c++
#version 330 core

// Ouput data
layout(location = 0) out float fragmentdepth;

void main(){
    // Not really needed, OpenGL does it anyway
    fragmentdepth = gl_FragCoord.z;
}
```
Вернемся к  обычному шейдеру. Для каждого фрагмента, который мы вычисляем, мы должны проверить, находится ли он «за» картой теней или нет.

Для этого нам нужно вычислить позицию текущего фрагмента в том же пространстве, которое мы использовали при создании карты теней . Поэтому нам нужно преобразовать его один раз с обычной матрицей MVP, а другой раз - с матрицей глубины MVP.

Однако есть небольшая хитрость. Умножение позиции вершины на глубину MVP даст однородные координаты, которые находятся в [-1,1]; но выборка текстуры должна быть сделана в [0,1].

Например, фрагмент в середине экрана будет в (0,0) в однородных координатах; но так как он должен будет отобрать середину текстуры, UVs должны быть (0,5, 0,5).

Это можно исправить, настроив координаты выборки непосредственно в фрагментном шейдере, но более эффективно умножить однородные координаты на следующую матрицу, которая просто делит координаты на 2 (диагональ: [-1,1] -> [-0,5, 0,5]) и переводит их (нижний ряд: [-0,5, 0,5] -> [0,1]).
```c++
glm::mat4 biasMatrix(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0
);
glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
```
При рендеринге возникнут проблемы, называемые shadow acne (теневые прыщи):

![](https://github.com/Yalkinzsun/OpenGL/blob/master/%D0%97%D0%B0%D0%B4%D0%B0%D0%BD%D0%B8%D1%8F/16.%20Shadow%20mapping/ShadowAcne.png)

Обычное «исправление» для этого заключается в добавлении поля ошибки: мы только затеняем, если глубина текущего фрагмента (опять же, в световом пространстве) действительно далека от значения карты освещения. Мы делаем это путем добавления смещения:

```c++
float bias = 0.005;
float visibility = 1.0;
if ( texture( shadowMap, ShadowCoord.xy ).z  <  ShadowCoord.z-bias){
    visibility = 0.5;
}
```
#### Результат выполнения программы:

![](https://github.com/Yalkinzsun/OpenGL/blob/master/screenshots/16.png)

