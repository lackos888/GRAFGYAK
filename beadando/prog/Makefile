all:
	gcc -Iinclude/ src/model.c src/draw.c src/callbacks.c src/camera.c src/init.c src/main.c src/scene.c src/texture.c src/terrain.c src/utils.c -lSOIL -lopengl32 -lglu32 -lglut32 -lm -o cube.exe -Wall -Wextra -Wpedantic

linux:
	gcc -Iinclude/ src/model.c src/draw.c src/callbacks.c src/camera.c src/init.c src/main.c src/scene.c src/texture.c src/terrain.c src/utils.c -lSOIL -lGL -lGLU -lglut -lm -o cube -Wall -Wextra -Wpedantic
