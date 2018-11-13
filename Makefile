#####################################################################

include hi3518e.mk


# target source
#SRC  := $(wildcard *.c) 
#OBJ  := $(SRC:%.c=%.o)

#TARGET := $(OBJ:%.o=%)
#.PHONY : clean all

#all: $(TARGET)


#$(TARGET):%:%.o
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS_FLAGS) $(LIBS_LD_FLAGS)

	
TARGET  = hi3518e

OBJ = $(patsubst %.c, %.o, $(wildcard *.c))


all:$(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) *.o $(LIBS_FLAGS) $(LIBS_LD_FLAGS)
	
clean:
	@rm -f $(TARGET)
	@rm -f $(OBJ)
