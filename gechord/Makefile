TARGET = $(shell basename $(CURDIR))
CC = g++
CFLAGS = -Qunused-arguments -ffinite-math-only -fno-math-errno -fno-strict-aliasing -O3 -std=c++11 \
		-fvisibility=hidden -ggdb -pipe -Wall -Wextra -fPIC -pthread -DNDEBUG -DSCRIPT
LIBS = -lgecodeflatzinc -lgecodedriver -lgecodesearch -lgecodeminimodel -lgecodeset -lgecodefloat -lgecodeint -lgecodekernel -lgecodesupport

default: $(TARGET)

$(TARGET).o: $(TARGET).cpp 
	echo $(OBJECTS)
	$(CC) -I. $(CFLAGS) -c -o $(TARGET).o $(TARGET).cpp

$(TARGET): $(TARGET).o
	$(CC) -o $(TARGET) $(TARGET).o -L. -I. $(CFLAGS) $(LIBS)

clean:
	rm -f *.o
	rm -f $(TARGET)