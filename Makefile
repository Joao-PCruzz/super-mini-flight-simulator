CXX = g++
CXXFLAGS = -Wall -std=c++11
SRC = src/main.cpp

# Valores definidos para falta de parâmetro da função
ARGS ?= 1.0 0.0 0.0 5.0

# Detecção do sistema operacional
ifeq ($(OS),Windows_NT)
	# configurações para windows
	LIBS = -lglfw3 -lopengl32 -lglu32 -lgdi32
	EXEC = bin/simulador.exe
	MKDIR = if not exist mkdir bin
	RM = rmdir /s /q bin
	RUN = bin\simulador.exe
else
	# Configurações para Linux
	LIBS = -lglfw -lGL -lGLU
	EXEC = bin/simulador
	MKDIR = mkdir -p bin
	RM = rm -rf bin
	RUN = ./$(EXEC)
endif

all:
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXEC) $(LIBS)

run: all
	$(RUN) $(ARGS)

clean:
	$(RM)