CXX=g++
DEBUG=-g
OPT=-O0
WARN=-Wall
CPPVERSION=-std=c++17
ifeq ($(OS), Windows_NT)	
	TARGET=skydiver-ai.exe
	REMOVE=del *.o
	SFML=-IC:\SFML\include -LC:\SFML\lib -LC:\mingw64\x86_64-w64-mingw32 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
else
	TARGET=./skydiver-ai
	REMOVE=rm *.o
	SFML=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
endif
CXXFLAGS=$(DEBUG) $(OPT) $(WARN) $(SFML) $(CPPVERSION)
LD=g++
OBJS= main.o game.o tools.o animation.o skydiver.o force.o plane.o Layer.o NeuralNetwork.o Neuron.o
all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(CXXFLAGS)
	@$(REMOVE)
	@./$(TARGET) 2

main.o: ./src/main.cpp ; $(CXX) -c $(CXXFLAGS) ./src/main.cpp -o main.o
game.o: ./src/model/game.cpp ; $(CXX) -c $(CXXFLAGS) ./src/model/game.cpp -o game.o
tools.o: ./src/model/tools.cpp ; $(CXX) -c $(CXXFLAGS) ./src/model/tools.cpp -o tools.o
animation.o: ./src/model/animation.cpp ; $(CXX) -c $(CXXFLAGS) ./src/model/animation.cpp -o animation.o
force.o: ./src/model/force.cpp ; $(CXX) -c $(CXXFLAGS) ./src/model/force.cpp -o force.o
skydiver.o: ./src/model/skydiver.cpp ; $(CXX) -c $(CXXFLAGS) ./src/model/skydiver.cpp -o skydiver.o
plane.o: ./src/model/plane.cpp ; $(CXX) -c $(CXXFLAGS) ./src/model/plane.cpp -o plane.o
Layer.o: ./src/model/mynn/Layer.cpp ; $(CXX) -c $(CXXFLAGS) ./src/model/mynn/Layer.cpp -o Layer.o
NeuralNetwork.o: ./src/model/mynn/NeuralNetwork.cpp ; $(CXX) -c $(CXXFLAGS) ./src/model/mynn/NeuralNetwork.cpp -o NeuralNetwork.o
Neuron.o: ./src/model/mynn/Neuron.cpp ; $(CXX) -c $(CXXFLAGS) ./src/model/mynn/Neuron.cpp -o Neuron.o
