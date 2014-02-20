#ifndef TEXT_H
#define TEXT_H
#include <cstring>
#include <vector>
#include "objloader.h"

using namespace std;
class text
{
    // width and height of one character
    float width, height;
    vector<unsigned int> characters;
    public:
        text(float, float, vector<unsigned int>&);
        /* position, rotation, */
        void drawText(coordinate pos, coordinate rot, const char* tex);

};

#endif
