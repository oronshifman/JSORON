TARGET = JSONObject

OBJS = src/JSONObject.o test/JSONObject_main.o

TEST=../../utils/generic_test.o

RELEASE_BIN = build/$(TARGET)_release
DEBUG_BIN = build/$(TARGET)_debug
PROFILED_BIN = build/$(TARGET)_profiled

CXX=g++

CXXFLAGS=-Wall -std=c++20

CPPFLAGS=-Iinclude -I../../utils -I../../profiler/include -I../JSONParser/include

RELEASE_FLAGS=-DNDEBUG -O3
DEBUG_FLAG=-g3
PROFILED_FLAGS=-DPROFILING

all: debug release profiled

release: CXXFLAGS+=$(RELEASE_FLAGS)
release: $(RELEASE_BIN) 

debug: CXXFLAGS+=$(DEBUG_FLAG)
debug: $(DEBUG_BIN)

profiled: CXXFLAGS+=$(PROFILED_FLAGS)
profiled: OBJS+=../../profiler/src/profiler.o
profiled: $(PROFILED_BIN)

$(DEBUG_BIN): $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(DEBUG_BIN) $(OBJS) $(TEST)

$(RELEASE_BIN): $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(RELEASE_BIN) $(OBJS) $(TEST)

$(PROFILED_BIN): $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(PROFILED_BIN) $(OBJS) $(TEST)

clean:
	-rm -f build/* src/*.o test/*.o; touch build/dummy.md
