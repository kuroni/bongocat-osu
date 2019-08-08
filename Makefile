CXX := x86_64-w64-mingw32-g++
WINDRES := x86_64-w64-mingw32-windres
SRCDIR := src
OBJDIR := obj
ICODIR := ico
SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
ICO := $(ICODIR)/ico.res
CXXFLAGS := -std=c++17 -s -O2 -mwindows
LDFLAGS := -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic
INCFLAGS := -I/home/kuroni/Documents/Repositories/external_lib/SFML-2.5.1-32bit/include -Iinclude
LIBFLAGS := -L/home/kuroni/Documents/Repositories/external_lib/SFML-2.5.1-32bit/lib
DEFINES := -DSFML_STATIC

.PHONY: test clean

bin/bongo.exe: $(OBJ) $(ICO)
	$(CXX) -o $@ $^ $(DEFINES) $(INCFLAGS) $(LIBFLAGS) $(CXXFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c -o $@ $^ $(DEFINES) $(INCFLAGS) $(LIBFLAGS) $(CXXFLAGS) $(LDFLAGS)

$(ICODIR)/ico.res: $(ICODIR)/ico.rc
	$(WINDRES) -O coff -o $@ $^

test:
	WINEDEBUG=-all,+fps wine bin/bongo.exe

clean:
	rm $(OBJ) $(ICODIR)/ico.res
