# Makefile
TGT	= LensBlur2.exe

OBJS = Main.obj 

CFLAGS = /Ox
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
	@del -f *.obj*
	@echo Cleaned.