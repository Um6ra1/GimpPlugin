# Makefile
TGT	= Test.dll

CC = g++
OBJS = Main.o

LDFLAGS = 
CFLAGS = -O3 -shared -Wl,--export-all-symbols
INCLUDE =
LIBS	=
#LIBS	+= -ladvapi32 -luser32 -lgdi32 -lkernel32 -lmingwex

.PHONY: all
all: $(TGT)

$(TGT): $(OBJS)# $(RCOBJS)
	$(CC) $(MEXFLAGS) $(OBJS) $(LIBS) -o $@
#	$(CC) $(MEXFLAGS) -o $@ $(LIBS) $(OBJS)
	@echo Done.

.cpp.o:
	$(CC) $(CFLAGS) $(MEXFLAGS) $(LDFLAGS) $(INCLUDE) -c $<

%.o: %.rc
	windres $^ $@

.PHONY: clean
clean:
	@rm -f *.o*
	@echo Cleaned.
