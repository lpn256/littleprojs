CXX = g++
CXXFLAGS = -O3
APPNAME = cd2date-ng
SRCFILE = $(APPNAME).cpp

.PHONY: all clean

all: $(APPNAME)

$(APPNAME): $(SRCFILE)
	$(CXX) $(CXXFLAGS) $(SRCFILE) -o $(APPNAME)

clean:
	rm -f $(APPNAME)
