#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include "functions.h"
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "camera.h"
#include <vector>
#include "level.h"
#include "objloader.h"
#include "player.h"



using namespace std;

// need to include this to integrate Bullet physics in application
#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btDefaultSoftBodySolver.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>

#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include "GlutDemoApplication.h"


class game
{   unsigned int gunID1;
    unsigned int cubeID;
    unsigned int tankID;
    objloader obj;

    vector<level*> levels;

    vector<weapon*> weapons;

//    vector<zombie*> zombies;

 //   vector<enemy*> enemies;

    vector<unsigned int> enemyframe;

//    item The_Item;
    // player


    int gunID;

    bool isOnline;
    bool Softbody_mode;


//    network* net;

    Uint32 Actual_Start;
    uint32_t count;
    bool decrease;
    // if the game is done running or not
    bool running;

    void update();



    unsigned int loadTexture(const char filename);

    void loadAnimation(vector<unsigned int>& anim,
                        string filename,
                        unsigned int frame_Num);



    public:

        btTransform tr;

        btRigidBody*    m_carChassis;


        /// a template class to replace vector, should be the same
        btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

        /// tuning is like engine tuning, i suppose...
        /// it has suspension, friction slip , damping settings...
        btRaycastVehicle::btVehicleTuning   m_tuning;

        /// provides interface for between vehicle simulation and raycasting
        btVehicleRaycaster* m_vehicleRaycaster;

        /// rayCast vehicle, very special constrait that turn a rigidbody into a vehicle
        btRaycastVehicle*   m_vehicle;
        btCollisionShape*   m_wheelShape;

        float       m_cameraHeiht;
        float       m_minCameraDistance;
        float       m_maxCameraDistance;

        btScalar    m_defaultContactProcessingThreshold;

        btRigidBody* CreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape);

        void Create_Vehicle();

        void clientResetScene();

        void Delete_Vehicle();




        player* player1;


        /* constructor */
        game();

        game(float angle);
        /* destructor */
        ~game();

        void start();


    void show();

// GLSL portion
    unsigned int vs, fs, programID;
    unsigned int myImg, myImg2;

    bool first_burst;

    unsigned int brick;

    void loadFile(const char* fn, std::string& str);
    unsigned int loadShader(string & source, unsigned int mode);
    void initShader(const char* vname,const char* fname);
    void cleanShader();
    unsigned int loadTexture(const char* filename);



    /* BasicDemo */
	GL_ShapeDrawer*	m_shapeDrawer;
	bool			m_enableshadows;
	btVector3		m_sundirection;


	bool	setTexturing(bool enable) { return(m_shapeDrawer->enableTexture(enable)); }
	bool	setShadows(bool enable)	{ bool p=m_enableshadows;m_enableshadows=enable;return(p); }

    void initPhysics_BasicDemo();
    void init_Lighting();
    void render_BasicDemo();
    void renderscene_BasicDemo(int pass);









};

#endif
