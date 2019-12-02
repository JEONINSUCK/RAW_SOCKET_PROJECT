CC=gcc
OBJECT=icmp_raw.o
OBJECTS=netif.o icmp_raw.o
TARGET=icmp_raw
CODE=icmp_raw.c


$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

$(OBJECT):$(CODE)
	$(CC) -g -c -o $(OBJECT) $(CODE)

netif.o:netif.c
	$(CC) -g -c -o netif.o netif.c

clean:
	rm -rf $(TARGET)
	rm -rf *.o
	rm -rf ._*.*
