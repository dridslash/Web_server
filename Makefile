NAME    =	Webserv

SRC    =	

GCC		=	c++ #-Wall -Wextra -Werror -std=c++98

all: $(NAME)
$(NAME):   $(SC_VEC)
	@$(GCC) $(SRC) -o $(NAME)
	@echo "\033[1;35m▐▌ DONE ▐▌"

clean:
	@$(RM)

fclean: clean
	@$(RM) $(NAME) autoindex.html

re:    fclean all

.PHONY: fclean clean re