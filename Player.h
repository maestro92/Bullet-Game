#ifndef PLAYER3D_H
#define PLAYER3D_H

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cstdlib>
#include "weapon.h"
#include "vector3d.h"
#include "collisionsphere.h"
#include "collision.h"
#include "collisionplane.h"
#include <string>
#include <vector>
#include "camera.h"


#define PLAYER_SPRINT_SPEED 0.8
#define PLAYER_WALK_SPEED 0.4
using namespace std;

class player
{
    string name;

    // one collisionsphere for collision detection
    collisionsphere player_sphere;
    int health;

    vector3d gravity;

    // whichever way the player is going.. Jumping or walking
    vector3d velocity;
    // pos = pos + direction
    // direction = direction + dorce

    // flags
    bool isground, iscollision, issprint;
    float sprintspeed, normalspeed, lookspeed;
    float energy; // for energy
    int points;

    vector<weapon*> weapons;
    int cur_weapon;
    bool isWeapon;
    int headPosition;
    bool head_Move_Up;


    int Net_ID;
    bool isready;


    public:
        camera cam;

        // constructor
        player(string n, collisionsphere sphere,
                float sprints,
                float normals,
                float looks,
                weapon* default_weapon);
        ~player();

        void update(vector<collisionplane>& PlaneList);
        void show();

        /* weapons */

        void addWeapon(weapon* w);
        weapon* getCurWeapon();
        void changeWeapon(unsigned int num);
        // up and down is using the mouse scrolling
        void changeWeaponUp();
        void changeWeaponDown();
        weapon* getCurrentWeapon();
        void haveWeapon(bool b);



        void setjump();
        void setSprint();
        void stopSprint();

        collisionsphere getCollisionSphere();


        string getName();
//        camera* getCamera();
        void setPosition(vector3d pos);
        vector3d getPosition();
        /* health */
        void decreaseHealth(int num);
        int getHealth();
        void setHealth(int h);
        void addHealth(int h);

        void setSprint(bool b);
        bool isSprinting();
        void addPoints(int num);
        int getPoints();
        int getNumWeapons();
        void holdWeapon(bool);

        void setNetID(int i);
        int getNetID();

        bool isReady();

        int getCurframe();
        vector3d getRotation();
        int getWeaponIndex();
};

#endif
