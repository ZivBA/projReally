CC = gcc

LIB = /cs/bio3d/dina/libs/gamb++/
MYLIB =  /cs/bio3d/dina/libs/DockingLib
LIBLEDA =  /cs/bio3d/dina/libs/leda/
LEDA =  /cs/bio3d/dina/libs/leda/incl/

CCFLAGS = -I$(LIB) -I$(MYLIB) -I$(LEDA) -Wall -O3 -DNDEBUG -fexpensive-optimizations -ffast-math
#CCFLAGS = -I$(LIB) -I$(MYLIB) -I$(LEDA) -Wall -O3 -g
LDFLAGS = -static -L$(LIB) -L$(MYLIB) -L$(LIBLEDA) -ldockingLib -lgamb++ -lleda -lpthread -lstdc++ -lm
#LDFLAGS = -L$(LIB) -L$(MYLIB) -L$(LIBLEDA) -ldockingLib -lgamb++ -lleda -lstdc++ -lm -lX11 -ltcmalloc

CLASSES = main

# Prepare object and source file list using pattern substitution func.
ALL  = $(CLASSES)
OBJS = $(patsubst %, %.o,  $(ALL))
SRCS = $(patsubst %, %.cc, $(CLASSES))

TARGET = projTest

#clean_before_compile: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.cpp
	$(CC) $(CCFLAGS) -c $*.cpp

clean:
	/bin/rm -f $(OBJS) $(TARGET)

backup:
	zip backup.zip *.cpp *.h Makefile* readme *.txt run

install:
	@cp $(TARGET) Debug/.;\

depend:
	makedepend -- $(CCFLAGS) -- $(SRCS)
# DO NOT DELETE THIS LINE -- make depend depends on it.
