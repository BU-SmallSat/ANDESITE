// Header guard
#ifndef ANDESITEFILE_H
#define ANDESITEFILE_H

// ========
// Includes
// ========

/* #include "Arduino.h" */
#include "libandesite.h"
#include "AndesiteData.h"
#include "AndesiteOrbit.h"
#include "AndesiteRadio.h"



// =======
// Classes
// =======

class AndesiteFile {
 public:
    AndesiteFile();
    boolean       init();
    
    boolean       send();
    void          done();
    
    void          write(String data);
    void          close();
    boolean       status();
    
    String        name();
    unsigned int  orbit();
    void          set();
    
    unsigned int  getLines();
    void          setLines(unsigned int num);
    unsigned long size();
    
 protected:
    File         _handle;
    String       _file;
    unsigned int _lines;
    unsigned int _limit;
};



// =======
// Globals
// =======

extern AndesiteFile _File;

#endif
