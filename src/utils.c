#include "main.h"
#include "utils.h"

float map(float value, float istart, float istop, float ostart,
                float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}
