NAME		:= webserv

SRC_DIR		:= src
INC_DIR		:= inc
OBJ_DIR		:= obj

SRC			:= 
vpath %.cpp $(SRC_DIR)
SRC			+= Main.cpp
SRC			+= Signals.cpp
vpath %.cpp $(SRC_DIR)/server
SRC			+= Server.cpp
SRC			+= ServerInit.cpp
SRC			+= ServerRun.cpp
SRC			+= ServerHandlePoll.cpp
SRC			+= ServerHandlePollin.cpp
SRC			+= ServerHandlePollout.cpp
SRC			+= ServerHandlePollErrs.cpp
SRC			+= ServerErrorHandling.cpp
SRC			+= ServerDebug.cpp
SRC			+= ServerWrappers.cpp
vpath %.cpp $(SRC_DIR)/logging
SRC			+= Logging.cpp
vpath %.cpp $(SRC_DIR)/config
SRC			+= ConfigGlobals.cpp
SRC			+= ConfigClass.cpp
SRC			+= Website.cpp
SRC			+= PathInfo.cpp
SRC			+= ParseServer.cpp
SRC			+= Parser.cpp
SRC			+= UnexpectedToken.cpp
SRC			+= Location.cpp
SRC			+= Scanner.cpp
SRC			+= Token.cpp
SRC			+= TokenType.cpp
vpath %.cpp $(SRC_DIR)/reaction
SRC			+= Autoindex.cpp
SRC			+= CGIProcess.cpp
SRC			+= Reaction.cpp
SRC			+= ReactionMethodCGI.cpp
SRC			+= ReactionMethodsNonCGI.cpp
SRC			+= ReactionProcess.cpp
vpath %.cpp $(SRC_DIR)/request
SRC			+= Request.cpp
SRC			+= ParseHeaders.cpp
SRC			+= ParseRequestLine.cpp
vpath %.cpp $(SRC_DIR)/dsa
SRC			+= Buffer.cpp
vpath %.cpp $(SRC_DIR)/http_headers
SRC			+= HttpHeaders.cpp
SRC			+= HttpHeadersOutput.cpp
vpath %.cpp $(SRC_DIR)/connection
SRC			+= Connection.cpp
vpath %.cpp $(SRC_DIR)/script
SRC			+= Script.cpp
vpath %.cpp $(SRC_DIR)/time
SRC			+= Time.cpp

OBJ			:= $(SRC:%.cpp=%.o)
OBJ			:= $(addprefix $(OBJ_DIR)/, $(OBJ))

DEP			:= $(OBJ:%.o=%.d)

CXX			:= c++
CXXFLAGS	:=
CXXFLAGS	+= -std=c++98
CXXFLAGS	+= -Wall
CXXFLAGS	+= -Wextra
CXXFLAGS	+= -Werror
CPPFLAGS 	+= -fstandalone-debug
CPPFLAGS	:=
CPPFLAGS	+= -MMD
CPPFLAGS	+= -MP
CPPFLAGS	+= $(addprefix -I, $(INC_DIR))

LD			:= c++
LDFLAGS		:= -std=c++98

RM			:= rm -rf

DIR_DUP		= mkdir -p $(@D)

MAKEFLAGS	:=
MAKEFLAGS	+= --no-print-directory

# options
ifeq ($(DEV), 1)
	ASAN		:= 1
	UBSAN		:= 1
	DEBUG		:= 1
	CXXFLAGS	+= -pedantic
	CXXFLAGS	+= -Wconversion
	CXXFLAGS	+= -Wshadow
	CXXFLAGS	+= -Wno-unused-parameter
	CXXFLAGS	+= -Wno-unused-function
endif

ifdef LOGLVL
	CPPFLAGS	+= -DLOG_LEVEL="logging::$(LOGLVL)"
endif

ifeq ($(LOGCOLOR), 0)
	CPPFLAGS	+= -DLOG_COLORED="false"
endif

ifeq ($(DEBUG), 1)
	CXXFLAGS += -O0
	CXXFLAGS += -g3
	CPPFLAGS += -O0
	CPPFLAGS += -g3
	
	CXXFLAGS	+= -Wno-unused-parameter
	CXXFLAGS	+= -Wno-unused-function
endif

ifeq ($(ASAN), 1)
	LDFLAGS += -fsanitize=address
	CPPFLAGS += -fsanitize=address
	CPPFLAGS += -g3
endif

ifeq ($(UBSAN), 1)
	LDFLAGS += -fsanitize=undefined
	CPPFLAGS += -fsanitize=undefined
	CPPFLAGS += -g3
endif

# rules

all: $(NAME)

$(NAME): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

$(OBJ_DIR)/%.o: %.cpp
	$(DIR_DUP)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

-include $(DEP)

clean:
	$(RM) $(DEP)
	$(RM) $(OBJ)
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
