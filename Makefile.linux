CXX := g++
SRCDIR := src
OBJDIR := obj
ICODIR := ico
SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
CXXFLAGS := -std=c++17 -O2
LDFLAGS := -lxdo -lsfml-graphics -lsfml-window -lsfml-system -lSDL2 -lX11 -lXrandr
INCFLAGS := -Iinclude

.PHONY: test create clean

all: create bin/bongo

bin/bongo: $(OBJ)
	$(CXX) -o $@ $^ $(INCFLAGS) $(CXXFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c -o $@ $^ $(INCFLAGS) $(CXXFLAGS) $(LDFLAGS)

create:
	mkdir -p $(OBJDIR)
	mkdir -p bin

test:
	bin/bongo

clean:
	rm $(OBJ) || true

