.PHONY: all run_aa run_sdl
all:
	$(CXX) -Wextra -Wall -Wpedantic -O2 -std=c++11 main_aa.cpp -l aa `pkg-config --cflags --libs jsoncpp` -o aa.out
	$(CXX) -Wextra -Wall -Wpedantic -O2 -std=c++11 main_sdl.cpp `pkg-config sdl2 jsoncpp --cflags --libs` -o sdl.out
run_aa:
	./aa.out
run_sdl:
	./sdl.out

