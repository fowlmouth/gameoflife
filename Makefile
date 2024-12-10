BIN := gol

SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)

CXXFLAGS := -std=c++20 -Wall -Wextra -Werror -pedantic
LDFLAGS := -ltermbox

ALL: $(BIN)
.PHONY: clean cleanall debug release

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

debug: CXXFLAGS += -g -DDEBUG
debug: $(BIN)

release: CXXFLAGS += -O2 -DNDEBUG -DRELEASE
release: $(BIN)

clean:
	$(RM) $(OBJ)

cleanall: clean
	$(RM) $(BIN)

