#ifndef BDSOutputFormat_h
#define BDSOutputFormat_h 

#include "BDSTypeSafeEnum.hh"

struct BDSOutputFormatDef {
  enum type {
    _ASCII = 0,
    _ROOT = 1,
    _COMBINED = 2
  };
};

typedef BDSTypeSafeEnum<BDSOutputFormatDef,int> BDSOutputFormat;

#endif
