# Makefile: base64

IDIRS		= -I.. -I/usr/local/include/stl -I/usr/local/include
CXXFLAGS	= -g3d $(XFLAGS) $(IDIRS) -w1 #-WC,-ew -Oatx
LDFLAGS		= -L.. -L/usr/local/lib -M -g3d
LDLIBS		= -loptions -lsocket++

SRC			= $(wildcard *.cc)
OBJ			= message.o address.o datetime.o mime.o quote.o \
				drums.o parse822.o parse2045.o \
				rfc821.o
LIB			= libmail++.a
EXE			= maildump mail t_crope t_drums t_mime t_quote
DEP			= .depends

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

MCC		= cc1@lh cc2@lh
MBCC	= bcc1@lh bcc2@lh
MTO		= to1@lh to2@lh
MTOE	= 1to@lh 2to@lh
MRT		= reply-to@there

test_mail: mail
	mail -f root@localhost -r $(MRT) $(MTO) --cc $(MCC) -b $(MBCC) -s"" $(MTOE) < _text 

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

