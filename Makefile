#PARTIE A MODIFIER : Liste des fichiers .cpp (et uniquement les .cpp) à compiler
SOURCES=main.cpp physique/cpp/particule.cpp OPENGL/cpp/IndexBuffer.cpp OPENGL/cpp/Renderer.cpp OPENGL/cpp/Shader.cpp OPENGL/cpp/VertexArray.cpp OPENGL/cpp/VertexBuffer.cpp
#FIN DE LA PARTIE A MODIFIER

# Nom du compilateur
CXX=g++
# Arguments pour l'étape d'édition de liens : tous les avertissements
LDFLAGS=-Wall -Wextra -lGL -lGLU -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor 
# Arguments pour l'etape de compilation : tous les avertissements (et même plus) sauf "ignored-attributes" (présence d'un bug dans GCC version 6 et plus)
CPPFLAGS=-Wall -Wextra -std=c++11
# Librairies : none
LDLIBS= -lGL -lGLU -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor

# Liste des fichier objet (*.o), générée automagiquement
OBJETS=$(subst .cpp,.o,$(SOURCES))

all: build

build: $(OBJETS)
	$(CXX) $(LDFLAGS) -o run $(OBJETS) $(LDLIBS)

depend: .depend

.depend: $(SOURCES)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	rm $(OBJETS)
	rm -f *~ .depend

include .depend
