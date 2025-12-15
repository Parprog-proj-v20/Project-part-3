# Project-part-3
Проект по параллельному программированию. Вариант 20, третья часть.

### Запуск из папки проекта через:
```
cmake -B build
cmake --build build
cd build
ctest -C Debug
```
Если нужны тесты по отдельности:
```
cmake -B build
cmake --build build
mpiexec -n 20 .\build\tests\Debug\<имя теста>.exe
```
