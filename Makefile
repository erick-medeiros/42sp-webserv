NAME = webserv

FILES = main.cpp

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -MMD -MP

BUILD_DIR = build
SRC_DIR = src

SRC = $(addprefix $(SRC_DIR)/, $(FILES))
OBJ = $(addprefix $(BUILD_DIR)/, $(FILES:.cpp=.o))
DEP = $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

clean:
	$(RM) $(BUILD_DIR)/*

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re