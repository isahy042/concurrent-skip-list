APP_NAME=skip-list

SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build

SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp $(SRC_DIR)/*.cpp)
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
TEST_OBJ_FILES := $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_FILES))

MAIN_EXEC = $(BUILD_DIR)/$(APP_NAME)
TEST_EXEC = $(BUILD_DIR)/test_suite

CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -I$(SRC_DIR) -g -fsanitize=address

# Default target
all: $(MAIN_EXEC)

# Build main program
$(MAIN_EXEC): $(OBJ_FILES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Build object files for source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build test executable
$(TEST_EXEC): $(TEST_OBJ_FILES) $(OBJ_FILES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Build object files for test files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
test: $(TEST_EXEC)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean up
clean:
	rm -rf $(BUILD_DIR)
