NAME = webserv

FLAGS = -Wall -Wextra -Werror -g -std=c++98

SRCS = main.cpp Server.cpp Utils.cpp\
 	get_next_line/get_next_line.cpp get_next_line/get_next_line_utils.cpp \
 	Web.cpp Conf.cpp CGI.cpp Position.cpp Response.cpp


OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	clang++ $(FLAGS) $(OBJS) -I./ -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re:	fclean $(NAME)

%.o: %.cpp
	clang++ $(FLAGS) -c $< -o $@

.PHONY: all clean fclean re
