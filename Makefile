# Makefile: base64

IDIRS		= -I.. -I/usr/local/include/stl -I/usr/local/include
CXXFLAGS	= -g2 $(XFLAGS) $(IDIRS) # -Oatx
LDFLAGS		= -L.. -L/usr/local/lib -M -g
LDLIBS		= -loptions -lsocket++

SRC			= $(wildcard *.cc)
OBJ			= msg_822.o parse822.o
LIB			= libmail++.a
EXE			= maildump mail crope
DEP			= depends.mak

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

test_mail: mail
	mail -f sam < text | hd -v
	echo "yeeeeeeeeeeeeeeeeee haaaaaaaaaaahh!!!" | mail -f sam | hd -v
	mail -f root -r sam@cogent.ca --cc cc1 cc2 cc3 -b ba bb -s "a subject" to1 to2 to3 < _text 

test_opts: mail
	./mail -h
	./mail --help
	./mail -s "the subject" -a a1 a2 -aa3 -w
	./mail -aa1 a2 -- t1 t2

clean:
	rm -f *.o *.map *.err

empty: clean
	rm -f $(EXE) $(LIB)

-include $(DEP)

deps:
	makedepend -f - -I /usr/local/include -- $(CXXFLAGS) -- $(SRC) > $(DEP) 2>/dev/null

