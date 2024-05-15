CXX = g++
CXXFLAGS = -Wall 

p: mytest.cpp dealer.o 
	$(CXX) $(CXXFLAGS) mytest.cpp dealer.o -o proj4

d: driver.cpp dealer.o 
	$(CXX) $(CXXFLAGS) driver.cpp dealer.o -o proj4

dealer.o: dealer.h dealer.cpp
	$(CXX) $(CXXFLAGS) -c dealer.cpp

clean:
	rm *.o*
	rm *~ 

v:
	valgrind ./proj4

r:
	./proj4

b:
	gdb ./proj4
