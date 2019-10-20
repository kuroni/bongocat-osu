SRCDIR := src
OBJDIR := obj
ICODIR := ico
SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
ICO := $(ICODIR)/ico.res
CXXFLAGS := -std=c++17 -s -O2 -mwindows
LDFLAGS := -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic
INCFLAGS := -I/<SFML-folder>/include -Iinclude
LIBFLAGS := -L/<SFML-folder>/lib
DEFINES := -DSFML_STATIC

.PHONY: test clean

bin/bongo.exe: $(OBJ) $(ICO)
	$(CXX) -o $@ $^ $(DEFINES) $(INCFLAGS) $(LIBFLAGS) $(CXXFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c -o $@ $^ $(DEFINES) $(INCFLAGS) $(LIBFLAGS) $(CXXFLAGS) $(LDFLAGS)

$(ICODIR)/ico.res: $(ICODIR)/ico.rc
	windres -O coff -o $@ $^

test:
	bin/bongo.exe

clean:
	rm $(OBJ)