//
// Created by bcub3d-desktop on 25/3/20.
//

#include <iostream>
#include "BufferUtil.h"

BufferUtil::BufferUtil() {

}

int BufferUtil::findFirstIndexLessThan(std::vector<double> cb, double value) {
    int index = -1;
    for(unsigned int i=0; i < cb.size(); i++) {
        index += 1;
        if(cb[index] < value) {
            break;
        }
    }

    return index;
}

int BufferUtil::findFirstIndexGreaterThan(std::vector<double> cb, double value) {
    int index = -1;
    for(unsigned int i=0; i < cb.size(); i++) {
        index += 1;
        if (cb[index] > value) {
            break;
        }
    }

    return index;
}
