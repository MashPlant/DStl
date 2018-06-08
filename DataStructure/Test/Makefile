SOURCE = $(wildcard *.cpp)
TARGET = $(SOURCE:%.cpp=%.exe)

all : $(TARGET)

%.exe : %.cpp
	-g++ $< -o $@

.PNONY : all clean

clean :
	rm *.exe
