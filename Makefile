#####################################################################

include hi3518e.mk

PWD        := $(shell pwd)

BIN_DIR		:= $(PWD)/bin

LIB_TARGET	:= $(BIN_DIR)/libhi3518e_avio.so
LIB_NAME	:= -lhi3518e_avio
LIB_SRC		:= $(PWD)/src
INC_DIR		:= -I$(PWD)/include


EXE_TARGET  := $(BIN_DIR)/hi3518e
EXE_SRC		:= $(PWD)/main/hi_main.c

PHONY := all
all: lib exe

PHONY += lib
lib:
	$(CC) -fPIC -shared $(CFLAGS) $(INC_DIR) -o $(LIB_TARGET) $(LIB_SRC)/*.c $(LIBS_FLAGS) $(LIBS_LD_FLAGS)
	
	
PHONY += exe
exe:	
	$(CC) -o $(EXE_TARGET) $(EXE_SRC) -I$(LIB_SRC) -L$(BIN_DIR) $(LIB_NAME) $(LIBS_FLAGS) $(LIBS_LD_FLAGS)
	
clean:
	@rm -rf $(BIN_DIR)/*
	@rm -rf $(LIB_SRC)/*.o
