.PHONY: build

build:
	mkdir -p build || true
	cd build && cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build ./build --target raylib-video-player-shader -j 10 --
	cd -

play:
	./build/raylib-video-player-shader

clean:
	rm -rf ./build
