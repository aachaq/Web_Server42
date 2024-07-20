NAME = webserv

CPP = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS =  ConfigFile/CfgLoaction.cpp \
		ConfigFile/CfgServer.cpp \
		ConfigFile/ConfigFile.cpp \
		ConfigFile/GlobGet.cpp \
		ConfigFile/Utiles.cpp \
		Server/Cgi.cpp \
		Server/Client.cpp \
		Server/ClientData.cpp \
		Server/Http.cpp \
		Server/Request.cpp \
		Server/Response.cpp \
		Server/send.cpp \
		Server/Server.cpp




SRCS += main.cpp

OBJS = $(SRCS:.cpp=.o)

RM = rm -fr

all : $(NAME)


$(NAME) :$(OBJS)
	mkdir -p InternalUse
	@$(CPP) $(CPPFLAGS)  $(OBJS) -o $(NAME) 

clean :
	@$(RM) $(OBJS)

fclean : clean
	@$(RM) $(NAME)

re : fclean all


.PHONY:all clean fclean re test