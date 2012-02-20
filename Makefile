CPPCOMPILER=g++
CCOMPILER=gcc
#CPPCOMPILER=i586-mingw32msvc-g++

FLAGS=-Wall -g -O3
LIBRARIES=-lpthread
MODULES=obj/HTTPRequest.o obj/main.o obj/Server.o obj/SocketUtils.o obj/HTTPServer.o \
	obj/CookieManager.o obj/FileLister.o
STATIC=obj/static/login.html.o obj/static/files.html.o

all: $(MODULES) $(STATIC)
	$(CPPCOMPILER) $(MODULES) $(STATIC) $(LIBRARIES) -o httpserver

obj/main.o : main.cpp
	@mkdir -p obj
	$(CPPCOMPILER) -c $(FLAGS) $< -o $@ 

obj/static/%.html.o : %.html
	@mkdir -p obj/static
	file2obj $< $(subst .,_,$<) > $<.c.temp
	$(CCOMPILER) -c $(FLAGS) -x c $<.c.temp -o $@

obj/%.o : %.cpp %.h
	@mkdir -p obj
	$(CPPCOMPILER) -c $(FLAGS) $< -o $@ 

clean:
	rm -rf $(STATIC) $(MODULES) httpserver
