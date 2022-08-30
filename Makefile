CXX = clang++

numutils: $(wildcard *.cpp)
	$(CXX) -x c++ -std=c++20 -Wall -Wextra -O2 -ffunction-sections -fdata-sections -Wl,--gc-sections -flto -o $@ $^
