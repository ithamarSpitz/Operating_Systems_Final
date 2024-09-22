CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -pthread
LDFLAGS = -pthread

SRCS = main.cpp Graph.cpp MSTAlgorithm.cpp MST.cpp MSTFactory.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

TARGET = mst_server

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)