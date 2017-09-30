.EXPORT_ALL_VARIABLES:

.PHONY: clean all

BIN_DIR    = ../bin
LIB_DIR    = ../lib
COMMON_DIR = ../common
MED_DIR    = ./med
MBEVTS_DIR = ../mbevts
MED2R_DIR = ../MedToRoot

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

LIBS      = -lm $(ROOTLIBS) -L$(LIB_DIR) -lCommandLineInterface -lMedToRoot

AR        = ar
CC        = $(shell root-config --cc)
CPP       = $(shell root-config --cxx)

CFLAGS    = -O2 -pipe -Wall -W -Woverloaded-virtual $(ROOTCFLAGS) -fPIC

INCLUDES  = -I$(MED_DIR)/inc -I$(COMMON_DIR) -I$(MBEVTS_DIR) -I$(MED2R_DIR) -I./
LFLAGS    = -O2 -g

O_FILES = Calibration.o \
		$(MBEVTS_DIR)/mbevts.o \
		$(COMMON_DIR)/CommandLineInterface.o

all: TreeBuilder NeSort

TreeBuilder: TreeBuilder.cc $(O_FILES) TreeBuilderDictionary.o
	$(CPP) $(CFLAGS) $(INCLUDES) TreeBuilder.cc $(O_FILES) TreeBuilderDictionary.o $(LIBS) -o $@
	$(AR) cru $(LIB_DIR)/libTreeBuilder.a $(O_FILES) TreeBuilderDictionary.o
	cp TreeBuilder $(BIN_DIR)/

NeSort: NeSort.cc $(O_FILES) NeSortDictionary.o
	$(CPP) $(CFLAGS) $(INCLUDES) NeSort.cc $(O_FILES) NeSortDictionary.o $(LIBS) -o $@
	$(AR) cru $(LIB_DIR)/libNeSort.a $(O_FILES) NeSortDictionary.o
	cp NeSort $(BIN_DIR)/

$(COMMON_DIR)/CommandLineInterface.o:
	@cd $(COMMON_DIR); make

%.o: %.cc %.hh
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@


#---- Root stuff

DEPENDENCIES = Calibration.hh \
			$(MED2R_DIR)/BuiltEvent.hh \
			$(MED2R_DIR)/Datas.hh \
			$(MED2R_DIR)/Modules.hh \
			$(MED2R_DIR)/SubEvents.hh \
			$(MBEVTS_DIR)/mbevts.hh \
			RootLinkDef.h

TreeBuilderDictionary.cc: $(DEPENDENCIES)
	rm -f TreeBuilderDictionary.cc TreeBuilderDictionary$(DICTEXT)
	$(ROOTDICT) -f TreeBuilderDictionary.cc -c $(INCLUDES) $(DEPENDENCIES)
	cp TreeBuilderDictionary$(DICTEXT) $(LIB_DIR)/

TreeBuilderDictionary.o: TreeBuilderDictionary.cc TreeBuilderDictionary$(DICTEXT)
	$(CPP) -fPIC $(CFLAGS) $(INCLUDES) -c $<

NeSortDictionary.cc: $(DEPENDENCIES)
	rm -f NeSortDictionary.cc NeSortDictionary$(DICTEXT)
	$(ROOTDICT) -f NeSortDictionary.cc -c $(INCLUDES) $(DEPENDENCIES)
	cp NeSortDictionary$(DICTEXT) $(LIB_DIR)/

NeSortDictionary.o: NeSortDictionary.cc NeSortDictionary$(DICTEXT)
	$(CPP) -fPIC $(CFLAGS) $(INCLUDES) -c $<

#--------------------------------------------------
clean:
	rm -f TreeBuilder NeSort *.o $(MED_DIR)/*.o $(MBEVTS_DIR)/*.o
	rm -f TreeBuilderDictionary.cc TreeBuilderDictionary$(DICTEXT) 
	rm -f NeSortDictionary.cc NeSortDictionary$(DICTEXT) 
	@cd $(COMMON_DIR); make clean
