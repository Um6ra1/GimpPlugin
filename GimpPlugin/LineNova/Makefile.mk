# Makefile
TGT	= LineNova

OBJS = Main.obj Dialog.obj Resource.res

CFLAGS =

.PHONY: all
all: $(TGT).exe

$(TGT).exe: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
	@echo Done.

.cpp.obj:
	$(CC) -c $(CFLAGS) $<
	
.rc.res:
	$(RC) $<

.PHONY: clean
clean:
	@del -f *.obj*
	@echo Cleaned.
