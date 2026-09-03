#include "main.h"
#include "utils.h"


void map(float *value, float istart, float istop, float ostart,
                float ostop) {
    *value = ostart + (ostop - ostart) * ((*value - istart) / (istop - istart));
}
