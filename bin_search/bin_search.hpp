#pragma once

int* LowerBound(int* first, int* last, int val) {
    last--;
    while (last - first > 1) {
        int middle = (last - first) / 2;
        if (*(first + middle) > val) {
            last = first + middle;
            continue;
        }
        if (*(first + middle) < val) {
            first = first + middle;
            continue;
        }
        if (*(first + middle) >= val) {
            return first + middle;
        }
    }
    return last;
}