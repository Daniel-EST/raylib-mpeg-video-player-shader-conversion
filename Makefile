build:
	mkdir build || true
	cd build && cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
	cmake --build ./build --target raylib-video-player-shader -j 10 --
	cd -

play:
	./build/raylib-video-player-shader

clean:
	rm -rf ./build
