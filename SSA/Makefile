CXX?=g++
CC=$(CXX)
CXXFLAGS?=-std=c++11 -Wall -Werror -pedantic-errors -g -O0
BISONFLAGS?=-v -Dparse.trace
PS=Parser/

%.tab.cpp %.tab.hpp: %.ypp
	bison $(BISONFLAGS) --file-prefix=$* $< 

%.flex.cpp %.flex.h: %.lpp
	flex --outfile=$*.flex.cpp --header-file=$*.flex.h $<

all: SSA

release:
	make clean
	CXXFLAGS="-std=c++11 -O2 -DNDEBUG -march=native" \
	BISONFLAGS="" make all 

Error.o: Error.cpp Error.h

VarCont.o: VarCont.cpp VarCont.h

SSA.o: SSA.cpp $(PS)ParsingDriver.h Error.h SyntaxTree.h TreeAnalyzer.h \
SSAPrinter.h

SSA: SSA.o Error.o VarCont.o TreeAnalyzer.o SSAPrinter.o \
$(PS)ParsingDriver.o $(PS)lexer.flex.o $(PS)parser.tab.o

TreeAnalyzer.o: TreeAnalyzer.cpp TreeAnalyzer.h Table.h VarCont.h \
Error.h SyntaxTree.h

$(PS)lexer.flex.o: $(PS)lexer.lpp $(PS)parser.tab.hpp $(PS)ParsingDriver.h

$(PS)parser.tab.o: $(PS)parser.ypp $(PS)ParsingDriver.h SyntaxTree.h \
Table.h

$(PS)ParsingDriver.o: $(PS)ParsingDriver.cpp $(PS)ParsingDriver.h \
$(PS)lexer.flex.h $(PS)parser.tab.hpp SyntaxTree.h Table.h \
Error.h

$(PS)ParsingDriver.h: $(PS)parser.tab.hpp $(PS)lexer.flex.h

SSAPrinter.o: SSAPrinter.h SyntaxTree.h Table.h

clean:
	rm -f *.o *~ Exprs/*~ $(PS)*~ $(PS)*.o \
	$(PS)*.tab.* $(PS)*.flex.* $(PS)stack.hh \
	$(PS)parser.output SSA output.txt