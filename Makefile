APP_NAME=skip-list

OBJS=main.o skip-list-fine.o skip-list-coarse.o skip-list-sequential.o checker.o
CXX = g++
CXXFLAGS = -Wall -O3 -std=c++17 -I. -Wno-unknown-pragmas -g

all: $(APP_NAME)

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	/bin/rm -rf *~ *.o $(APP_NAME) *.class
