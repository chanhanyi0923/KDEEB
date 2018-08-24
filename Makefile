# Declaration of variables

#CC = g++ -std=c++11 -O3 -fopenmp -Wall -Wextra -Wno-reorder
CC = g++ -std=c++11 -O3 -fopenmp -Wall -Wextra -Wno-reorder
#CC = g++ -std=c++11 -g -Wall -Wextra -Wno-reorder
CC_FLAGS = -L/usr/lib/nvidia-396 -lOpenGL -lEGL
#CC_FLAGS = -L/usr/lib/nvidia-396 -lOpenGL -lEGL -lpistache -lpthread

# File names
EXEC = main
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
 
# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(CC_FLAGS)

# To obtain object files
%.o: %.cpp
	$(CC) -c $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)

