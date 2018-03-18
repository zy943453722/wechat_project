
SRCS = $(wildcard *.cpp)

OBJS = $(patsubst %.cpp,%.o,$(SRCS))

INCL=-I/usr/include/mysql -I./
LIB=-L/usr/lib/x86_64-linux-gnu -lmysqlclient -static -lpthread -ldl -lz
.SUFFIXES: .o .cpp

EXE=./test

all:$(EXE)

$(EXE):$(OBJS)
	g++ $(INCL) -o $(EXE) $^ $(LIB) 
	
.cpp.o:
	g++ $(INCL) -c $< 

clean:
	 rm -f $(OBJS) $(EXE)
