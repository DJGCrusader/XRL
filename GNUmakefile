# Library Directories
LIBDIRS += -L /usr/lib -L /usr/local/lib/

# Static Libraries
STATICLIBS += -lEposCmd

# Include Directories
#INCLUDEDIRS = -I./

all:: squat

squat: src/squat.cc src/RoboteqDevice.cc include/RoboteqDevice.h 
	g++ $(INCLUDEDIRS) src/RoboteqDevice.cc -ldl -o $@ $< -lm $(LIBDIRS) $(STATICLIBS) -std=c++11

sample: src/sample.cc src/RoboteqDevice.cc include/RoboteqDevice.h 
	g++ $(INCLUDEDIRS) src/RoboteqDevice.cc -ldl -o $@ $< -lm $(LIBDIRS) $(STATICLIBS) -std=c++11

clean::
	rm -f build/*.o sample squat

.PHONY: clean
