NAME = webserv

FILES = main.cpp Webserv.cpp Request.cpp

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -MMD -MP

BUILD_DIR = build
SRC_DIR = src

SRC = $(addprefix $(SRC_DIR)/, $(FILES))
OBJ = $(addprefix $(BUILD_DIR)/, $(FILES:.cpp=.o))
DEP = $(OBJ:.o=.d)

TESTER = tests.out
FILE_TEST = $(wildcard $(SRC:.cpp=_test.cpp))
SRC_TEST = $(addprefix $(SRC_DIR)/, $(FILE_TEST))
OBJ_TEST = $(addprefix $(BUILD_DIR)/, $(notdir $(FILE_TEST:.cpp=.o)))

VALGRIND = valgrind -q --error-exitcode=1 \
--leak-check=full --show-leak-kinds=all \
--trace-children=yes --track-origins=yes --track-fds=yes

FORMAT = which clang-format >/dev/null 2>&1 \
&& echo $^ | xargs clang-format -i || true

all: $(NAME)

run: all
	./$(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@$(FORMAT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

clean:
	$(RM) $(BUILD_DIR)/*

fclean: clean
	$(RM) $(NAME)

re: fclean all

leaks: $(NAME)
	$(VALGRIND) ./$(NAME) NORUN

$(TESTER): CXXFLAGS += -Ilib
$(TESTER): $(NAME) $(OBJ_TEST)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_TEST) $(filter-out %main.o, $(OBJ))

tests: $(TESTER)
	@$(VALGRIND) ./$(TESTER)

retests: fclean tests

-include $(DEP)

.PHONY: all clean fclean re leaks tests retests
