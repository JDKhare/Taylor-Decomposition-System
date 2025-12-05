# makefile for "GAUT" tool

# paths to files
ARCH		= $(shell uname -s)-$(shell uname -m)
BIN		= .
TOOLS		= ../tools
SRC		= .
target := gaut
NAME	= $(target)

# tools
CC		= g++ -c
CFLAGS		= -DYYSTACKSIZE=5000 -DUNIX -Wno-deprecated -static
LD		= g++ 
LDFLAGS		= -static
LDLIBS		= -lm
YACC		= yacc -d
LEX		= lex -l

# list of all binary files to compile
BINS	=	allocation.o \
		b_graph.o \
		bus.o \
		cdfg.o \
		cdfg2c.o \
		check.o \
		clock.o \
		cycles.o \
		debug_toggles.o \
		dot_gantt.o \
		dot_mem.o \
		function.o \
		ic.o \
		io_constraints.o \
		lib.o \
		local_search.o \
		main.o \
		mapping_constraints.o \
		mentor_type.o \
		multimode_binding.o \
		multimode.o \
		multimode_tools.o \
		munkres.o \
		operation.o \
		operator.o \
		ordo2c.o \
		parse.o \
		pattern.o \
		port.o \
		reg.o \
		resizing.o \
		scheduling.o \
		selection.o \
		soclib.o \
		switches.o 

GDBFLAGS:= -O6

.PHONEY: clean cleall
release: $(target)

# targets
debug: GDBFLAGS := -O0 -g -pg
debug: $(target)

.cpp.o:
	$(CC) $(CFLAGS) $(GDBFLAGS) $< -o $@

$(target):	$(BINS)
	$(LD) $(CFLAGS) $(LDFLAGS) $(BINS) $(LDLIBS) -o $(BIN)/$(NAME)

# cleaning rules
clean:
	-$(RM) $(NAME) *.o  lex.yy.*


cleanall : clean
	-$(RM) $(target)

# end of makefile
