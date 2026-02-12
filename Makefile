NAME		:= webserv

SRC_DIR		:= src
INC_DIR		:= inc
OBJ_DIR		:= obj

SRC			:= 
vpath %.cpp $(SRC_DIR)
SRC			+= main.cpp
vpath %.cpp $(SRC_DIR)/logging
SRC			+= Logging.cpp
vpath %.cpp $(SRC_DIR)/config
SRC			+= Config.cpp
vpath %.cpp $(SRC_DIR)/dsa
SRC			+= Buffer.cpp

OBJ			:= $(SRC:%.cpp=%.o)
OBJ			:= $(addprefix $(OBJ_DIR)/, $(OBJ))

DEP			:= $(OBJ:%.o=%.d)

CXX			:= c++
CXXFLAGS	:=
CXXFLAGS	+= -std=c++98
CXXFLAGS	+= -Wall
CXXFLAGS	+= -Wextra
CXXFLAGS	+= -Werror
	
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

# static analysis
SA_DIR		:= sa
SA_CCMDS	:= $(SA_DIR)/compile.json
SA_REPORTS	:= $(SA_DIR)/reports
SA_HTML		:= $(SA_DIR)/html
SA_ANALYZER	:=
SA_ANALYZER	+= clangsa
SA_ANALYZER	+= clang-tidy
SA_ANALYZER_CONFIG	:=
SA_ANALYZER_CONFIG	+= clang-tidy:take-config-from-directory=true
HTML_OPEN	:= xdg-open
SA_REPORTS_STAMP	:= $(SA_REPORTS)/.done
SA_HTML_STAMP		:= $(SA_HTML)/.done

# doxygen
DOXY_DEPS	:=
DOXY_DEPS	+= $(shell find . -name '*.cpp' -o -name '*.hpp')
DOXY_DIR	:= docs
DOXY_CONFIG	:= Doxyfile
DOXYGEN		:= doxygen
DOXY_TIMESTAMP := $(DOXY_DIR)/.done

# options
DEV			:= 1
ifeq ($(DEV), 1)
	ASAN		:= 1
	UBSAN		:= 1
	CXXFLAGS	+= -pedantic
	CXXFLAGS	+= -Wconversion
	CXXFLAGS	+= -Wno-unused-parameter
	CXXFLAGS	+= -Wno-unused-function
endif

ifdef LOG
	CPPFLAGS	+= -DDEFAULT_LOG_LEVEL="logging::$(LOG)"
endif

ifeq ($(DEBUG), 1)
	CXXFLAGS += -O0
	CXXFLAGS += -g3
	CPPFLAGS += -g3
endif

ifeq ($(ASAN), 1)
	LDFLAGS += -fsanitize=address
	CPPFLAGS += -fsanitize=address
endif

ifeq ($(UBSAN), 1)
	LDFLAGS += -fsanitize=undefined
	CPPFLAGS += -fsanitize=undefined
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

fclean: clean clean_sa
	$(RM) $(NAME)

re: fclean all

# doxygen
doc: $(DOXY_TIMESTAMP)

$(DOXY_TIMESTAMP): $(DOXY_DEPS) $(DOXY_CONFIG)
	$(DOXYGEN) $(DOXY_CONFIG)
	touch $@

read: $(DOXY_TIMESTAMP)
	$(HTML_OPEN) $(DOXY_DIR)/html/index.html

# static analysis
check: fclean $(SA_HTML_STAMP)
	$(HTML_OPEN) $(SA_HTML)/index.html

$(SA_CCMDS):
	$(DIR_DUP)
	CodeChecker log -b 'make -j' -o $@

$(SA_REPORTS_STAMP): $(SA_CCMDS)
	mkdir -p $(SA_REPORTS)
	-CodeChecker analyze \
		--ctu \
		--analyzers $(SA_ANALYZER) \
		--analyzer-config $(SA_ANALYZER_CONFIG) \
		--enable sensitive \
		-o $(SA_REPORTS) \
		$(SA_CCMDS) 
	touch $@

$(SA_HTML_STAMP): $(SA_REPORTS_STAMP)
	mkdir -p $(SA_HTML)
	-CodeChecker parse \
		--export html \
		-o $(SA_HTML) \
		$(SA_REPORTS) 
	touch $@

clean_sa:
	$(RM) $(SA_DIR)

.PHONY: all clean fclean re check clean_sa doc read
