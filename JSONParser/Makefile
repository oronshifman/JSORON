TARGET = JSONParser

OBJS = src/JSONParser.o test/test_JSONParser.o ../JSONObject/src/JSONObject.o

TEST=../../utils/generic_test.o

RELEASE_BIN = build/$(TARGET)_release
DEBUG_BIN = build/$(TARGET)_debug

CXX=g++

CXXFLAGS=-Wall -std=c++11

CPPFLAGS=-Iinclude -I../../new_part2/utils -I../../new_part2/profiler/include -I../JSONObject/include

RELEASE_FLAGS=-DNDEBUG -O3
DEBUG_FLAG=-g3

all:

release: CXXFLAGS+=$(RELEASE_FLAGS)
release: $(RELEASE_BIN) 

debug: CXXFLAGS+=$(DEBUG_FLAG)
debug: $(DEBUG_BIN)

$(DEBUG_BIN): $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(DEBUG_BIN) $(OBJS) $(TEST)

$(RELEASE_BIN): $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(RELEASE_BIN) $(OBJS) $(TEST)

clean:
	-rm -f build/* src/*.o test/*.o; touch build/dummy.md
