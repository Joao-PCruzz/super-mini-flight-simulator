simulador_linux: src/main.cpp
	g++ src/main.cpp -o bin/simulador -lglfw -lGL -lGLU

simulador_windows: src/main.cpp
	g++ src/main.cpp -o bin/simulador -lglfw3 -lopengl32 -lglu32 -lgdi32