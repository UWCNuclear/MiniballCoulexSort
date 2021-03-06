.EXPORT_ALL_VARIABLES:

.PHONY: clean all

BIN_DIR    = ../bin
LIB_DIR    = ../lib
COMMON_DIR = ../common
MED_DIR    = ./med
MBEVTS_DIR = ../mbevts

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTINC      := $(shell root-config --incdir)
ROOTVER      := $(shell root-config --version | head -c1)

ifeq ($(ROOTVER),5)
	ROOTDICT  := rootcint
	DICTEXT   := .h
else
	ROOTDICT  := rootcling
	DICTEXT   := _rdict.pcm
endif

LIBS      = -lm $(ROOTLIBS) -L$(LIB_DIR) -lCommandLineInterface

AR        = ar
CC        = $(shell root-config --cc)
CPP       = $(shell root-config --cxx)

CFLAGS    = -O2 -pipe -Wall -W -Woverloaded-virtual $(ROOTCFLAGS) -fPIC

INCLUDES  = -I$(MED_DIR)/inc -I$(COMMON_DIR) -I$(MBEVTS_DIR) -I./
LFLAGS    = -O2 -g

PLATFORM:=$(shell uname)
ifeq ($(PLATFORM),Darwin)
SHAREDSWITCH = -Qunused-arguments -shared -undefined dynamic_lookup -dynamiclib -Wl,-install_name,'@executable_path/../lib/'# NO ENDING SPACE
else
export __LINUX__:= 1
LINKFLAGS_PREFIX += -Wl,--no-as-needed
SHAREDSWITCH = -shared -Wl,-soname,# NO ENDING SPACE
endif

O_FILES = BuiltEvent.o \
		Datas.o \
        EventBuffer.o \
        EventBuilder.o \
        GlobalSettings.o \
        Modules.o \
        SubEvents.o \
        UnpackedEvent.o \
        $(MED_DIR)/mbsio.o \
        $(MED_DIR)/byte_order.o

all: MKDIR_P MedToRoot

MKDIR_P:
	mkdir -p $(BIN_DIR) $(LIB_DIR)

MedToRoot: MedToRoot.cc $(O_FILES) MedToRootDictionary.o
	$(CPP) $(CFLAGS) $(INCLUDES) MedToRoot.cc $(O_FILES) MedToRootDictionary.o $(LIBS) -o $@
	$(AR) cru $(LIB_DIR)/libMedToRoot.so $(O_FILES) MedToRootDictionary.o
	cp MedToRoot $(BIN_DIR)/


$(MED_DIR)/mbsio.o: $(MED_DIR)/src/mbsio.c $(MED_DIR)/inc/mbsio.h
	$(CC) -I$(ROOTINC) $(INCLUDES) -c $< -o $@

$(MED_DIR)/byte_order.o: $(MED_DIR)/src/byte_order.c $(MED_DIR)/inc/byte_order.h 
	$(CC) -I$(ROOTINC) $(INCLUDES) -c $< -o $@

$(COMMON_DIR)/CommandLineInterface.o:
	@cd $(COMMON_DIR); make

%.o: %.cc %.hh
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@


#---- Root stuff

DEPENDENCIES = BuiltEvent.hh \
			Datas.hh \
			EventBuffer.hh \
			EventBuilder.hh \
			GlobalSettings.hh \
			Modules.hh \
			SubEvents.hh \
			UnpackedEvent.hh \
			RootLinkDef.h

MedToRootDictionary.cc: $(DEPENDENCIES)
	$(ROOTDICT) -f $@ -c $(INCLUDES) $(DEPENDENCIES)
	cp $(basename $@)$(DICTEXT) $(LIB_DIR)/
	cp $(basename $@)$(DICTEXT) $(BIN_DIR)/

MedToRootDictionary.o: MedToRootDictionary.cc MedToRootDictionary$(DICTEXT)
	$(CPP) -fPIC $(CFLAGS) $(INCLUDES) -c $<

#--------------------------------------------------
clean:
	rm -f MedToRoot *.o $(MED_DIR)/*.o $(MBEVTS_DIR)/*.o
	rm -f MedToRootDictionary.cc MedToRootDictionary$(DICTEXT)
	rm -f $(LIB_DIR)/MedToRootDictionary$(DICTEXT)

clean-dict:
	rm -f MedToRootDictionary.cc MedToRootDictionary$(DICTEXT)
	rm -f F/MedToRootDictionary$(DICTEXT)
