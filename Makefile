CC=g++
OBJ_ARGS=-g
CC_SRCS=main.cpp flog.cpp hashFuncs.cpp
OBJDIR=obj/
DEPDIR=dep/
CC_ARGS=-g

all: $(OBJDIR) $(DEPDIR) hash

$(OBJDIR)%.o: %.cpp
	$(CC) -M $(CC_ARGS) $< -o $(DEPDIR)$(<:.cpp=.d)
	echo "$(OBJDIR)" > line.tmp
	mv $(DEPDIR)$(<:.cpp=.d) input.tmp
	head -c -1 -q line.tmp input.tmp > $(DEPDIR)$(<:.cpp=.d)
	rm input.tmp line.tmp
	$(CC) -c $(CC_ARGS) $< -o $@

include $(wildcard $(DEPDIR)/*.d)

hash: $(addprefix $(OBJDIR), $(CC_SRCS:.cpp=.o))
	$(CC) $^ -o $@ $(OBJ_ARGS)

$(OBJDIR):
	mkdir $@

$(DEPDIR):
	mkdir $@

grind:
	rm -rf callgrind.out*
	sudo valgrind --tool=callgrind ./hash
	sudo kcachegrind callgrind.out*

clean:
	rm -rf *.o *.d hash *.dot *.png *.data* *.out.callgrind *.hist
	rm -rf $(DEPDIR) $(OBJDIR)
	clear

.PHONY: clean all grind