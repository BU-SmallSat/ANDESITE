// Header guard
#ifndef ANDESITEWSN_H
#define ANDESITEWSN_H

// ========
// Includes
// ========

#include "AndesiteCollect.h"
#include "AndesiteData.h"
#include "AndesiteOrbit.h"
#include "AndesiteFile.h"
#include "AndesiteRadio.h"
#include "libandesite.h"



// =======
// Classes
// =======

class AndesiteWSN {
  public:
    AndesiteWSN();
    int init();
    boolean isScienceMode();
    int scienceMode();
    boolean isTransferMode();
    int transferMode();
    void wait();
    
 protected:
    unsigned long _science; 
    unsigned long _transfer; 
    unsigned long _magfreq; 
    unsigned long _ggfreq; 
};

#endif
