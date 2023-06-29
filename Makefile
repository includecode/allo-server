CXX      := -g++
CXXFLAGS := -Werror -pedantic-errors -Wall -Wextra -Werror
LIBS     := -lpthread
BUILD    := ./build
OBJ_DIR  := $(BUILD)/obj
TARGET   := app
INCLUDE  := -Iinc/
SRC      := $(wildcard src/*.cpp) \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(BUILD)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(BUILD)/$(TARGET) $^ $(LIBS)

.PHONY: clean

build:
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rvf $(OBJ_DIR)/*

info:
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"