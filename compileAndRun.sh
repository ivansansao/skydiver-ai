#!sh
# mkdir build
# cd build
# cmake --build .
# make
# ./Skydiver-ai
# cd ..

# g++ src/main.cpp src/model/game.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio && ./a.out

pwd
rm -f skydiver-ai > /dev/null
rm -f *.o > /dev/null
# cd src
make
# cd ..
rm -f *.o > /dev/null

