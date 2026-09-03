#include "sort.h"
#include "bars.h"

int sort_step = 0;
void selection_sort(Bars *bars, int i) {

    printf("Sort step: %d\n", i);
    if (sort_step < bars->total) {
        int min_i = i;
        for (int j = i + 1; j < bars->total; j++) {
            if (bars->bar_n[j] < bars->bar_n[min_i]) {
                min_i = j;
            }

        }

        float temp = bars->bar_n[min_i];

        bars->bar_n[min_i] = bars->bar_n[i];
        bars->bar_n[i] = temp;

        set_bar_height(bars);
    }

    printf("[ ");
    for (int i = 0; i < bars->total; i++) {
        printf("%.4f ", bars->bar_n[i]);
    }
    printf("]\n");
}
