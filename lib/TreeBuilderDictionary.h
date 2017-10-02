/********************************************************************
* TreeBuilderDictionary.h
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************************/
#ifdef __CINT__
#error TreeBuilderDictionary.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define G__ANSIHEADER
#define G__DICTIONARY
#define G__PRIVATE_GVALUE
#include "G__ci.h"
#include "FastAllocString.h"
extern "C" {
extern void G__cpp_setup_tagtableTreeBuilderDictionary();
extern void G__cpp_setup_inheritanceTreeBuilderDictionary();
extern void G__cpp_setup_typetableTreeBuilderDictionary();
extern void G__cpp_setup_memvarTreeBuilderDictionary();
extern void G__cpp_setup_globalTreeBuilderDictionary();
extern void G__cpp_setup_memfuncTreeBuilderDictionary();
extern void G__cpp_setup_funcTreeBuilderDictionary();
extern void G__set_cpp_environmentTreeBuilderDictionary();
}


#include "TObject.h"
#include "TMemberInspector.h"
#include "Calibration.hh"
#include "../MedToRoot/BuiltEvent.hh"
#include "../MedToRoot/Datas.hh"
#include "../MedToRoot/Modules.hh"
#include "../MedToRoot/SubEvents.hh"
#include "../mbevts/mbevts.hh"
#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__TreeBuilderDictionaryLN_TClass;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_TBuffer;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_TMemberInspector;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_TObject;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEunsignedsPintcOallocatorlEunsignedsPintgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEfloatcOallocatorlEfloatgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEdoublecOallocatorlEdoublegRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_string;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlETStringcOallocatorlETStringgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlETStringcOallocatorlETStringgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_TRandom;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEstringcOallocatorlEstringgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEstringcOallocatorlEstringgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEintcOallocatorlEintgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEintcOallocatorlEintgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_Calibration;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlElongsPdoublecOallocatorlElongsPdoublegRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlElongsPdoublecOallocatorlElongsPdoublegRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEvectorlElongsPdoublecOallocatorlElongsPdoublegRsPgRcOallocatorlEvectorlElongsPdoublecOallocatorlElongsPdoublegRsPgRsPgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEvectorlElongsPdoublecOallocatorlElongsPdoublegRsPgRcOallocatorlEvectorlElongsPdoublecOallocatorlElongsPdoublegRsPgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEvectorlEdoublecOallocatorlEdoublegRsPgRcOallocatorlEvectorlEdoublecOallocatorlEdoublegRsPgRsPgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEvectorlEdoublecOallocatorlEdoublegRsPgRcOallocatorlEvectorlEdoublecOallocatorlEdoublegRsPgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_iteratorlEbidirectional_iterator_tagcOTObjectmUcOlongcOconstsPTObjectmUmUcOconstsPTObjectmUaNgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_maplEstringcOTObjArraymUcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOTObjArraymUgRsPgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEAdcDatacOallocatorlEAdcDatagRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEAdcDatacOallocatorlEAdcDatagRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEDgfDatacOallocatorlEDgfDatagRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEDgfDatacOallocatorlEDgfDatagRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEDgfSubEventcOallocatorlEDgfSubEventgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEDgfSubEventcOallocatorlEDgfSubEventgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEAdcSubEventcOallocatorlEAdcSubEventgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEAdcSubEventcOallocatorlEAdcSubEventgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEPatternUnitSubEventcOallocatorlEPatternUnitSubEventgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEPatternUnitSubEventcOallocatorlEPatternUnitSubEventgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEScalerSubEventcOallocatorlEScalerSubEventgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEScalerSubEventcOallocatorlEScalerSubEventgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEDgfScalerSubEventcOallocatorlEDgfScalerSubEventgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEDgfScalerSubEventcOallocatorlEDgfScalerSubEventgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_vectorlEBraggChamberSubEventcOallocatorlEBraggChamberSubEventgRsPgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_reverse_iteratorlEvectorlEBraggChamberSubEventcOallocatorlEBraggChamberSubEventgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TreeBuilderDictionaryLN_mbevts;

/* STUB derived class for protected member access */
