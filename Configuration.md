A chaque nouveau TP (donc à faire tout de suite pour le TP1), mettre le fichier source .cpp dans *.../easycppogl/TP_Prog_Graphique_3D/* et les shaders .vs et .fs dans */.../easycppogl/TP_Prog_Graphique_3D/* puis ajouter le nom du fichier .cpp (sans l'extension) dans la liste *tp_list* du fichier *CMakeLists.txt* qui se trouve dans */TP_Prog_Graphique_3D/*

set( tp_list

​	tp1

​	tp2

​	...

)

## Compilation linux

#### Dependences

* eigen3
* glfw3
* assimp

Tout est déjà installé sur les PC de l'UFR.

Si besoin sur votre PC perso, installer les dépendances :

`apt install libeigen3-dev libglfw3-dev libassimp-dev`

#### Config et compil

A la racine du répertoire easycppogl :

`mkdir build`

`cd build`

`ccmake ..`

​	- c [Configure]

​	- e [Exit]

​	- changer le SYS_DATA_PATH en indiquant le répertoire *.../easycppogl/data* (sans le / à la fin du chemin) et le TP_SHADERS_PATH en indiquant le répertoire *.../easycppogl/TP_Prog_Graphique_3D/shaders* (sans le / à la fin du chemin)

​	- c [Configure]

​	- e [Exit]

​	- g [Generate]

`make`

#### Exécution

Les binaires se trouvent dans *build/TP_Prog_Graphique_3D/*


## Compilation Windows

Use vckpg (install follow https://vcpkg.io/en/getting-started.html)

Use triplet x64-windows (var env: VCPKG_DEFAULT_TRIPLET=x64-windows)

Use cmake-gui version:

* Generator Visual Studio 15 2017 minimum
* Optional platform: x64
* Specificy toolchain file : C:\VCPKG\scripts\buildsystems\vcpkg.cmake (changee path to yours)

Or by command line
	cmake.exe ..\easycppogl\ -DCMAKE_GENERATOR_PLATFORM=x64 
	-DCMAKE_TOOLCHAIN_FILE="C:\VCPKG\scripts\buildsystems\vcpkg.cmake"

Do not forget to add path of your installation bins `C:\VCPKG\installed\x64-windows\bin` and
`C:\VCPKG\installed\x64-windows\debug\bin` to your **PATH** environment variable