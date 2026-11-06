# Makefile - reactor_server
CXX      ?= g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pthread
# make asan=1 构建地址消毒版本用于调试
ifeq ($(asan),1)
CXXFLAGS += -g -fsanitize=address,undefined
endif
LDFLAGS  :=
TARGET   := reactor_server
SRCS     := $(wildcard *.cpp)
HDRS     := $(wildcard *.h)

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
