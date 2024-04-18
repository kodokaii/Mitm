NAME		= mitm

CXX			= c++
RM			= rm -f
CLONE 		= git clone --depth=1

CXXFLAGS	+= -Wall -Wextra -Werror -std=c++98
LDLIBS		=

KDO			= kdolib
KDOLIB		= $(KDO)/kdolib.a

SRC			= Mitm.cpp

OBJ 		= $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(KDOLIB) $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ) $(KDOLIB) $(LDLIBS)

$(KDOLIB): $(KDO)
	$(MAKE) -C $(KDO)

$(KDO):
	$(CLONE) https://github.com/kodokaii/kdolib_cpp.git $(KDO)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	if [ -d "$(KDO)" ]; then $(MAKE) clean -C $(KDO); fi
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(KDOLIB)
	$(RM) $(NAME)

clear: fclean
	$(RM) -r $(KDO)

re: fclean all

fast: fclean
	$(MAKE) -j$$(nproc)

.PHONY:		all clean fclean clear re
