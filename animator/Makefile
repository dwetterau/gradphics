
#  Makefile for fltk applications
#

LOCAL = /lusr

FLTK = /p

INCLUDE = -I$(LOCAL)/include -I$(FLTK)/include -I/lusr/X11/include
LIBDIR = -L$(LOCAL)/lib -L$(FLTK)/lib -L/lusr/X11/lib

LIBS = -lfltk -lfltk_gl -lXext -lX11 -lm -lGL -lGLU

CFLAGS = -g

CC = g++

.SUFFIXES: .o .cpp .cxx

.cpp.o: 
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $*.o $<

ALL.O = bitmap.o camera.o color.o curve.o curveevaluator.o \
        graphwidget.o indicatorwindow.o linearcurveevaluator.o \
        modelerapp.o modelerdraw.o modelerui.o animatoruiwindows.o \
        modelerview.o particleSystem.o point.o \
        rect.o robotarm.o rulerwindow.o

OURS.O = bitmap.o camera.o color.o curve.o curveevaluator.o \
        graphwidget.o indicatorwindow.o linearcurveevaluator.o \
        modelerapp.o modelerdraw.o modelerui.o animatoruiwindows.o \
        modelerview.o particleSystem.o point.o \
        rect.o xwing.o rulerwindow.o

animator:  $(OURS.O)
	$(CC) $(CFLAGS) -o $@ $(OURS.O) $(INCLUDE) $(LIBDIR) $(LIBS)

old_animator: $(ALL.O)
	$(CC) $(CFLAGS) -o $@ $(ALL.O) $(INCLUDE) $(LIBDIR) $(LIBS)

clean:  
	rm -f $(ALL.O)

clean_all:
	rm -f $(ALL.O) animator
