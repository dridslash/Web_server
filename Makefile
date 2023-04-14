NAME    =	Webserv

SRC    =	*.cpp Response/*.cpp Config/*.cpp

GCC		=	c++ -Wall -Wextra -Werror -std=c++98

all: $(NAME)
$(NAME):	$(SRC)
	@$(GCC) $(SRC) -o $(NAME)
	@echo "\033[1;35m▐▌ DONE ▐▌"

run: re
	@ ./$(NAME) Config/ConfigFile.conf

launch: all
	@ ./$(NAME) Config/ConfigFile.conf

clean:
	@$(RM)

fclean: clean
	@$(RM) $(NAME) autoindex.html

re:   fclean all

.PHONY: fclean clean re