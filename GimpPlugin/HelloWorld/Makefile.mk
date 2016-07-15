# Makefile
TGT	= Test.exe

OBJS = Main.obj 

CFLAGS =# /LD
#INCLUDE =
#LIBS	=
#LIBS	+= -ladvapi32 -luser32 -lgdi32 -lkernel32 -lmingwex

.PHONY: all
all: $(TGT)

$(TGT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
	@echo Done.

.cpp.obj:
	$(CC) -c $(CFLAGS) $(LDFLAGS) $(INCLUDE) $<

.PHONY: clean
clean:
	@rm -f *.obj*
	@echo Cleaned.
