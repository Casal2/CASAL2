
ifeq ($(MINGW),TRUE) # MS-Windows MINGW compilation
  CREATE_ICON=windres -i simCASAL.rc -o icon.o
  MINGW_OPTIONS=-lgw32c -lwsock32 -Lc:/winnt/system32 -static icon.o
else
  MINGW_OPTIONS=
  CREATE_ICON=
endif

DEVELOPMENT_PATH = ../../../src
DEPENDENCIES =  ${DEVELOPMENT_PATH}/GetPot.h makefile version.h

COMPILER=g++

FILE_NAME = simCASAL

all: VERSION $(FILE_NAME)

clean:
	rm -f *.o
	rm -f *.exe
	rm -f $(FILE_NAME)

# Create list of IDs for casal version number
VERSION:
	svnversion -q 

$(FILE_NAME).o: $(FILE_NAME).cpp $(DEPENDENCIES)
	$(COMPILER) -c $(FILE_NAME).cpp \
	-I$(DEVELOPMENT_PATH)

$(FILE_NAME): $(FILE_NAME).o makefile
	$(CREATE_ICON)
	$(COMPILER) $(FILE_NAME).o -o $(FILE_NAME) \
	$(MINGW_OPTIONS)
