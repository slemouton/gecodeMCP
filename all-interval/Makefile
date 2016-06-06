TARGET = all-interval
CC = g++
CFLAGS = -Qunused-arguments -fno-signaling-nans -fno-rounding-math -ffinite-math-only -fno-math-errno -fno-strict-aliasing -O3 -fvisibility=hidden -ggdb -pipe -Wall -Wextra -fPIC -pthread -DNDEBUG
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