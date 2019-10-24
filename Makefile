CC=gcc
OBJECT=arp_raw.o
TARGET=arp_raw
CODE=arp_raw.c

$(TARGET):$(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT)

$(OBJECT):$(CODE)
	$(CC) -g -c -o $(OBJECT) $(CODE)

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJECT)
