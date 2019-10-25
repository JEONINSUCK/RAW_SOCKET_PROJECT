CC=gcc
OBJECT=arp_raw.o
OBJECTS=netif.o arp_raw.o
TARGET=arp_raw
CODE=arp_raw.c


$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

$(OBJECT):$(CODE)
	$(CC) -g -c -o $(OBJECT) $(CODE)

netif.o:netif.c
	$(CC) -g -c -o netif.o netif.c

clean:
	rm -rf $(TARGET)
	rm -rf *.o
