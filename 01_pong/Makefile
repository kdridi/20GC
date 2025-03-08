CXX						=	clang++ -std=c++20
CPPFLAGS				=	 

SRC_DIR					=	src
BUILD_DIR				=	build
TEST_DIR				=	tests

SRC_FILES				=	$(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES				=	$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
TEST_FILES				=	$(patsubst $(SRC_DIR)/%.cpp, $(TEST_DIR)/%_tests.cpp, $(SRC_FILES))

NAME					=	01_pong
TARGET					=	$(BUILD_DIR)/$(NAME).a

all						:	$(TARGET)

$(BUILD_DIR)			:
							mkdir -p $@

$(TARGET)				:	$(OBJ_FILES)
							$(AR) rcs $@ $^

clean					:
							$(RM) -rf $(OBJ_FILES)

fclean					:	clean
							$(RM) -rf $(TARGET)
							$(RM) -rf $(BUILD_DIR)

re						:	fclean all

watch					:
							inotifywait -r -m -e modify,create,delete src $(TEST_FILES) | while read path action file; do reset; make; done

.PHONY					:	all clean fclean re

$(BUILD_DIR)/%.o		:	$(SRC_DIR)/%.cpp $(SRC_DIR)/%.h $(TEST_DIR)/%_tests.o | $(BUILD_DIR)
							$(CXX) $(CPPFLAGS) -c $< -o $@

$(TEST_DIR)/%_tests.o	:	$(TEST_DIR)/%_tests.cpp $(SRC_DIR)/%.h | $(BUILD_DIR)
							$(CXX) $(CPPFLAGS) -I$(SRC_DIR) -c $< -o $@
