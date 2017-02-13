# Makefile
TGT	= VsEffect3D
OBJS = Main.obj
CFLAGS =

.PHONY: all
all: $(TGT)

$(TGT).exe: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
	@echo Done.

.cpp.obj:
	$(CC) -c $(CFLAGS)  $<

.rc.res:
	$(RC) $<

.PHONY: clean
clean:
	@del -f *.obj*
	@echo Cleaned.
