CPP    	= clang++
CFLAGS 	= -g -O0
INC 	:= $(shell llvm-config-3.5 --ldflags --cppflags)
LIB 	:= $(shell llvm-config-3.5 --libs core)

OBJS = lexer.o error.o parser.o

.SUFFIXS: .cc .o
main: ${OBJS} 
	${CPP} ${OBJS} ${CFLAGS} ${INC} ${LIB} main.cc -o $@
%.o: %.cc 
	${CC} ${CFLAGS} ${INC} ${LIB} $< -c 

.PHONY: clean 
clean: 
	rm -rf a.out
	rm -rf *.h.gch
	rm -rf *.o
	rm -rf main
