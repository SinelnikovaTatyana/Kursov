.PHONY: all debug release test clean

Compiler = g++
RELEASE_FLAGS = -O2 -Wall -DNDEBUG
DEBUG_FLAGS   = -g -O0 -Wall
INCLUDE_DIRS  = -Iheaders
SOURCES       = authorization.cpp calculator.cpp error.cpp Interface.cpp server.cpp
MAIN_SOURCE   = main.cpp
TEST_SOURCE   = test.cpp
LIBS          = -lboost_program_options -lcryptopp -lUnitTest++

RELEASE_EXEC  = main
DEBUG_EXEC    = main-dbg
TEST_EXEC     = test

all: debug release test

debug: $(DEBUG_EXEC)

$(DEBUG_EXEC): $(SOURCES) $(MAIN_SOURCE)
	$(Compiler) $(DEBUG_FLAGS) $(INCLUDE_DIRS) $(SOURCES) $(MAIN_SOURCE) -o $(DEBUG_EXEC) $(LIBS)

release: $(RELEASE_EXEC)

$(RELEASE_EXEC): $(SOURCES) $(MAIN_SOURCE)
	$(Compiler) $(RELEASE_FLAGS) $(INCLUDE_DIRS) $(SOURCES) $(MAIN_SOURCE) -o $(RELEASE_EXEC) $(LIBS)

test: $(TEST_EXEC)

$(TEST_EXEC): $(SOURCES) $(TEST_SOURCE)
	$(Compiler) $(DEBUG_FLAGS) $(INCLUDE_DIRS) $(SOURCES) $(TEST_SOURCE) -o $(TEST_EXEC) $(LIBS)

clean:
	rm -f $(RELEASE_EXEC) $(DEBUG_EXEC) $(TEST_EXEC)
