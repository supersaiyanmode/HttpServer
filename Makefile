CPPCOMPILER=g++
#CPPCOMPILER=i586-mingw32msvc-g++

FLAGS=-Wall -g -O3
LIBRARIES=-lpthread
MODULES=obj/HTTPRequest.o obj/main.o obj/Server.o obj/SocketUtils.o obj/HTTPServer.o

all: $(MODULES)
	$(CPPCOMPILER) $(MODULES) $(LIBRARIES) -o httpserver

obj/main.o : main.cpp
	@mkdir -p obj
	$(CPPCOMPILER) -c $(FLAGS) $< -o $@ 
obj/%.o : %.cpp %.h
	@mkdir -p obj
	$(CPPCOMPILER) -c $(FLAGS) $< -o $@ 
clean:
	rm -rf $(MODULES) httpserver
