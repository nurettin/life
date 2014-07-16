all:
	$(CXX) -Wextra -Wall -Wpedantic -O2 -std=c++11 main.cpp -l aa `pkg-config --cflags --libs jsoncpp`
run:
	./a.out
