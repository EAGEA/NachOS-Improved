#ifndef ALLOCAT_H
#define ALLOCAT_H


#include "system.h"

#include <stdio.h>
#include <cstdlib>


void operator delete(void *p) ;

void operator delete[](void *p) ;

void *operator new(size_t s) ;

void *operator new[](size_t s) ;


#endif
