//
// Created by bcub3d-desktop on 25/3/20.
//

#ifndef OPENGLMAP_BUFFERUTIL_H
#define OPENGLMAP_BUFFERUTIL_H

// Standard Includes
#include <vector>


class BufferUtil {
public:
    /* Constructor */
    BufferUtil();

    /* Functions */
    static int findFirstIndexLessThan(std::vector<double> cb, double value);
    static int findFirstIndexGreaterThan(std::vector<double> cb, double value);


};


#endif //OPENGLMAP_BUFFERUTIL_H
