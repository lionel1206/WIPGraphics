########################################################################
# Makefile for Linux

CXXFLAGS = -I. -g -Iglsdk/glm -Iglsdk/glimg/include -Iglsdk/freeglut/include -Iglsdk/glload/include -I/usr/X11R6/include/GL/ -I/usr/include/GL/
LIBS =  -lfreeglut -lX11 -lGLU -lGL -L/usr/X11R6/lib -Lglsdk/glimg/lib/ -Lglsdk/glload/lib/ -Lglsdk/freeglut/lib/ -lglload -lglimg
target = framework.exe

src = framework.cpp shader.cpp fbo.cpp models.cpp scene.cpp
headers = scene.h shader.h fbo.h models.h
extras = framework.vcxproj Makefile final.frag final.vert phong.frag phong.vert glsdk

pkgFiles = $(src) $(headers) $(extras)
pkgName = CS300-$(notdir $(CURDIR))-framework

objects = $(patsubst %.cpp,%.o,$(src))

$(target): $(objects)
	@echo Link $(target)
	g++ -g  -o $@  $(objects) $(LIBS)

%.o: %.cpp
	@echo Compile $<
	@$(CXX) -c $(CXXFLAGS) $< -o $@

%.o: %.c
	@echo Compile $<
	@$(CC) -c $(CXXFLAGS) $< -o $@

run: $(target)
	./$(target)

zip: $(pkgFiles)
	rm -rf ../$(pkgName) ../$(pkgName).zip
	mkdir ../$(pkgName)
	cp -r $(pkgFiles) ../$(pkgName)
	cd ..;  zip -r $(pkgName).zip $(pkgName); rm -rf $(pkgName)

clean:
	rm -f *.o *~ framework make.depend

dosify:
	unix2dos $(src) $(headers) $(extras)

make.depend: 
	g++ -MM $(CXXFLAGS)  $(src) > make.depend

include make.depend
