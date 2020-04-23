CXX = g++
CXXFLAGS = -O2 -Wall -std=c++17 -Iinclude/
LDLIBS = -lpthread

SRC = $(wildcard src/modules/*.cpp) $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)
OUT = shagbar

$(OUT): $(OBJ)
	$(CXX) $(LDLIBS) $(OBJ) -o $@

debug: CXXFLAGS += -g -O0
debug: $(OUT)

install: $(OUT)
	cp $(OUT) $(DESTDIR)/usr/bin/

clean:
	rm -f $(OBJ) $(OUT)

.PHONY: clean install debug
