SHELL = /bin/sh
CC    = g++

CPPFLAGS       = -g -Wall -Iinclude 
OLDFLAGS	= -std=c++11
CFLAGS       = $(CPPFLAGS) 
LDLIBS		 = -lutil
TARGET  = alarm-controller
SOURCES = $(shell echo src/*.cpp)
HEADERS = $(shell echo include/*.h)
OBJECTS = $(SOURCES:.cpp=.o)
MAIN	= $(shell echo main/*.cpp)
MAINOBJ	= $(MAIN:.cpp=.o)
DEBUGFLAGS = -g
 
all: $(TARGET)
 

clean:
	-echo "clean"
	-rm -f $(OBJECTS)
	-rm -f gmon.out
	-rm -f main/*.o
	-rm -f $(TARGET)
 
distclean: clean
	-rm -f $(TARGET)
 
gitadd:
	-git add src/*.cpp
	-git add include/*.h
	-git add main/*.cpp
	-git add Makefile
 
.SECONDARY: $(OBJECTS) $(MAINOBJ)
.PHONY : all install uninstall clean distclean gitadd

.SECONDEXPANSION:

$(foreach TGT, $(TARGET), $(eval $(TGT)_HEADERS = $(filter $(shell $(CC) $(CFLAGS) -MM main/$(TGT).cpp | sed s/.*://), $(HEADERS))))
$(foreach TGT, $(TARGET), $(eval $(TGT)_OBJECTS = $(filter $(subst include, src, $($(TGT)_HEADERS:.h=.o)), $(OBJECTS))))
$(foreach OBJ,$(OBJECTS),$(eval $(OBJ)_HEADERS = $(filter $(shell $(CC) $(CFLAGS) -MM $(OBJ:.o=.cpp) | sed s/.*://), $(HEADERS))))
$(foreach OBJ,$(OBJECTS),$(eval $(OBJ)_OBJECTS = $(filter-out $(OBJ), $(filter $(subst include, src, $($(OBJ)_HEADERS:.h=.o)), $(OBJECTS)))))


%:   main/%.o $$($$@_OBJECTS)  $$($$@_HEADERS)
	$(CC) $(CFLAGS) -o $@ $($@_OBJECTS) $< $(LDLIBS)
 
%.o: %.cpp $$($$@_OBJECTS) $$($$@_HEADERS)
	$(CC) $(CFLAGS) $(LDLIBS) -c -o $@ $<


