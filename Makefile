NAME = webserv

FILES = main.cpp Server.cpp Response.cpp Request.cpp utils.cpp Cookie.cpp
FILES += CGIRequest.cpp Config.cpp Connection.cpp loop.cpp Logger.cpp EventWrapper.cpp

ifeq ($(shell uname -s),Linux)
  FILES += EpollWrapper.cpp
endif
ifeq ($(shell uname -s),Darwin)
  FILES += KqueueWrapper.cpp
endif

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -MMD -MP -g

BUILD_DIR = build
SRC_DIR = src

SRC = $(addprefix $(SRC_DIR)/, $(FILES))
OBJ = $(addprefix $(BUILD_DIR)/, $(FILES:.cpp=.o))
DEP = $(OBJ:.o=.d)

TESTER = tests.out
FILE_TEST = $(wildcard $(SRC:.cpp=_test.cpp))
SRC_TEST = $(addprefix $(SRC_DIR)/, $(FILE_TEST))
OBJ_TEST = $(addprefix $(BUILD_DIR)/, $(notdir $(FILE_TEST:.cpp=.o)))

TESTER_INTEGRATION=tests/integration.sh
TESTER_BAD_CONFIG=tests/run_bad_config.sh

VALGRIND = valgrind -q --error-exitcode=1 \
--leak-check=full --show-leak-kinds=all \
--trace-children=yes --track-origins=yes --track-fds=yes

FORMAT = which clang-format >/dev/null 2>&1 \
&& echo $^ | xargs clang-format -i || true

all: $(NAME)

run: all
	./$(NAME) --log-level=DEBUG

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
	$(RM) $(NAME) *.out

re: fclean all

leaks: $(NAME)
	$(VALGRIND) ./$(NAME) --no-run --log-level=NONE

$(TESTER): CXXFLAGS += -Ilib
$(TESTER): $(NAME) $(OBJ_TEST)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_TEST) $(filter-out %main.o, $(OBJ))

tests: $(TESTER)
	@$(VALGRIND) ./$(TESTER)

# Optional: make tests-integration TEST_NAME=your_test_name
tests-integration: $(NAME)
	sh $(TESTER_BAD_CONFIG)
	sh $(TESTER_INTEGRATION) $(TEST_NAME)

retests: fclean tests

all-tests: $(NAME) leaks tests tests-integration

-include $(DEP)

up:
	docker-compose up --build -d
	docker exec -it webserv bash

down:
	docker-compose down --rmi all --volumes

.PHONY: all clean fclean re leaks tests retests all-tests up down