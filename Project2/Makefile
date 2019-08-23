SRC:=$(wildcard ./*.cpp)
OBJS:=$(patsubst %.cpp, %.o, $(SRC))
GCC:=g++
INC:=./

TARGET:=test

all:$(TARGET)
$(TARGET):$(OBJS)
	$(GCC) $^ -I$(INC) -I/usr/local/include -L/usr/local/lib64 -lssl -pthread  -o $@

%.o:%.cpp
	$(GCC) -c -g -std=c++11  $< -o $@ 

.PHONY:clean
clean:
	$(RM) $(OBJS) $(TARGET)
