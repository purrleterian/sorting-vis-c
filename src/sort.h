#ifndef SORT_H
#define SORT_H

#include "bars.h"

extern int sort_step;
// maybe use a void pointer later to pass a sort function callback as parameter
extern void selection_sort(Bars *bars, int i);

#endif
