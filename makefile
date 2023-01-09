R_DIR = /usr/share/R/include

CPPFLAGS = -I $(R_DIR) -std=c++11 -Wall -g

OBJECTS = Util.o Feature.o Model.o Feature_Selection.o
SOBJECTS = $(OBJECTS) r_wrapper.o
HEADERS = Util.hpp Feature.hpp Model.hpp Feature_Selection.hpp

default: classifier r_wrapper

classifier: $(OBJECTS)
	g++ $(CPPFLAGS) -shared -o $@ $(OBJECTS)

r_wrapper: $(SOBJECTS)
	gcc -I $(R_DIR) -shared -o lib_$@.so $(SOBJECTS)

%.o: %.cpp $(HEADERS)
	g++ $(CPPFLAGS) -fPIC -c $< -o $@

clean:
	rm *.o
	rm *.so
	rm classifier
