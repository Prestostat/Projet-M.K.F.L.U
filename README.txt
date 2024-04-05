Projet M.K.F.L.U
L.DAVID, A.BOURDEAUX,M.CODRON

L'objectif du projet est de simuler un fluide. Le point de vu adopté est de faire une simulation reproduisant des comportements similaires à des fluides et non la création d'une simulation exacte d'un fluide réel.

Nous travaillons dans un environnement linux (ubuntu), utilisons OpenGL, Dear ImGui et OpenMP en plus de la bibliothèque standard. Il convient de les installer pour pouvoir compiler et lancer les programmes (voir les commandes en fin de README).

Il n'y a qu'un makefile à utiliser et une application à lancer (nommée run).

Le projet a l'architecture suivante :
Un dossier OPENGL contenant les classes relatives à l'implémentation de fonctions de base pour OpenGl, ainsi que les Shaders utilisés.

Un dossier Physique contenant les fichiers relatifs aux classes Particule et Ensemble et Aux contenant des fonctions auxilières. 

Un fichier main contenant l'initialisation de la simulation, les fonctions ImGui et la fonction main qui lance le programme. Celui ci s'arrête à la fermeture de la fenêtre d'affichage.

Des dossiers contenant une partie des bibliothèques extérieures.

Étapes pour installer les dépendances (glfw et dlew pour OpenGL)


sudo apt-get install libglfw3
sudo apt-get install libglfw3-dev
sudo apt-get install libglew-dev

Installer de quoi utiliser le Makefile

sudo apt-get -y install cmake
sudo apt install libwayland-dev libxkbcommon-dev wayland-protocols extra-cmake-modules
sudo apt install xorg-dev
