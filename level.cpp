
#include "level.h"
#define OFFSET  4.45

level::level(const char* mapname,
        unsigned int mapID,
        vector<collisionplane>& cplane,
        vector<vector3d>& spawnpoints)
{

    mesh = mapID;
    Map_collision_planes = cplane;
    spawnPoints = spawnpoints;

/*    for(int i=0; i<Map_collision_planes.size();i++)
    {
        if(Map_collision_planes[i].id == 200)
        {
            spawnPoints.push_back(Map_collision_planes[i].p[0]);
            Map_collision_planes.erase(Map_collision_planes.begin()+i);
        }

    }
*/

    name = mapname;
}

level::~level()
{

}

void level::update()
{

}
void level::show()
{
    glPushMatrix();
        glTranslatef(0,OFFSET,0);
        glCallList(mesh);
    glPopMatrix();
}

vector<vector3d>& level::getSpawnPoints()
{
    return spawnPoints;
}

vector<collisionplane>& level::getCollisionPlanes()
{
    return Map_collision_planes;
}


vector3d level::getRandomSpawnPoint()
{
    int i=rand()%spawnPoints.size();
    return spawnPoints[i];
}
