NAME = webserv
CFLAGS = -std=c++98
FILES = *
SRCS = $(addprefix srcs/, $(addsuffix .cpp, $(FILES)))
OBJS = $(SRCS:.cpp=.o)
CXX = clang++

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CFLAGS) $(SRCS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: 
	rm $(OBJS)
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re all