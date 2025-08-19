NAME = webserv

SRC = ./src/main.cpp \
	  ./src/utils.cpp \
	  ./src/server/Socket.cpp \
	  ./src/server/Client.cpp \
	  ./src/server/Config.cpp \
	  ./src/server/Config_subclass/Autoindex.cpp \
	  ./src/server/Config_subclass/Cgi.cpp \
	  ./src/server/Config_subclass/CgiBin.cpp \
	  ./src/server/Config_subclass/ClientMaxBodySize.cpp \
	  ./src/server/Config_subclass/Directive.cpp \
	  ./src/server/Config_subclass/DirectoryListing.cpp \
	  ./src/server/Config_subclass/ErrorCodePage.cpp \
	  ./src/server/Config_subclass/Index.cpp \
	  ./src/server/Config_subclass/Listen.cpp \
	  ./src/server/Config_subclass/Location.cpp \
	  ./src/server/Config_subclass/Methods.cpp \
	  ./src/server/Config_subclass/Redirect.cpp \
	  ./src/server/Config_subclass/Root.cpp \
	  ./src/server/Config_subclass/ServConfig.cpp \
	  ./src/server/Config_subclass/ServerName.cpp \
	  ./src/server/Config_subclass/Upload.cpp \
	  ./src/server/Response.cpp \
	  ./src/server/SessionClient.cpp \
	  ./src/server/Upload.cpp \
	  ./src/server/Request.cpp \
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
