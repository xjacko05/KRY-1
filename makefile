SOURCE = kry
CC = g++
all: $(SOURCE).cpp
	@$(CC) -std=c++17 -Wall -Wextra -pedantic -o $(SOURCE) $(SOURCE).cpp
clean: $(SOURCE)
	@rm $(SOURCE)
zip:
	zip 222092.zip kry.cpp makefile doc.pdf