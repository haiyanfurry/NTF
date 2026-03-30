CC = gcc
FLAGS = `pkg-config --cflags gtk4`
LIBS = `pkg-config --libs gtk4`

all:
	$(CC) src/App/main.c src/Core/AppCore.c src/Pages/HomePage.c -o FindFriendApp $(FLAGS) $(LIBS)
