# Makefile

TGT	= LineNova.exe

OBJS = Main.obj Dialog.obj Resource.res

CFLAGS =
LFLAGS =
#INCLUDE =
#LIBS	=
#LIBS	+= -ladvapi32 -luser32 -lgdi32 -lkernel32

.PHONY: all
all: $(TGT)

$(TGT): $(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
	link $** /out:$@ $(LFLAGS)
	@echo Done.

.cpp.obj:
	$(CC) -c $(CFLAGS) $(LDFLAGS) $(INCLUDE) $<
	
.rc.res:
	$(RC) $<

.PHONY: clean
clean:
	@del -f *.obj*
	@echo Cleaned.
