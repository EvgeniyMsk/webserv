NAME = webserv
 CFLAGS = -Wall -Wextra -Werror -std=c++98
 FILES = Utils Request Config main
 FILES = main Client HTTP Server Utils Webserv
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