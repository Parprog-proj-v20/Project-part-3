# Project-part-3
Проект по параллельному программированию. Вариант 20, третья часть.

Для запуска наужно перейти сюда https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi и скачать Microsoft MPI (MS-MPI). Установить обе версии msmpisdk.msi (SDK) и msmpisetup.exe (Runtime)
Далее ПКМ на проекте → Свойства. Там Каталоги VC++ и в поле Каталоги исполняемых файлов пишем C:\Program Files (x86)\Microsoft SDKs\MPI\Include
Дальше Компоновщик → Общие. В поле Дополнительные каталоги библиотек пишем C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64;%(AdditionalLibraryDirectories)
Нажимаем Применить

Я запускала через x64 Native Tools Command Prompt for VS 2022 с помощью следующей последовательности команд:
cd "C:\Users\user\source\repos\Rabbits\Rabbits"
cl /EHsc /I "C:\Program Files (x86)\Microsoft SDKs\MPI\Include" main.cpp Rabbits.cpp /link /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" msmpi.lib /out:rabbits.exe
dir rabbits.exe - на всякий пожарный
mpiexec -n 20 rabbits.exe

Если внесли изменения в проект:
Сборка (левый верхний угол VS) → Пересобрать решение. Потом в x64 Native Tools Command Prompt for VS 2022:
del main.exe 2>nul
del rabbits.exe 2>nul
del *.obj 2>nul

cl /EHsc /I "C:\Program Files (x86)\Microsoft SDKs\MPI\Include" main.cpp Rabbits.cpp /link /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" msmpi.lib /out:rabbits.exe
dir rabbits.exe - на всякий пожарный
mpiexec -n 20 rabbits.exe
