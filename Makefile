# Makefile:

IDIRS		= -I.. -I/usr/local/include/stl -I/usr/local/include
CXXFLAGS	= $(IDIRS) -g3d $(XFLAGS) -w1 #-WC,-ew -Oatx
LDFLAGS		= -L.. -L/usr/local/lib -g3d
LDLIBS		= -loptions -lsocket++

SRC			= $(wildcard *.cc)
OBJ			= message.o header.o field.o address.o datetime.o quote.o \
				mime.o entity.o multipart.o text.o application.o \
				qp.o base64.o \
				drums.o parse822.o parse2045.o \
				rfc821.o
LIB			= libmail++.a
EXE			= maildump mail
DEP			= .depends

CXX = cc

.PHONY: all

all: $(LIB) $(EXE)

$(EXE): libmail++.a

echo:
	: XFLAGS: $(XFLAGS)
	: CXXFLAGS: $(CXXFLAGS)
	: LDFLAGS: $(LDFLAGS)

lib: $(LIB)

$(LIB): $(OBJ)
	ar r -cq $@ $^

clean:
	rm -f *.o *.map *.err

empty: clean
	rm -f $(EXE) $(LIB)

-include $(DEP)

deps:
	makedepend -f - -I /usr/local/include -- $(CXXFLAGS) -- $(SRC) > $(DEP) 2>/dev/null

