.PHONY: all clean

CC=g++
CFLAGS=-I.
LDFLAGS+=


DIR1=src/sbin
DIR2=src/cbin
TARGET1=messenger
TARGET2=users
SOURCE1=src/code/messenger.cpp
SOURCE2=src/code/users.cpp

all: messenger users

messenger: src/code/messenger.cpp
	$(CC) -o $(DIR1)/$(TARGET1) $(SOURCE1) $(CFLAGS)

users: src/code/users.cpp
	$(CC) -o $(DIR2)/$(TARGET2) $(SOURCE2) $(CFLAGS)


clean:
	$(RM)  $(DIR1)/$(TARGET1) $(DIR2)/$(TARGET2)

