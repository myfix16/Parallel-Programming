CXX:=g++
MPICXX:=mpic++
CXXFLAGS:=-std=c++14
DIR_GUARD:= mkdir -p build

all: build/gen build/ssort build/psort build/check

build/gen: test_data_generator.cpp
	$(DIR_GUARD)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/ssort: odd_even_sequential_sort.cpp
	$(DIR_GUARD)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/psort: odd_even_parallel_sort.cpp
	$(DIR_GUARD)
	$(MPICXX) $(CXXFLAGS) -o $@ $^

build/check: check_sorted.cpp
	$(DIR_GUARD)
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean
clean:
	-rm -rf build