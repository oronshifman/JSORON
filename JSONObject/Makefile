TARGET_NO_PROF = JSONObject
TARGET_PROF = JSONObject_profiled

OBJS = src/JSONObject.o 

MAIN_OBJ_NO_PROF=test/$(TARGET_NO_PROF)_main.o
MAIN_OBJ_PROF=test/$(TARGET_PROF)_main.o

TEST=../../utils/generic_test.o

RELEASE_BIN = build/$(TARGET_NO_PROF)_release
DEBUG_BIN = build/$(TARGET_NO_PROF)_debug
PROFILED_BIN = build/$(TARGET_PROF)_debug

CXX=g++

CXXFLAGS=-Wall -std=c++11

CPPFLAGS=-Iinclude -I../JSONParser/include -I../../utils

RELEASE_FLAGS=-DNDEBUG -O3
DEBUG_FLAG=-g3
PROFILING_FLAGS=-DPROFILING

all:

profiled: CXXFLAGS+=$(DEBUG_FLAG) $(PROFILING_FLAGS)
profiled: CPPFLAGS+=-I../../profiler/include
profiled: OBJS+=../../profiler/src/profiler.o
profiled: $(PROFILED_BIN)

release: CXXFLAGS+=$(RELEASE_FLAGS)
release: $(RELEASE_BIN) 

debug: CXXFLAGS+=$(DEBUG_FLAG)
debug: $(DEBUG_BIN)

$(PROFILED_BIN): $(MAIN_OBJ_PROF) $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(PROFILED_BIN) $(MAIN_OBJ_PROF) $(OBJS) $(TEST)

$(DEBUG_BIN): $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(DEBUG_BIN) $(OBJS) $(TEST)

$(RELEASE_BIN): $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(RELEASE_BIN) $(OBJS) $(TEST)

clean:
	-rm -f build/* src/*.o test/*.o; touch build/dummy.md