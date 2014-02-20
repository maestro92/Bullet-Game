#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <iostream>
#include "gl/gl.h"
#include "gl/glu.h"
#include <string>
#include "collisionplane.h"
#include "vector3d.h"
#include "SDL\SDL.h"

using namespace std;
class level
{

    unsigned int mesh;

    vector<collisionplane> Map_collision_planes;
    vector<vector3d> spawnPoints;
    string name;

    public:
        level(const char* mapname,
                unsigned int mapID,
                vector<collisionplane>& cplane,
                vector<vector3d>& spawnpoints);
        ~level();

        void update();
        void show();
        vector<collisionplane>& getCollisionPlanes();
        vector<vector3d>& getSpawnPoints();

        string& getName();

        void change(const char* mapname,
                unsigned int mapID,
                vector<collisionplane>& cplane,
                vector<vector3d>& spawnpoints);

        vector3d getRandomSpawnPoint();
};


#endif
