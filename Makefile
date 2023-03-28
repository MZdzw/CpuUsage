CC := $(CC)
CPPFLAGS := -MMD -MP
ifeq ($(CC),gcc)
  CFLAGS = -Wall -Wextra
else ifeq ($(CC),clang)
  CFLAGS = -Weverything -Wno-padded -Wno-declaration-after-statement -Wno-shadow
  CFLAGS += -Wno-implicit-int-float-conversion -Wno-disabled-macro-expansion
endif
#CFLAGS = -Wall -Wextra
LDFLAGS  := -Llib
LDLIBS   := -lm


PRJ_NAME = qualTask
TARGET_EXTENSION = out

#Common (for program and for tests)
BUILD_DIR = build

SRC_DIR = src
OBJ_DIR := build/obj
BIN_DIR := build/bin
###########################

#For program
INC_DIR = inc

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(subst $(SRC_DIR), $(OBJ_DIR), $(SRC))
OBJ := $(OBJ:.c=.o)
########################

#For tests
UNITY_DIR = Unity/src
TEST_DIR = tests
RESULTS_DIR = build/results

SRCTEST = $(wildcard $(TEST_DIR)/*.c) $(wildcard $(UNITY_DIR)/*.c) $(wildcard $(SRC_DIR)/*.c)
SRCTEST := $(filter-out $(SRC_DIR)/main.c, $(SRCTEST))
OBJTEST = $(subst $(TEST_DIR), $(OBJ_DIR), $(SRCTEST))
OBJTEST := $(subst $(UNITY_DIR), $(OBJ_DIR), $(OBJTEST))
OBJTEST := $(subst $(SRC_DIR), $(OBJ_DIR), $(OBJTEST))
OBJTEST := $(OBJTEST:.c=.o)

RESULTS = $(wildcard $(TEST_DIR)/*.c)
RESULTS := $(patsubst $(TEST_DIR)/Test%.c,$(RESULTS_DIR)/Test%.txt,$(RESULTS) )

PASSED = `grep -s PASS $(RESULTS_DIR)/*.txt`
FAIL = `grep -s FAIL $(RESULTS_DIR)/*.txt`
IGNORE = `grep -s IGNORE $(RESULTS_DIR)/*.txt`
##################

.PHONY: all test clean

all: $(PRJ_NAME).$(TARGET_EXTENSION)

$(PRJ_NAME).$(TARGET_EXTENSION): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -I$(INC_DIR) $(CPPFLAGS) -pthread $(CFLAGS) -c $< -o $@

##########################################################################################

test: $(RESULTS)
	@echo "-----------------------\nIGNORES:\n-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------\nPASSED:\n-----------------------"
	@echo "$(PASSED)"
	@echo "\nDONE"

$(RESULTS): $(PRJ_NAME)Test.$(TARGET_EXTENSION) | $(RESULTS_DIR)
	-./$(BIN_DIR)/$< > $@ 2>&1

$(PRJ_NAME)Test.$(TARGET_EXTENSION): $(OBJTEST) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) -I$(INC_DIR) -I$(UNITY_DIR)/ $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -I$(INC_DIR) $(CPPFLAGS) -pthread $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(UNITY_DIR)/%.c | $(OBJ_DIR)
	$(CC) -I$(UNITY_DIR)/ $(CPPFLAGS) $(CFLAGS) -c $< -o $@

############################################################################################

-include $(OBJ:.o=.d)

$(BIN_DIR) $(OBJ_DIR) $(RESULTS_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(OBJ_DIR)/* $(BIN_DIR)/* $(RESULTS_DIR)/*
