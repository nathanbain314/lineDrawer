CXX += -std=c++11 -stdlib=libc++ -O3 -g

EXECUTABLE = RunGridDraw RunLineDraw RunLineRect RunLinePolygon

SRC = lib
INCLUDE = include
OBJDIR = build

CFLAGS += -I$(INCLUDE) -Wimplicit-function-declaration -Wall -Wextra -pedantic
LDLIBS = -lncurses
VIPS_FLAGS = `pkg-config vips-cpp --cflags --libs`

all: $(EXECUTABLE)

RunGridDraw: $(OBJDIR)/gridDraw.o $(OBJDIR)/progressbar.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(LDLIBS) $(VIPS_FLAGS) $(OBJDIR)/gridDraw.o $(OBJDIR)/progressbar.o -o RunGridDraw

RunLineDraw: $(OBJDIR)/lineDraw.o $(OBJDIR)/progressbar.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(LDLIBS) $(VIPS_FLAGS) $(OBJDIR)/lineDraw.o $(OBJDIR)/progressbar.o -o RunLineDraw

RunLineRect: $(OBJDIR)/lineRect.o $(OBJDIR)/progressbar.o $(OBJDIR)/shortestPath.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(LDLIBS) $(VIPS_FLAGS) $(OBJDIR)/lineRect.o $(OBJDIR)/shortestPath.o $(OBJDIR)/progressbar.o -o RunLineRect

RunLinePolygon: $(OBJDIR)/linePolygon.o $(OBJDIR)/progressbar.o $(OBJDIR)/shortestPath.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(LDLIBS) $(VIPS_FLAGS) $(OBJDIR)/linePolygon.o $(OBJDIR)/shortestPath.o $(OBJDIR)/progressbar.o -o RunLinePolygon

$(OBJDIR)/%.o: $(SRC)/%.c $(INCLUDE)/%.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(OBJDIR)/%.o: $(SRC)/%.c++
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(VIPS_FLAGS) $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(EXECUTABLE)