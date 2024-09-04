# Skydiver - Artificial Intelligence

Skydiver learning how to land on Boat!

![Alt Text](src/asset/image/show1.gif)

## Compile on Linux

Here we are using Makefile.

Install
~~~bash
sudo apt update
sudo apt install libsfml-dev -y
sudo apt install make -y
sudo apt install g++ -y
~~~

~~~bash
mkdir ai
cd ai
git clone https://github.com/ivansansao/skydiver-ai.git
chmod +x compileAndRun.sh # Just first time
./compileAndRun.sh
~~~

### Compile on Windows (Using g++)

#### 1 - Download cmake installer for Windows like: cmake-3.26.0-rc5-windows-x86_64.msi

	Just intall it

#### 2 - Download SFML zip for Windows like: SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip

	Link: https://www.sfml-dev.org/download.php

	Put in: C:\SFML (Needed to stay like C:\SFML\bin,C:\SFML\include,C:\SFML\lib)

#### 3 - Download Ming for Windows like: x86_64-7.3.0-release-posix-seh-rt_v5-rev0.7z

	Hey? PAY ATTENTION ;) look for: "Here are links to the specific MinGW compiler versions used to build the provided packages"	
	And choose: MinGW Builds 7.3.0 (64-bit)

	Put in: C:\mingw64 (Needed to stay like C:\mingw64\bin,C:\mingw64\x86_64-w64-mingw32)
	Rename C:\mingw64\bin\mingw32-make.exe to C:\mingw64\bin\make.exe
	If you already have Ming, make sure is most recently g++, if not, delete all and download again!

Close all screen command lines, go to Variable Windows and add it below to PATH of system  
C:\mingw64\bin  
C:\SFML\bin  

#### 4 - Compile

~~~bash
mkdir ai
cd ai
git clone https://github.com/ivansansao/skydiver-ai.git
cd skydiver-ai  
make  
~~~

Done, skydiver-ai.exe has created! I believe!
