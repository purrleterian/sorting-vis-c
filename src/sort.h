#ifndef SORT_H
#define SORT_H

#include "bars.h"


extern int selection_sort_thread(void *data);
extern int bubble_sort_thread(void *data);
extern int quick_sort_thread(void *data);

extern int comp;
extern int atr;

#endif
