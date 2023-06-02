CC=g++
OBJ_ARGS=-g
CC_SRCS=main.cpp flog.cpp hashFuncs.cpp
TESTSRCS=test.cpp
OBJDIR=obj/
DEPDIR=dep/
CC_ARGS=-g
ASM=nasm

all: $(OBJDIR) $(DEPDIR) hash tester

$(OBJDIR)%.o: %.cpp
	$(CC) -M $(CC_ARGS) $< -o $(DEPDIR)$(<:.cpp=.d)
	echo "$(OBJDIR)" > line.tmp
	mv $(DEPDIR)$(<:.cpp=.d) input.tmp
	head -c -1 -q line.tmp input.tmp > $(DEPDIR)$(<:.cpp=.d)
	rm input.tmp line.tmp
	$(CC) -c $(CC_ARGS) $< -o $@

include $(wildcard $(DEPDIR)/*.d)

_rolHash.o: rolHash.asm
	$(ASM) -f elf64 $^ -o $@

hash: $(addprefix $(OBJDIR), $(CC_SRCS:.cpp=.o)) _rolHash.o
	$(CC) -no-pie -m64 $^ -o $@ $(OBJ_ARGS) -lc

tester: $(addprefix $(OBJDIR), $(TESTSRCS:.cpp=.o))
	$(CC) $^ -o $@ $(OBJ_ARGS)

$(OBJDIR):
	mkdir $@

$(DEPDIR):
	mkdir $@

grind:
	sudo kcachegrind oneHash.out.callgrind
	sudo kcachegrind firstAsciiHash.out.callgrind
	sudo kcachegrind lenHash.out.callgrind
	sudo kcachegrind asciiSumHash.out.callgrind
	sudo kcachegrind rolHash.out.callgrind
	sudo kcachegrind rorHash.out.callgrind
	sudo kcachegrind crc32Hash.out.callgrind

clean:
	rm -rf *.o *.d hash *.dot *.data* *.out.callgrind *.hist tester
	rm -rf $(DEPDIR) $(OBJDIR)
	clear

.PHONY: clean all grind