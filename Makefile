CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
else
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
	LDFLAGS = -lglut -lGLU
endif

SOURCES = main.cpp LocalGeo.cpp BezierPatch.cpp Scene.cpp
OBJECTS = main.o LocalGeo.o BezierPatch.o Scene.o
	
RM = /bin/rm -f 
all: main 
main: $(OBJECTS)
	$(CC) $(CFLAGS) -o AS3 $(OBJECTS) $(LDFLAGS) 
main.o:
	$(CC) $(CFLAGS) -c $(SOURCES)
clean: 
	$(RM) *.o AS3
 


