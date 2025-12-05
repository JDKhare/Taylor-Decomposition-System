#=====================================================================================
#    Description:  General Makefile
#        Created:  04/15/2007 01:27:27 PM EDT
#         Author:  Daniel Gomez-Prado, dgomezpr@ecs.umass.edu
#         		   Qian Ren (R&D Engineer), qren@ren-chao.com
#        Company:  UMASS
#=====================================================================================
#  $Revision:: 209                                          $: Revision of last commit
#  $Author:: daniel@linux                                   $: Author of last commit
#  $Date:: 2012-07-09 05:20:55 -0400 (Mon, 09 Jul 2012)     $: Date of last commit         
#=====================================================================================

TARGET  := tds
NAME    := $(TARGET)

CC      := gcc
CPP     := g++ 
LD      := g++ 

USRFLAGS:= -O6 -Wno-deprecated
GDBFLAGS:= -O0 -g -pg
LDFLAGS := -Wno-deprecated 
GDBLDFLAGS:=-g -pg


#-------------------------------------------------------------------------
# Static Libraries 
#-------------------------------------------------------------------------

#LIBS   += ./lib/libreadline.a ./lib/libhistory.a ./lib/libtermcap.a
LIBS   += -lreadline -lhistory -ltermcap

#-------------------------------------------------------------------------
# defined variables
# if compiling on MSYS use:
# $ make mingw=true
#-------------------------------------------------------------------------
# CURDATE := "-DCURDATE=`date +%s`"

mingw :=
ifdef msys
MINGW32 := "-D_MINGW='true'"
else
MINGW32 := 
endif

#-------------------------------------------------------------------------
# Source and Objects 
#-------------------------------------------------------------------------
SRCS    := $(shell find ./src -path './src/3rdparty/gaut' -prune -iname '*.c' -o -iname '*.cc' -o -iname '*.cpp' )

OBJS    := $(patsubst %.c, %.o,   $(filter %.c, $(SRCS)))   \
	   $(patsubst %.cc, %.o,  $(filter %.cc, $(SRCS)))  \
	   $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRCS))) 

DEPS    := $(OBJS:.o=.d)

HEADERS := $(shell find ./src -iname '*.h' -o -iname '*.hh' -o -iname '*.hpp')
INCDIRS := $(patsubst %, -I%, $(sort $(dir $(HEADERS))))

USE_INLINE := "-DINLINE"
STEADY_FLAGS := $(USE_INLINE) $(INCDIRS) $(CURDATE) $(MINGW32)
CFLAGS  := $(USRFLAGS) "-DNDEBUG" $(STEADY_FLAGS)

#-------------------------------------------------------------------------
# Actual targets
#-------------------------------------------------------------------------
.PHONY: tags clean docs 
all: release
release: $(TARGET)

debug:   CFLAGS := $(GDBFLAGS) "-D_DEBUG" $(STEADY_FLAGS)
debug:   LDFLAGS:= $(LDFLAGS) $(GDBLDFLAGS)
debug:   $(TARGET)

$(TARGET): $(OBJS) 
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

project: 
	@rm -f $(NAME).project
	@touch $(NAME).project
	@echo $(NAME)=$(shell pwd) flags=Sr { >> $(NAME).project
	@echo Makefile >> $(NAME).project;
	@echo Revision >> $(NAME).project;
	@echo ToDo >> $(NAME).project;
	@for opt in ./src/*; do \
 	  cd $$opt; \
	  echo `basename $$opt`=$(notdir $$opt) filter=\"\*.c \*.cc \*.cpp \*.h \*.hh \*.hpp \*.inl\" { >> ../../$(NAME).project; \
	  touch filter4$(NAME).project; \
	  find . -iname '*.c' -o -iname '*.cc' -o -iname '*.cpp' -o -iname '*.h' -o -iname '*.hh' -o -iname '*.hpp' -o -iname '*.inl' >> filter4$(NAME).project; \
	  sort filter4$(NAME).project | tr ' ' '\n' >> ../../$(NAME).project; \
	  rm filter4$(NAME).project; \
	  echo } >> ../../$(NAME).project; \
	  cd ../../; \
  	done
	@echo } >> $(NAME).project
	@echo $(NAME).project has been create

clean: 
	rm -fr $(OBJS) $(DEPS) $(TARGET) 

tags:
	ctags -R --file-scope=no --c++-kinds=+p --fields=+iamnlKS --extra=+q --languages=c++ --langmap=C++:+.inl -I
	mkid --include="C C++"

distclean: clean
	rm -fr dotfiles tags $(NAME).project ID 

tar:    distclean
	tar -C .. --exclude ".*" --exclude "$(TARGET)??????.tar.gz" -zcvf $(TARGET)`date "+%m%d%y"`.tar.gz `basename \`pwd\``


#-------------------------------------------------------------------------
# Implicit Targets
#-------------------------------------------------------------------------

%.d: %.c 
	@$(CC)  $(CFLAGS) -MM -MG $< | sed -e "s@^\(.*\)\.o:@`dirname $<`/\1.d `dirname $<`/\1.o:@" > $@
%.d: %.cc 
	@$(CPP) $(CFLAGS) -MM -MG $< | sed -e "s@^\(.*\)\.o:@`dirname $<`/\1.d `dirname $<`/\1.o:@" > $@
%.d: %.cpp 
	@$(CPP) $(CFLAGS) -MM -MG $< | sed -e "s@^\(.*\)\.o:@`dirname $<`/\1.d `dirname $<`/\1.o:@" > $@
-include $(DEPS)

%.o: %.c 
	$(CC)  $(CFLAGS) -c $< -o $@
%.o: %.cc 
	$(CPP)  $(CFLAGS) -c $< -o $@
%.o: %.cpp 
	$(CPP)  $(CFLAGS) -c $< -o $@
