NAME = ircserv
SRCS = main.cpp Client.cpp Kqueue.cpp Channel.cpp DataManager.cpp Server.cpp Socket.cpp Executor.cpp
INCS = Client.hpp Kqueue.hpp Channel.hpp DataManager.hpp Server.hpp Socket.hpp Executor.hpp
INC_DIR = -I ./
OBJ_DIR = objs
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
CPP = c++ -std=c++98
CFLAGS = -Wall -Wextra -Werror

all : $(NAME)

$(NAME) : $(OBJS)
	$(CPP) $(CFLAGS) -o $@ $^ $(INC_DIR)

$(OBJ_DIR)/%.o : %.cpp $(INCS)
	@mkdir -p $(OBJ_DIR)
	$(CPP) $(CFLAGS) -c -o $@ $< $(INC_DIR)

clean :
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re
