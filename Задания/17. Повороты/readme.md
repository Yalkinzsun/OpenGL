# Вращения

Для вращения используются углы Эйлера или кватернионы
**Углы Эйлера** — углы, описывающие поворот тела в трёхмерном евклидовом пространстве.
```c++
vec3 EulerAngles( RotationAroundXInRadians, RotationAroundYInRadians, RotationAroundZInRadians);
```
**Кватернионы** (от лат. quaterni, по четыре) — система гиперкомплексных чисел, образующая векторное пространство размерностью четыре над полем вещественных чисел.
```c++
// RotationAngle is in radians
x = RotationAxis.x * sin(RotationAngle / 2)
y = RotationAxis.y * sin(RotationAngle / 2)
z = RotationAxis.z * sin(RotationAngle / 2)
w = cos(RotationAngle / 2)
```

В сравнении с углами Эйлера, кватернионы позволяют проще комбинировать вращения, а также избежать проблемы, связанной с невозможностью поворота вокруг оси, независимо от совершённого вращения по другим осям.

Создание кватернионов:
```c++
// Creates an identity quaternion (no rotation)
quat MyQuaternion;

// Direct specification of the 4 components
// You almost never use this directly
MyQuaternion = quat(w,x,y,z); 

// Conversion from Euler angles (in radians) to Quaternion
vec3 EulerAngles(90, 45, 0);
MyQuaternion = quat(EulerAngles);

// Conversion from axis-angle
// In GLM the angle must be in degrees here, so convert it.
MyQuaternion = gtx::quaternion::angleAxis(degrees(RotationAngle), RotationAxis);
```
Преобразование кватерниона в матрицу:
```c++
mat4 RotationMatrix = quaternion::toMat4(quaternion);
```

#### Результат выполнения программы:

![](https://github.com/Yalkinzsun/OpenGL/blob/master/screenshots/17.png)

