CXX = g++
CXXFLAGS = -std=c++17 -Wall
SRC_DIR = src
OBJ_DIR = obj
DEP_DIR = dependencies

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

.PHONY: all deps clean

all: sqlDatabase

deps:

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

sqlDatabase: deps $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

clean:
	rm -rf $(OBJ_DIR) sqlDatabase