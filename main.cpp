#include "main.h"
#include "DemoApplication.h"



#include "GLDebugDrawer.h"
#include <stdio.h> //printf debugging

#include "GL_ShapeDrawer.h"

#include "GlutStuff.h"



#define PLANE_SIZE  1000
#define SPHERE_VELOCITY  20


#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480





///create 125 (5x5x5) dynamic object
#define ARRAY_SIZE_X 2
#define ARRAY_SIZE_Y 2
#define ARRAY_SIZE_Z 2



///scaling of the objects (0.1 = 20 centimeter boxes )
#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y -5
#define START_POS_Z -3







int rightIndex = 0;
int upIndex = 1;
int forwardIndex = 2;
btVector3 wheelDirectionCS0(0,-1,0);
btVector3 wheelAxleCS(-1,0,0);



const int maxProxies = 32766;
const int maxOverlap = 65535;

///btRaycastVehicle is the interface for the constraint that implements the raycast vehicle
///notice that for higher-quality slow-moving vehicles, another approach might be better
///implementing explicit hinged-wheel constraints with cylinder collision, rather then raycasts
float	gEngineForce = 0.f;
float	gBreakingForce = 0.f;

float	maxEngineForce = 1000.f;//this should be engine/velocity dependent
float	maxBreakingForce = 100.f;

float	gVehicleSteering = 0.f;
float	steeringIncrement = 0.04f;
float	steeringClamp = 0.3f;
float	wheelRadius = 0.5f;
float	wheelWidth = 0.4f;
float	wheelFriction = 1000;//BT_LARGE_FLOAT;
float	suspensionStiffness = 20.f;
float	suspensionDamping = 2.3f;
float	suspensionCompression = 4.4f;
float	rollInfluence = 0.1f;//1.0f;

btScalar suspensionRestLength(0.6);
#define CUBE_HALF_EXTENTS 1






using namespace std;

struct bulletObject
{
    int id;
    float r,g,b;
    bool hit;
    // void* yourOwnStructure; // function
    btRigidBody* body;
    bulletObject(btRigidBody* b, int i, float r0, float g0, float b0) : body(b), id(1)
                            ,r(r0), g(g0), b(b0), hit(false)
    {}
};




camera cam;

// this is to draw a sphere
GLUquadricObj* quad;

// we will create a btDiscreteDynamicsWorld object through polymorphism (as a btDynamicsWorld object pointer)
// src->BulletDynamics->Dynamics

// The entire physics pipeline computation and its data structures
// are represented in Bullet by a dynamics world
// The default dynamics world implementation is the btDiscreteDynamicsWorld
// btDynamicsWorld* world;	//every physical object go to the world
btSoftRigidDynamicsWorld* world;



// used to register a custom collision detection algorithm
btDispatcher* dispatcher;	//what collision algorithm to use?


btCollisionConfiguration* collisionConfig;	//what collision algorithm to use?

// handling collision interface
btBroadphaseInterface* broadphase;	//should Bullet examine every object, or just what close to each other

// Fast and stable rigid body dynamics constraint solver calculator: how much force to apply, etc
btConstraintSolver* solver;					//solve collisions, apply forces, impulses

// a vector to save everything
std::vector<bulletObject*> bodies;


btSoftBodySolver* softbodySolver;




/*
// radius
// position
// mass
// btRigidBody: main class for rigid body objects
btRigidBody* addSphere(float rad,float x,float y,float z,float mass)
{
	btTransform t;	//position and rotation
	t.setIdentity();
	t.setOrigin(btVector3(x,y,z));	//put it to x,y,z coordinates
	btSphereShape* sphere=new btSphereShape(rad);	//it's a sphere, so use sphereshape
	btVector3 inertia(0,0,0);	//inertia is 0,0,0 for static object, else

	// if the body is static
	if(mass!=0.0)
		sphere->calculateLocalInertia(mass,inertia);	//it can be determined by this function (for all kind of shapes)

	btMotionState* motion=new btDefaultMotionState(t);	//set the position (and motion)
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);	//create the constructioninfo, you can create multiple bodies with the same info
	btRigidBody* body=new btRigidBody(info);	//let's create the body itself

	// let the world know there's a sphere
	world->addRigidBody(body);
	bodies.push_back(new bulletObject(body, 0, 1,0,0));	//to be easier to clean, I store them a vector

    // get the last element of the vector
    body->setUserPointer(bodies[bodies.size()-1]);

	return body;
}
*/


btRigidBody* addSphere(float rad,float x,float y,float z,float mass)
{
        btTransform t;
        t.setIdentity();
        t.setOrigin(btVector3(x,y,z));
        btSphereShape* sphere=new btSphereShape(rad);
        btVector3 inertia(0,0,0);
        if(mass!=0.0)
                sphere->calculateLocalInertia(mass,inertia);

        btMotionState* motion=new btDefaultMotionState(t);
        btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);
        btRigidBody* body=new btRigidBody(info);
        world->addRigidBody(body);
        bodies.push_back(new bulletObject(body,0,1.0,0.0,0.0));
        body->setUserPointer(bodies[bodies.size()-1]);
        return body;
}


// radius
// position
// mass
// btRigidBody: main class for rigid body objects
btRigidBody* addCylinder(float cylin_d,float cylin_h,float x, float y,float z,float mass)
{
	btTransform t;	//position and rotation
	t.setIdentity();
	t.setOrigin(btVector3(x,y,z));	//put it to x,y,z coordinates
	btCylinderShape* sphere=new btCylinderShape(btVector3(cylin_d/2.0, cylin_h/2.0, cylin_d/2.0));
	btVector3 inertia(0,0,0);	//inertia is 0,0,0 for static object, else

	// if the body is static
	if(mass!=0.0)
		sphere->calculateLocalInertia(mass,inertia);	//it can be determined by this function (for all kind of shapes)

	btMotionState* motion=new btDefaultMotionState(t);	//set the position (and motion)
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);	//create the constructioninfo, you can create multiple bodies with the same info
	btRigidBody* body=new btRigidBody(info);	//let's create the body itself
	world->addRigidBody(body);	//and let the world know about it
	bodies.push_back(new bulletObject(body, 0, 0,1,0));	//to be easier to clean, I store them a vector

    body->setUserPointer(bodies[bodies.size()-1]);
	return body;
}


btRigidBody* addCone(float d,float h,float x,float y,float z,float mass)
{
	btTransform t;	//position and rotation
	t.setIdentity();
	t.setOrigin(btVector3(x,y,z));	//put it to x,y,z coordinates
	btConeShape* sphere=new btConeShape(d,h);
	btVector3 inertia(0,0,0);	//inertia is 0,0,0 for static object, else

	// if the body is static
	if(mass!=0.0)
		sphere->calculateLocalInertia(mass,inertia);	//it can be determined by this function (for all kind of shapes)

	btMotionState* motion=new btDefaultMotionState(t);	//set the position (and motion)
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);	//create the constructioninfo, you can create multiple bodies with the same info
	btRigidBody* body=new btRigidBody(info);	//let's create the body itself
	world->addRigidBody(body);	//and let the world know about it
	bodies.push_back(new bulletObject(body, 2, 1,0,1));	//to be easier to clean, I store them a vector

    body->setUserPointer(bodies[bodies.size()-1]);
	return body;
}


btRigidBody* addBox(float width,float height,float depth,float x,float y,float z,float mass)
{
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(x,y,z));
    btBoxShape* sphere=new btBoxShape(btVector3(width/2.0,height/2.0,depth/2.0));
    btVector3 inertia(0,0,0);
    if(mass!=0.0)
            sphere->calculateLocalInertia(mass,inertia);

    btMotionState* motion=new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);
    btRigidBody* body=new btRigidBody(info);
    world->addRigidBody(body);
    bodies.push_back(new bulletObject(body, 3, 1,1,0));	//to be easier to clean, I store them a vector

    body->setUserPointer(bodies[bodies.size()-1]);
    return body;
}


// all possible shapes
// http://bulletphysics.org/Bullet/BulletFull/btBroadphaseProxy_8h.html

// rigidBody are like main objects
void renderSphere(bulletObject* b_obj)
{
    btRigidBody* sphere=b_obj->body;
    // check if it's a sphere by checking the shape
    // btRigidBody.h line 248
	if(sphere->getCollisionShape()->getShapeType()!=SPHERE_SHAPE_PROXYTYPE)	//only render, if it's a sphere
		return;

    if(!b_obj->hit)
            glColor3f(b_obj->r,b_obj->g,b_obj->b);
    else
            glColor3f(1,0,0);

	// typecast sphere->getCollisionShape to btSphereShape*
	float r=((btSphereShape*)sphere->getCollisionShape())->getRadius();

	// to get the motionState
	btTransform t;
	sphere->getMotionState()->getWorldTransform(t);	//get the transform
	float mat[16];
	t.getOpenGLMatrix(mat);	//OpenGL matrix stores the rotation and orientation

	// this Matrix multiplication will only apply to this object, not everything else
	glPushMatrix();
		glMultMatrixf(mat);	//multiplying the current matrix with it moves the object in place
		// draw a sphere
		// Glint slices, Glint stacks: resolution
		gluSphere(quad,r,20,20);
	glPopMatrix();
}


// all possible shapes
// http://bulletphysics.org/Bullet/BulletFull/btBroadphaseProxy_8h.html

// rigidBody are like main objects
void renderCylinder(bulletObject* b_obj)
{
    btRigidBody* sphere=b_obj->body;
    // check if it's a sphere by checking the shape
    // btRigidBody.h line 248
	if(sphere->getCollisionShape()->getShapeType()!=CYLINDER_SHAPE_PROXYTYPE)	//only render, if it's a sphere
		return;

    if(!b_obj->hit)
            glColor3f(b_obj->r,b_obj->g,b_obj->b);
    else
            glColor3f(1,0,0);

	// typecast sphere->getCollisionShape to btSphereShape*
	btVector3 extent =((btCylinderShape*)sphere->getCollisionShape())->getHalfExtentsWithoutMargin();

	// to get the motionState
	btTransform t;
	sphere->getMotionState()->getWorldTransform(t);	//get the transform
	float mat[16];
	t.getOpenGLMatrix(mat);	//OpenGL matrix stores the rotation and orientation

	// this Matrix multiplication will only apply to this object, not everything else
	glPushMatrix();
		glMultMatrixf(mat);	//multiplying the current matrix with it moves the object in place
		// draw a sphere

        // always translate then rotate
		glTranslatef(0, extent.y(),0);
		// Glint slices, Glint stacks: resolution
        glRotatef(90, 1, 0, 0);
		// bullet

		// OpenGL considers the center of a cylinder at the bottom so just translate it

		gluCylinder(quad,extent.x(), extent.x(), extent.y()*2.0, 20,20);
	glPopMatrix();
}



// rigidBody are like main objects
void renderCone(bulletObject* b_obj)
{
    btRigidBody* cone=b_obj->body;
    // check if it's a sphere by checking the shape
    // btRigidBody.h line 248
	if(cone->getCollisionShape()->getShapeType()!=CONE_SHAPE_PROXYTYPE)	//only render, if it's a sphere
		return;

    if(!b_obj->hit)
            glColor3f(b_obj->r,b_obj->g,b_obj->b);
    else
            glColor3f(1,0,0);

    float r=((btConeShape*)cone->getCollisionShape())->getRadius();
    float h=((btConeShape*)cone->getCollisionShape())->getHeight();

	// to get the motionState
    btTransform t;
    cone->getMotionState()->getWorldTransform(t);
    float mat[16];
    t.getOpenGLMatrix(mat);
    glPushMatrix();
            glMultMatrixf(mat);     //translation,rotation
            glTranslatef(0,h/2.0,0);
            glRotatef(90,1,0,0);
            gluCylinder(quad,0,r,h,20,20);
    glPopMatrix();
}



void renderBox(bulletObject* b_obj)
{
        btRigidBody* sphere=b_obj->body;
        if(sphere->getCollisionShape()->getShapeType()!=BOX_SHAPE_PROXYTYPE)
                return;

        if(!b_obj->hit)
                glColor3f(b_obj->r,b_obj->g,b_obj->b);
        else
                glColor3f(1,0,0);
        btVector3 extent=((btBoxShape*)sphere->getCollisionShape())->getHalfExtentsWithoutMargin();
        btTransform t;
        sphere->getMotionState()->getWorldTransform(t);
        float mat[16];
        t.getOpenGLMatrix(mat);
        glPushMatrix();
                glMultMatrixf(mat);     //translation,rotation
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                glEnd();
        glPopMatrix();
}





//similar then renderSphere function
void renderPlane(bulletObject* b_obj)
{
    btRigidBody* plane=b_obj->body;
	if(plane->getCollisionShape()->getShapeType()!=STATIC_PLANE_PROXYTYPE)
		return;
    if(!b_obj->hit)
            glColor3f(b_obj->r,b_obj->g,b_obj->b);
    else
            glColor3f(0.8,0.8,0.8);
	btTransform t;
	plane->getMotionState()->getWorldTransform(t);
	float mat[16];
	t.getOpenGLMatrix(mat);
	glPushMatrix();
		glMultMatrixf(mat);	//translation,rotation
		glBegin(GL_QUADS);
			glVertex3f(-PLANE_SIZE,0,PLANE_SIZE);
			glVertex3f(-PLANE_SIZE,0,-PLANE_SIZE);
			glVertex3f(PLANE_SIZE,0,-PLANE_SIZE);
			glVertex3f(PLANE_SIZE,0,PLANE_SIZE);
		glEnd();
	glPopMatrix();
}

// softbody changes shape
// rigid body doesn't change shapes
void renderSoftbody(btSoftBody* b)
{
    // faces
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_TRIANGLES);
    for(int i=0; i<b->m_faces.size(); i++)
    {
        for(int j=0; j<3; j++)
        {
            glVertex3f(b->m_faces[i].m_n[j]->m_x.x(),
                        b->m_faces[i].m_n[j]->m_x.y(),
                        b->m_faces[i].m_n[j]->m_x.z());
        }
    }
    glEnd();

    // lines
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_LINES);
    for(int i=0; i<b->m_links.size(); i++)
    {
        for(int j=0; j<2; j++)
        {
            glVertex3f(b->m_links[i].m_n[j]->m_x.x(),
                        b->m_links[i].m_n[j]->m_x.y(),
                        b->m_links[i].m_n[j]->m_x.z());
        }
    }

    glEnd();
}



game::game(float angle) //init(float angle)
{
    // creates and returns a pointer to a new quadric object
	quad = gluNewQuadric();

    // initialize Glew
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      // Problem: glewInit failed, something is seriously wrong.
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

	// collision configuration contains default setup
	// collisionConfig=new btDefaultCollisionConfiguration();
    collisionConfig=new btSoftBodyRigidBodyCollisionConfiguration();

	// use the default collision dispatcher.
	dispatcher=new btCollisionDispatcher(collisionConfig);

	// broadphase, an algorithm to detect colliding pairs
	broadphase=new btDbvtBroadphase();

    // constraint solver
	solver=new btSequentialImpulseConstraintSolver();

    softbodySolver = new btDefaultSoftBodySolver();

	// building the world with all the configuration we just set
	// world=new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfig);
    world=new btSoftRigidDynamicsWorld(dispatcher,broadphase,solver,collisionConfig, softbodySolver);


	// set gravity
	world->setGravity(btVector3(0,-10,0));

	//similar to glTransfrom
	btTransform t;
	// similar to glLoadIdentity()
	t.setIdentity();
	// setting the origin
	t.setOrigin(btVector3(0,0,0));



    /// makes a inifinite flat surface/plane
    // set the shape we want to use (we're using an inifinite plane)
    // we need to allocate dynamically, otherwise it's gonna be a local variable
    // and it'll be gone
    // 1st arg: btVector3(0,1,0) normal vector of the plane
    // 2nd arg: distance from the origin
	btStaticPlaneShape* plane=new btStaticPlaneShape(btVector3(0,1,0),0);

    // motionstate provides interpolation capabilities, and only synchornizes 'active objects'
	btMotionState* motion=new btDefaultMotionState(t);

    // btRigidBodyConstructionInfo constructor
    // Dynamics\btRigidBody.h line 149
	btRigidBody::btRigidBodyConstructionInfo info(0.0,motion,plane);
	btRigidBody* body=new btRigidBody(info);

    // adding the plane
	// add the body to the dynamic world
	world->addRigidBody(body);
    bodies.push_back(new bulletObject(body,4,0.8,0.8,0.8));
    body->setUserPointer(bodies[bodies.size()-1]);






    tr.setIdentity();


    // we start off with a sphere AT (0,40,0)
	/*
	addSphere(1.0,
                20,20,0,
                    1.0);	//add a new sphere above the ground

*/
	addSphere(1.0,
                0,20,0,
                    1.0);	//add a new sphere above the ground


    // position of the cloth
    float s=4;

    // height of the cloth
    float h = 20;

    /*
    // look up btSoftBodyHelpers.h
    btSoftBody* softBody = btSoftBodyHelpers::CreatePatch(
                    world->getWorldInfo(),
                    btVector3(-s, h, -s),
                    btVector3(s, h, -s),
                    btVector3(-s, h, s),
                    btVector3(s, h, s), 50, 50, 4+8, true);
*/


 btSoftBody* softBody=btSoftBodyHelpers::CreatePatch(
        world->getWorldInfo(),btVector3(-s,h,-s),btVector3(s,h,-s),
        btVector3(-s,h,s),btVector3(s,h,s),50,50,4+8,true);
    // add to the world


    // changing the softbody configuration
    softBody->m_cfg.viterations=25;
    softBody->m_cfg.piterations=25;
    softBody->setTotalMass(3.0);
    softBody->setMass(100,100);

 //   world->addSoftBody(softBody);

    softBody = btSoftBodyHelpers::CreateEllipsoid(world->getWorldInfo(),
                                                btVector3(10,10,10),
                                                btVector3(2,2,2), 1000);
    softBody->m_cfg.viterations=25;
    softBody->m_cfg.piterations=25;
    softBody->setTotalMass(3.0);
    softBody->setMass(0,0);
    world->addSoftBody(softBody);

	glClearColor(0,0,0,1);
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(angle,640.0/480.0,1,1000);
	glMatrixMode(GL_MODELVIEW);


//	initskybox();
	glEnable(GL_DEPTH_TEST);
	cam.setLocation(vector3d(10,10,10));	//the player will be top of the terrain







    initskybox();

    vector<unsigned int> anim;
    loadAnimation(anim, "./3D models/glocks/glock", 56);
    weapons.push_back(new weapon(anim,
                                anim[0],
                                TEST_NORMAL_FRAME, TEST_FIRE_FRAME, TEST_RELOAD_FRAME,
                                vector3d(0.56004,-0.83,-1.85628),// vector3d(2.39973,-1.58889,-10.0001),
                             //   vector3d(0,272.6,0),
                                //vector3d(10,0,0),// vector3d(2.39973,-1.58889,-10.0001),
                                vector3d(0,187,0),
                                vector3d(0.0400399,-0.75,-1.41628),
                                vector3d(0,178.6,0),
                                100, 1000, 100, TEST_10_AMMO, TEST_30_TOTAL_AMMO, 20, "weapon1", AUTOMATIC));




    player1 = new player("player1",
                        collisionsphere(vector3d(10,10,10),3.0),PLAYER_SPRINT_SPEED, PLAYER_WALK_SPEED,0.2, weapons[0]);



    initPhysics_BasicDemo();

//    Create_Vehicle();


}

game::~game()
{

	SDL_Quit();    // delete the sphere
}






void game::Create_Vehicle()
{
    m_defaultContactProcessingThreshold = BT_LARGE_FLOAT;


    /// creating the shapes
    /// btBoxShape takes boxHalfExtents argument
    btCollisionShape* chassisShape = new btBoxShape(btVector3(1.f, 0.5f, 2.f));
    m_collisionShapes.push_back(chassisShape);
    btCompoundShape* compound = new btCompoundShape();


    m_collisionShapes.push_back(compound);
    // check all the addsphere, addbox functions, all of them have a btTransform function
    btTransform localTrans;
    localTrans.setIdentity();
    localTrans.setOrigin(btVector3(0,1,0));


    compound->addChildShape(localTrans, chassisShape);

    tr.setOrigin(btVector3(0,0.f,0));


    m_carChassis = CreateRigidBody(800, tr, compound);


    /// This is causing it to crash...
    m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth, wheelRadius, wheelRadius));

    clientResetScene();




    {
        m_vehicleRaycaster = new btDefaultVehicleRaycaster(world);
        m_vehicle = new btRaycastVehicle(m_tuning, m_carChassis, m_vehicleRaycaster);

        m_carChassis->setActivationState(DISABLE_DEACTIVATION);

        world->addVehicle(m_vehicle);

        float connectionHeight = 1.2f;


        m_vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

        bool isFrontWheel = true;
        btVector3 connectionPointCS0(CUBE_HALF_EXTENTS-(0.3*wheelWidth), connectionHeight, 2*CUBE_HALF_EXTENTS-wheelRadius);
        m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS,suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);


        connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS+(0.3*wheelWidth),connectionHeight,2*CUBE_HALF_EXTENTS-wheelRadius);
        m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,m_tuning,isFrontWheel);


        connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS+(0.3*wheelWidth),connectionHeight,-2*CUBE_HALF_EXTENTS+wheelRadius);
        isFrontWheel = false;
        m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,m_tuning,isFrontWheel);

        connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS-(0.3*wheelWidth),connectionHeight,-2*CUBE_HALF_EXTENTS+wheelRadius);
        m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,m_tuning,isFrontWheel);

        for (int i=0;i<m_vehicle->getNumWheels();i++)
		{
			btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
			wheel.m_suspensionStiffness = suspensionStiffness;
			wheel.m_wheelsDampingRelaxation = suspensionDamping;
			wheel.m_wheelsDampingCompression = suspensionCompression;
			wheel.m_frictionSlip = wheelFriction;
			wheel.m_rollInfluence = rollInfluence;
		}
    }
}



void game::Delete_Vehicle()
{
    delete m_carChassis;
    	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}


    delete m_wheelShape;

}




void game::clientResetScene()
{
    /*

    gVehicleSteering = 0.f;
	m_carChassis->setCenterOfMassTransform(btTransform::getIdentity());
	m_carChassis->setLinearVelocity(btVector3(0,0,0));
	m_carChassis->setAngularVelocity(btVector3(0,0,0));

*/
//	world->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_carChassis->getBroadphaseHandle(),world->getDispatcher());
/*
	if (m_vehicle)
	{

		m_vehicle->resetSuspension();

		for (int i=0;i<m_vehicle->getNumWheels();i++)
		{
			//synchronize the wheels with the (interpolated) chassis worldtransform
			m_vehicle->updateWheelTransform(i,true);
		}

	}
*/
}








btRigidBody* game::CreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape)
{
    btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
	btRigidBody* body = new btRigidBody(mass,0,shape,localInertia);
	body->setWorldTransform(startTransform);
#endif//

	world->addRigidBody(body);

	return body;


}


void game::show()
{
//    player1->update(levels[0]->getCollisionPlanes());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	player1->cam.Control();
	//cam.Control();
    drawSkybox(500.0);

	player1->cam.UpdateCamera();
	//cam.UpdateCamera();



    vector3d direction = player1->cam.getVector()*10000;

    glColor3f(1,0,0);
//    gluSphere(quad, 1.0, 30, 30);
/*
    glPushMatrix();
        glTranslatef(2,2,2);
        gluCylinder(quad, 1.0,1.0,5.0,30,1);
    glPopMatrix();
*/
    cout << "player1 getposition is " << player1->getPosition().x << " "
                                        << player1->getPosition().y << " "
                                        << player1->getPosition().z << endl;

    cout << "player1->cam Location is " << player1->cam.getLocation().x << " "
                                        << player1->cam.getLocation().y << " "
                                        << player1->cam.getLocation().z << endl << endl;


    // btCollisionWorld::ClosestRayResultCallback
    // if I want all the objects hit by the ray,not just the closest

    /*
    btCollisionWorld::AllHitsRayResultCallback rayCallback(btVector3(player1->cam.getLocation().x,
                                                        player1->cam.getLocation().y,
                                                        player1->cam.getLocation().z),

                                                            btVector3(direction.x,
                                                                        direction.y,
                                                                        direction.z)
                                                           );


    world->rayTest(btVector3(player1->cam.getLocation().x,
                                player1->cam.getLocation().y,
                                player1->cam.getLocation().z),

                                btVector3(direction.x,
                                            direction.y,
                                            direction.z)
                                 ,rayCallback);

    // if it hit something
    if(rayCallback.hasHit())
    {
        for(int i=0; i<rayCallback.m_collisionObjects.size(); i++)
        {
      //  (bulletObject*)(rayCallback.m_collisionObject->getUserPointer())->hit = true;
            ((bulletObject*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->hit=true;
            cout << rayCallback.m_hitFractions[i] << endl;

        }
    }

*/


	for(int i=0;i<bodies.size();i++)
	{
        if(bodies[i]->body->getCollisionShape()->getShapeType()==STATIC_PLANE_PROXYTYPE)
                renderPlane(bodies[i]);
        else if(bodies[i]->body->getCollisionShape()->getShapeType()==SPHERE_SHAPE_PROXYTYPE)
                renderSphere(bodies[i]);
        else if(bodies[i]->body->getCollisionShape()->getShapeType()==CYLINDER_SHAPE_PROXYTYPE)
                renderCylinder(bodies[i]);
        else if(bodies[i]->body->getCollisionShape()->getShapeType()==CONE_SHAPE_PROXYTYPE)
                renderCone(bodies[i]);
        else if(bodies[i]->body->getCollisionShape()->getShapeType()==BOX_SHAPE_PROXYTYPE)
                renderBox(bodies[i]);
	}

    for(int i=0; i<world->getSoftBodyArray().size(); i++)
        renderSoftbody(world->getSoftBodyArray()[i]);



/*
	btScalar m[16];
	int i;


// rendering the car
    btVector3 wheelColor(1,0,0);

    btVector3	worldBoundsMin,worldBoundsMax;
	world->getBroadphase()->getBroadphaseAabb(worldBoundsMin,worldBoundsMax);





    for (i=0;i<m_vehicle->getNumWheels();i++)
	{
		//synchronize the wheels with the (interpolated) chassis worldtransform
		m_vehicle->updateWheelTransform(i,true);
		//draw wheels (cylinders)
		m_vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);
		m_shapeDrawer->drawOpenGL(m,m_wheelShape,wheelColor,getDebugMode(),worldBoundsMin,worldBoundsMax);
	}
*/

    render_BasicDemo();
          glColor3f(1,1,1);



}




void game::loadAnimation(vector<unsigned int>& anim, string filename,
            unsigned int frame_Num)
{
    char tmp[500];
    unsigned int id;
    // from 0 to 37 because the filenames are written that way
    for(int i=0; i< frame_Num; i++)
    {
    //    if(i<10)
        sprintf(tmp, "_%06d.obj", i);
    //    else if (i<100)

        string tmp2(filename+tmp);
 //       tmp2 += ".obj";
   //     cout << tmp2 << endl;

        unsigned int id = obj.load(tmp2, NULL);
        anim.push_back(id);
    }
}



// ..\SDL\Tank_Game\libraries\bullet Physics\bullet-2.81-rev2613\src\vectormath\scalar

// btManifoldPoint.h line 42
// used to manage collision detection objects
bool callbackFunc(btManifoldPoint& cp,
                    const btCollisionObjectWrapper* obj1,
                    int id1,int index1,
                    const btCollisionObjectWrapper* obj2,
                    int id2,int index2)
{
    /*
    ((bulletObject*)obj1->getUserPointer())->hit=true;

    ((bulletObject*)obj2->getUserPointer())->hit=true;
*/


// with ray tracing

    cout << "collision" << endl;

    const btCollisionObject* obj3 = obj1->getCollisionObject();
    const btCollisionObject* obj4 = obj2->getCollisionObject();
 //   if(obj3 != NULL && obj4 != NULL)
 //   cout << ((bulletObject*)obj3->getUserPointer())->id <<  " " <<  ((bulletObject*)obj4->getUserPointer())->id <<  endl;
    ((bulletObject*)obj3->getUserPointer())->hit=true;

    ((bulletObject*)obj4->getUserPointer())->hit=true;
    return false;

}




/*
void game::start()
{

}
*/





extern ContactAddedCallback	gContactAddedCallback;

int main(int argc, char *argv[])
{


	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetVideoMode(640,480,32,SDL_OPENGL);
	Uint32 start;
	SDL_Event event;
	bool running=true;
	float angle=50;

    game Martin(angle);

	//init(angle);
    gContactAddedCallback=callbackFunc;
/*
    addCylinder(2,5,
                    20,30,0,
                        1.0);

    addCone(2,5,
            25,30,0,
                1.0);

    addBox(10,2,3,
            20,40,0,
                1.0);
*/
    addCylinder(2,5,0,30,0,1.0);
    addCone(2,5,5,30,0,1.0);
    addBox(10,2,3,0,40,0,1.0);



	while(running)
	{
		start=SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					running=false;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{

						case SDLK_ESCAPE:
							running=false;
							break;

                        case SDLK_p:
                            Martin.player1->cam.mouseIn(false);

                            //cam.mouseIn(false);
                            SDL_ShowCursor(SDL_ENABLE);
                            break;

						case SDLK_SPACE:
						//if space is pressed, shoot a ball
							btRigidBody* sphere=addSphere(1.0,Martin.player1->cam.getLocation().x,
                                                                Martin.player1->cam.getLocation().y,
                                                                Martin.player1->cam.getLocation().z,10.0);

							// by setting velocity, the ball will go forward
                            // first set the vector where the camera is facing
                            // then give it to the sphere
							vector3d look=Martin.player1->cam.getVector()*SPHERE_VELOCITY;
							//sphere->setLinearVelocity(btVector3(look.x,look.y,look.z));

                            sphere->setLinearVelocity(btVector3(look.x,look.y,look.z));

                            // btCollisionObject.h line 118
                            sphere->setCollisionFlags(sphere->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
							break;

					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym)
					{

					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					Martin.player1->cam.mouseIn(true);
					SDL_ShowCursor(SDL_DISABLE);
					break;

			}
		}

		// steps through the simluation to update
		// wating for the time elapsed then
		// argument: amount of time to step the simulation by
		// you can actually call this with varying variables, so you can actually mease the time since last update

        for(int i=0;i<bodies.size();i++)
            bodies[i]->hit=false;

		world->stepSimulation(1/60.0);

		Martin.show();
		SDL_GL_SwapBuffers();
		if(1000.0/60>SDL_GetTicks()-start)
			SDL_Delay(1000.0/60-(SDL_GetTicks()-start));
	}
	//killskybox();
    for(int i=0;i<bodies.size();i++)
    {
            world->removeCollisionObject(bodies[i]->body);
            btMotionState* motionState=bodies[i]->body->getMotionState();
            btCollisionShape* shape=bodies[i]->body->getCollisionShape();
            delete bodies[i]->body;
            delete shape;
            delete motionState;
            delete bodies[i];
    }

    for(int i=0;i<world->getSoftBodyArray().size();i++)
    {
            world->removeSoftBody(world->getSoftBodyArray()[i]);
            delete (world->getSoftBodyArray()[i]);
    }

    Martin.Delete_Vehicle();

	delete dispatcher;
	delete collisionConfig;
	delete solver;
	delete broadphase;
	delete softbodySolver;
	delete world;
    SDL_Quit();
	gluDeleteQuadric(quad);

}



void game::initPhysics_BasicDemo()
{

    /// look at the DemoApplication.cpp constructor
    m_sundirection = 1000* btVector3(1, -2, 1);
    m_shapeDrawer = new GL_ShapeDrawer();
	m_shapeDrawer->enableTexture(true);
	m_enableshadows = false;


    setTexturing(true);
    setShadows(true);

    btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));

    m_collisionShapes.push_back(groundShape);
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-50,0));

    //We can also use DemoApplication::localCreateRigidBody, but for clarity it is provided here:
    {
        btScalar mass(0.);

        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (mass != 0.f);

        btVector3 localInertia(0,0,0);
        if (isDynamic)
            groundShape->calculateLocalInertia(mass, localInertia);

        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);

        //add the body to the dynamics world
    //    world->addRigidBody(body);
    }


    {
    // create a few dynamic rigidbodies
    // re-using the same collision for better for memory usage and performance

        btBoxShape* colShape = new btBoxShape(btVector3(SCALING*1,SCALING*1,SCALING*1));

        m_collisionShapes.push_back(colShape);

        /// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

        btScalar	mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0,0,0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass,localInertia);

        float start_x = START_POS_X - ARRAY_SIZE_X/2;
		float start_y = START_POS_Y;
		float start_z = START_POS_Z - ARRAY_SIZE_Z/2;

        for (int k=0;k<ARRAY_SIZE_Y;k++)
		{
			for (int i=0;i<ARRAY_SIZE_X;i++)
			{
				for(int j = 0;j<ARRAY_SIZE_Z;j++)
				{
					startTransform.setOrigin(SCALING*btVector3(
										btScalar(2.0*i + start_x),
										btScalar(20+2.0*k + start_y),
										btScalar(2.0*j + start_z)));

					//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
					btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
					btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
					btRigidBody* body = new btRigidBody(rbInfo);


					world->addRigidBody(body);




				}
			}
		}
	}
}


void game::myinit()
{
    GLfloat light_ambient[] = { btScalar(0.2), btScalar(0.2), btScalar(0.2), btScalar(1.0) };
	GLfloat light_diffuse[] = { btScalar(1.0), btScalar(1.0), btScalar(1.0), btScalar(1.0) };
	GLfloat light_specular[] = { btScalar(1.0), btScalar(1.0), btScalar(1.0), btScalar(1.0 )};
	/*	light_position is NOT default value	*/
	GLfloat light_position0[] = { btScalar(1.0), btScalar(10.0), btScalar(1.0), btScalar(0.0 )};
	GLfloat light_position1[] = { btScalar(-1.0), btScalar(-10.0), btScalar(-1.0), btScalar(0.0) };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);


	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(btScalar(0.7),btScalar(0.7),btScalar(0.7),btScalar(0));

	//  glEnable(GL_CULL_FACE);
	//  glCullFace(GL_BACK);

}

void game::render_BasicDemo()
{
   // 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    myinit();

    if(world)
    {
        cout << "I'm inside render_BasicDemo" << endl;
        {
            glDisable(GL_CULL_FACE);
            renderscene_BasicDemo(0);

        }

        int xOffset = 10;
        int yStart = 20;
        int yIncr = 20;

        glDisable(GL_LIGHTING);
        glColor3f(0 ,0 ,0);
    }

}

void game::renderscene_BasicDemo(int pass)
{
    btScalar	m[16];
	btMatrix3x3	rot;
	rot.setIdentity();

	const int	numObjects=world->getNumCollisionObjects();
	btVector3 wireColor(1,1,1);
    cout << "numObjects is " << numObjects << endl;

    glPushMatrix();
    for(int i=4; i<numObjects; i++)
    {



        btCollisionObject*  colObj=world->getCollisionObjectArray()[i];

        // check btRigidBody.h line 197
        btRigidBody*        body=btRigidBody::upcast(colObj);


        cout << "shape is " << colObj->getCollisionShape()->getShapeType() << endl;
  //      cout << "collision shape is " << body->getCollisionShape()->getShapeType()<< endl;

		if(body&&body->getMotionState())
		{
			btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
			myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
			rot=myMotionState->m_graphicsWorldTrans.getBasis();
		}
		else
		{
			colObj->getWorldTransform().getOpenGLMatrix(m);
			rot=colObj->getWorldTransform().getBasis();
		}

/*
        btVector3 wireColor(1.f,1.0f,0.5f); //wants deactivation
		if(i&1) wireColor=btVector3(0.f,0.0f,1.f);
		///color differently for active, sleeping, wantsdeactivation states
		if (colObj->getActivationState() == 1) //active
		{
			if (i & 1)
			{
				wireColor += btVector3 (1.f,0.f,0.f);
			}
			else
			{
				wireColor += btVector3 (.5f,0.f,0.f);
			}
		}
		if(colObj->getActivationState()==2) //ISLAND_SLEEPING
		{
			if(i&1)
			{
				wireColor += btVector3 (0.f,1.f, 0.f);
			}
			else
			{
				wireColor += btVector3 (0.f,0.5f,0.f);
			}
		}


*/




        btVector3 aabbMin,aabbMax;
		world->getBroadphase()->getBroadphaseAabb(aabbMin,aabbMax);

		aabbMin-=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);
		aabbMax+=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);

        switch(pass)
        {
            case	0:	m_shapeDrawer->drawOpenGL(m,colObj->getCollisionShape(),wireColor,0,aabbMin,aabbMax);break;
            case	1:	m_shapeDrawer->drawShadow(m,m_sundirection*rot,colObj->getCollisionShape(),aabbMin,aabbMax);break;
            case	2:	m_shapeDrawer->drawOpenGL(m,colObj->getCollisionShape(),wireColor*btScalar(0.3),0,aabbMin,aabbMax);break;
        }

 //        glBindTexture(GL_TEXTURE_2D, 0);
    }
        glPopMatrix();
        glColor3f(1,1,1);
    //    glDisable(GL_COLOR_MATERIAL);

}


