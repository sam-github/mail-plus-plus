# Makefile:

IDIRS		= -I../.. -I/usr/local/include/stl -I/usr/local/include
CXXFLAGS	= -g3d $(XFLAGS) $(IDIRS) -w1 #-WC,-ew -Oatx
LDFLAGS		= -L.. -L/usr/local/lib -g3d
LDLIBS		= -loptions -lsocket++

SRC			= $(wildcard *.cc)
EXE			= t_b64buf t_rope t_drums t_mime t_quote t_istext t_buf
DEP			= .depends

.PHONY: all

all: $(LIB) $(EXE)

$(EXE): ../libmail++.a

echo:
	: XFLAGS: $(XFLAGS)
	: CXXFLAGS: $(CXXFLAGS)
	: LDFLAGS: $(LDFLAGS)

clean:
	rm -f *.o *.map *.err

empty: clean
	rm -f $(EXE) $(LIB)

-include $(DEP)

deps:
	makedepend -f - -I /usr/local/include -- $(CXXFLAGS) -- $(SRC) > $(DEP) 2>/dev/null

