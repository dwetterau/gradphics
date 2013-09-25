#
#  Makefile for campus Linux computers
#

INCLUDE = -I/lusr/include -I/lusr/X11/include -I/p/include
LIBDIR = -L/lusr/lib -L/lusr/X11/lib -L/p/lib

LIBS = -lfltk -lfltk_gl -lfltk_images -lGL -ljpeg -lpng -lz

CFLAGS = -g

CC = g++

.cpp.o: 
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $*.o $<

ALL.O = bitmap.o impBrush.o \
	impressionist.o impressionistDoc.o impressionistUI.o \
	originalView.o paintView.o \
	pointBrush.o circleBrush.o scatteredPointBrush.o scatteredCircleBrush.o lineBrush.o scatteredLineBrush.o haloBrush.o

impressionist: $(ALL.O)
	$(CC) $(CFLAGS) -o $@ $(ALL.O) $(INCLUDE) $(LIBDIR) $(LIBS)

clean:
	rm -f *.o impressionist

