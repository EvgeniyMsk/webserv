
#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# include <stdlib.h>
# include <unistd.h>
# include <new>

typedef struct s_fd
{
	int			fd;
	char		*left;
	char		*left_begin;
	struct s_fd	*next;
}              t_fd;

int					get_next_line(int fd, char **line);
t_fd				*new_fd_to_read(int fd);
char				*ft_strjoin_re(char **s1, char *s2);
char				*ft_strchr(const char *s, int c);
size_t				ft_strlen(const char *str);
char				*ft_strdup(char *s);

#endif
