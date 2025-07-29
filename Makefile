NAME = webserv

SRC = ./src/main.cpp \
	  ./src/server/tcpServer.cpp

ROOT = ./

SRCS = $(ROOT)$(SRC)
OBJS = $(SRCS:.cpp=.o)

CC = c++

CFLAGS = -Wall -Wextra -Werror -I./includes -I/usr/includes -std=c++98

%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
		rm -f $(OBJS)

fclean: clean
		rm -f $(NAME)

re:	fclean all

.PHONY: all clean re
