compile:
	g++ -o main.out main.cpp shader_utils.cpp gl_utils.cpp camera.cpp vector3D.cpp world.cpp sphere.cpp triangle.cpp quadric.cpp renderengine.cpp color.cpp ray.cpp material.cpp -lIL -lILU -lglut -lGL -lglui -lGLEW -Wno-deprecated-declarations -std=c++11

run: compile
	./main.out

clean:
	rm -f main.out
