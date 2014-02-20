#ifndef WEAPON_H
#define WEAPON_H_
#include <vector>
#include "gl/gl.h"
#include "gl/glu.h"
#include "SDL/SDL.h"
#include "vector3d.h"
#include <iostream>
#include <fstream>
#include <cstdlib>


#define AUTOMATIC   1
#define TEST_NORMAL_FRAME   1
#define TEST_FIRE_FRAME     16
#define TEST_RELOAD_FRAME   38

#define TEST_10_AMMO    10
#define TEST_30_TOTAL_AMMO  30
using namespace std;

class weapon
{


    // frames of all animation
    vector<unsigned int> frames;

    // out the weapon looks from the outside, different from the one you'll be holding in your hand
    unsigned int outerview;

    // for frames
    // 0,1 normal
    // 2,3,4 fire_animation
    // 5,6,7,8 reload_animation

    // the starting index of each animation withing the vector frames
    unsigned int normal_animation;

    unsigned int fire_animation;

    unsigned int reload_animation;

    // these are like two temp values
    vector3d position_expected;
    vector3d rotation_expected;

    // when you are not aiming
    vector3d position;
    vector3d rotation;

    // when aiming
    vector3d aimposition;
    vector3d aimrotation;

    // current position
    vector3d curpos;
    vector3d currot;

    // some "error bars" when shooting
    float precision, aimprecision;
    int damage;

    // flags for the gun
    bool isaim, isreloading, isautomatic, isfired, istest;

    unsigned int maxBulletsInMag;
    unsigned int bulletsInMag;
    unsigned int allbullets;

    unsigned int lastshot;
    unsigned int speed;

    unsigned int curframe;
    unsigned curmode; // 1-normal, 2-fire, 3-reload
    string name;



    enum{
    NORMAL_MODE = 1,
    FIRE_MODE,
    RELOAD_MODE
    };

    public:
        weapon(vector<unsigned int>& anim,
                unsigned int outside_view,
                unsigned int normal_anim,
                unsigned int fire_anim,
                unsigned int reload_anim,
                vector3d pos,
                vector3d rot,
                vector3d aim_pos,
                vector3d aim_rot,
                float precision,
                float aim_precision,
                int damage,
                unsigned int max_bullets,
                unsigned int all_bullets,
                unsigned int speed,
                const char* name,
                bool isauto);

        ~weapon();

        void update();
        void show();

        // if I shot a bullet or not
        bool fire(vector3d& bullet_direction,
                vector3d cam_direction);

        // for semi automatic
        void stopfire();

        // reload function
        void reload();
        void aim();

        void test();

        void addBullets(unsigned int num);
        void setBullets(unsigned int num);

        int getDamage();
        int getAmmo();
        int getAllAmmo();

        // get weapon name
        string getName();

        vector<unsigned int>& getAnimation();
        bool isAimed();
        unsigned int getOuterView();


};

#endif
