#include "main.h"


#include "GLDebugDrawer.h"
#include <stdio.h> //printf debugging

#include "GL_ShapeDrawer.h"

#include "GlutStuff.h"

#include <SOIL.h>


#define PLANE_SIZE  1000
#define SPHERE_VELOCITY  50


#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480


///create 125 (5x5x5) dynamic object
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5



///scaling of the objects (0.1 = 20 centimeter boxes )
#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y -5
#define START_POS_Z -3

bool PILE = true;



int Cyl_x = 0;
int Cyl_y = 30;
int Cyl_z = 0;

int Cone_x = 5;
int Cone_y = 30;
int Cone_z = 0;

int Box_x = 0;
int Box_y = 40;
int Box_z = 0;



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



int gPickingConstraint_M = 0;
btVector3 gOldPickingPos_M;
btVector3 gHitPos_M(-1,-1,-1);
float gOldPickingDist_M = 0.f;
btRigidBody* pickedBody_M = 0;
btScalar mousePickClamping_M = 30.f;





using namespace std;

struct bulletObject
{
    int id;
    float r,g,b;
    bool hit;

    bool pile;

    // void* yourOwnStructure; // function
    btRigidBody* body;
    bulletObject(btRigidBody* b, int i, float r0, float g0, float b0, bool pile0 = false) : body(b), id(1)
                            ,r(r0), g(g0), b(b0), hit(false), pile(pile0)
    {}
};




camera cam;

// this is a declared Quadrics, used to draw any bizzare shapes
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


btRigidBody* addBox(float width,float height,float depth,float x,float y,float z,float mass, bool pile = false)
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

    bodies.push_back(new bulletObject(body, 3, 1,1,0, pile));	//to be easier to clean, I store them a vector

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



void renderCappedCylinder(bulletObject* b_obj)
{
    btRigidBody* sphere=b_obj->body;
    // check if it's a sphere by checking the shape
    // btRigidBody.h line 248
	if(sphere->getCollisionShape()->getShapeType()!=CYLINDER_SHAPE_PROXYTYPE)	//only render, if it's a sphere
		return;

    // typecast sphere->getCollisionShape to btCylinderShape
    const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(sphere->getCollisionShape());
    int upAxis = cylinder->getUpAxis();


    float radius = cylinder->getRadius();
    float halfHeight = cylinder->getHalfExtentsWithMargin()[upAxis];



    if(!b_obj->hit)
            glColor3f(b_obj->r,b_obj->g,b_obj->b);
    else
            glColor3f(1,0,0);



// to get the motionState
	btTransform t;
	sphere->getMotionState()->getWorldTransform(t);	//get the transform
	float mat[16];
	t.getOpenGLMatrix(mat);	//OpenGL matrix stores the rotation and orientation

	// this Matrix multiplication will only apply to this object, not everything else
	glPushMatrix();
		glMultMatrixf(mat);	//multiplying the current matrix with it moves the object in place


    switch (upAxis)
    {
        case 0:
            glRotatef(-90.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 0.0, -halfHeight);
            break;
        case 1:
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glTranslatef(0.0, 0.0, -halfHeight);
            break;
        case 2:

            glTranslatef(0.0, 0.0, -halfHeight);
            break;

        default:
            {
                cout << "weird wheel axis" << endl;
            }
    }



	gluQuadricDrawStyle(quad, (GLenum)GLU_FILL);
	gluQuadricNormals(quad, (GLenum)GLU_SMOOTH);

    gluDisk(quad,0,radius,15, 10);

	gluCylinder(quad, radius, radius, 2.f*halfHeight, 15, 10);
	glTranslatef(0.0f, 0.0f, 2.f*halfHeight);
	glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);

    gluDisk(quad,0,radius,15, 10);

    glPopMatrix();


}





void renderWheel(btScalar* m, const btCollisionShape* shape)
{

    // check if it's a sphere by checking the shape
	if(shape->getShapeType()!=CYLINDER_SHAPE_PROXYTYPE)	//only render, if it's a sphere
		return;

    // typecast sphere->getCollisionShape to btCylinderShape
    const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(shape);
    int upAxis = cylinder->getUpAxis();


    float radius = cylinder->getRadius();
    float halfHeight = cylinder->getHalfExtentsWithMargin()[upAxis];


    glColor3f(1,0,0);


	// this Matrix multiplication will only apply to this object, not everything else
	glPushMatrix();
		glMultMatrixf(m);	//multiplying the current matrix with it moves the object in place


        switch (upAxis)
        {
            case 0:
                glRotatef(-90.0, 0.0, 1.0, 0.0);
                glTranslatef(0.0, 0.0, -halfHeight);
                break;
            case 1:
                glRotatef(-90.0, 1.0, 0.0, 0.0);
                glTranslatef(0.0, 0.0, -halfHeight);
                break;
            case 2:

                glTranslatef(0.0, 0.0, -halfHeight);
                break;

            default:
                {
                    cout << "weird wheel axis" << endl;
                }
        }



        gluQuadricDrawStyle(quad, (GLenum)GLU_FILL);
        gluQuadricNormals(quad, (GLenum)GLU_SMOOTH);

        gluDisk(quad,0,radius,15, 10);

        gluCylinder(quad, radius, radius, 2.f*halfHeight, 15, 10);
        glTranslatef(0.0f, 0.0f, 2.f*halfHeight);
        glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);

        gluDisk(quad,0,radius,15, 10);

    glPopMatrix();


}




void renderPileBox(bulletObject* b_obj, int i, bool draw_outline)
{

    if(!draw_outline)
    {

        glDisable(GL_CULL_FACE);

        btRigidBody* sphere=b_obj->body;



        btTransform t;
        sphere->getMotionState()->getWorldTransform(t);
        btScalar mat[16];
        t.getOpenGLMatrix(mat);


        // determining the color
        btVector3 boxColor(1.f,1.0f,0.5f); //wants deactivation
        if(i&1) boxColor=btVector3(0.f,0.0f,1.f);
        ///color differently for active, sleeping, wantsdeactivation states
        if (sphere->getActivationState() == 1) //active
        {
            if (i & 1)
            {
                boxColor += btVector3 (1.f,0.f,0.f);
            }
            else
            {
                boxColor += btVector3 (.5f,0.f,0.f);
            }
        }
        if(sphere->getActivationState()==2) //ISLAND_SLEEPING
        {
            if(i&1)
            {
                boxColor += btVector3 (0.f,1.f, 0.f);
            }
            else
            {
                boxColor += btVector3 (0.f,0.5f,0.f);
            }
        }
        glColor3f(boxColor.x(), boxColor.y(), boxColor.z());




        glPushMatrix();
        btglMultMatrix(mat);
            btCollisionShape* shape = sphere->getCollisionShape();

            const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);

            btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();

            static int indices[36] = {
                0,1,2,
                3,2,1,
                4,0,6,
                6,0,2,
                5,1,4,
                4,1,0,
                7,3,1,
                7,1,5,
                5,4,7,
                7,4,6,
                7,2,3,
                7,6,2};

             btVector3 vertices[8]={
                btVector3(halfExtent[0],halfExtent[1],halfExtent[2]),
                btVector3(-halfExtent[0],halfExtent[1],halfExtent[2]),
                btVector3(halfExtent[0],-halfExtent[1],halfExtent[2]),
                btVector3(-halfExtent[0],-halfExtent[1],halfExtent[2]),
                btVector3(halfExtent[0],halfExtent[1],-halfExtent[2]),
                btVector3(-halfExtent[0],halfExtent[1],-halfExtent[2]),
                btVector3(halfExtent[0],-halfExtent[1],-halfExtent[2]),
                btVector3(-halfExtent[0],-halfExtent[1],-halfExtent[2])};

            glBegin (GL_TRIANGLES);
            int si=36;
            for (int i=0;i<si;i+=3)
            {
                const btVector3& v1 = vertices[indices[i]];;
                const btVector3& v2 = vertices[indices[i+1]];
                const btVector3& v3 = vertices[indices[i+2]];
                btVector3 normal = (v3-v1).cross(v2-v1);
                normal.normalize ();
                glNormal3f(normal.getX(),normal.getY(),normal.getZ());
                glVertex3f (v1.x(), v1.y(), v1.z());
                glVertex3f (v2.x(), v2.y(), v2.z());
                glVertex3f (v3.x(), v3.y(), v3.z());

            }
            glEnd();


        glNormal3f(0,1,0);

        glPopMatrix();
    }
    else
    {
    }
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
                        glNormal3f(-1.0,0.0,0.0);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(1.0,0.0,0.0);
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(0.0,0.0,1.0);
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(0.0,0.0,-1.0);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(0.0,1.0,0.0);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(0.0,-1.0,0.0);
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                glEnd();
        glPopMatrix();

}

void renderCarBody(btScalar* m, const btCollisionShape* boxShape);






void renderCompound(bulletObject* b_obj)
{
    cout << "I'm in Render Compound " << endl;

    btRigidBody* sphere=b_obj->body;
    if(sphere->getCollisionShape()->getShapeType()!=COMPOUND_SHAPE_PROXYTYPE)
        return;

    glColor3f(0,0,1);

    btTransform t;
    sphere->getMotionState()->getWorldTransform(t);
    float mat[16];
    t.getOpenGLMatrix(mat);

	glPushMatrix();
	btglMultMatrix(mat);

        const btCompoundShape* compoundShape = static_cast<const btCompoundShape*>(sphere->getCollisionShape());

        for (int i=compoundShape->getNumChildShapes()-1;i>=0;i--)
        {

            btTransform childTrans = compoundShape->getChildTransform(i);
            const btCollisionShape* colShape = compoundShape->getChildShape(i);


            ATTRIBUTE_ALIGNED16(btScalar) childMat[16];
            childTrans.getOpenGLMatrix(childMat);
            renderCarBody(childMat, colShape);
       //     drawOpenGL(childMat,colShape,color,debugMode,worldBoundsMin,worldBoundsMax);
        }
    glPopMatrix();
}



void renderCarBody(btScalar* m, const btCollisionShape* shape)
{

        const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);
        glColor3f(0,0,1);


     //   btVector3 extent=((btBoxShape*)sphere->getCollisionShape())->getHalfExtentsWithoutMargin();



        btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
        glPushMatrix();
                btglMultMatrix(m);    //translation,rotation
					static int indices[36] = {
						0,1,2,
						3,2,1,
						4,0,6,
						6,0,2,
						5,1,4,
						4,1,0,
						7,3,1,
						7,1,5,
						5,4,7,
						7,4,6,
						7,2,3,
						7,6,2};

					 btVector3 vertices[8]={
						btVector3(halfExtent[0],halfExtent[1],halfExtent[2]),
						btVector3(-halfExtent[0],halfExtent[1],halfExtent[2]),
						btVector3(halfExtent[0],-halfExtent[1],halfExtent[2]),
						btVector3(-halfExtent[0],-halfExtent[1],halfExtent[2]),
						btVector3(halfExtent[0],halfExtent[1],-halfExtent[2]),
						btVector3(-halfExtent[0],halfExtent[1],-halfExtent[2]),
						btVector3(halfExtent[0],-halfExtent[1],-halfExtent[2]),
						btVector3(-halfExtent[0],-halfExtent[1],-halfExtent[2])};

					glBegin (GL_TRIANGLES);
					int si=36;
					for (int i=0;i<si;i+=3)
					{
						const btVector3& v1 = vertices[indices[i]];;
						const btVector3& v2 = vertices[indices[i+1]];
						const btVector3& v3 = vertices[indices[i+2]];
						btVector3 normal = (v3-v1).cross(v2-v1);
						normal.normalize ();
						glNormal3f(normal.getX(),normal.getY(),normal.getZ());
						glVertex3f (v1.x(), v1.y(), v1.z());
						glVertex3f (v2.x(), v2.y(), v2.z());
						glVertex3f (v3.x(), v3.y(), v3.z());

					}
					glEnd();
        glPopMatrix();
/*
        glPushMatrix();
                glMultMatrixf(m);     //translation,rotation
                glBegin(GL_QUADS);
                        glNormal3f(-1.0,0.0,0.0);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(1.0,0.0,0.0);
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(0.0,0.0,1.0);
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(0.0,0.0,-1.0);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(0.0,1.0,0.0);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                glEnd();
                glBegin(GL_QUADS);
                        glNormal3f(0.0,-1.0,0.0);
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                glEnd();
        glPopMatrix();
*/
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
    draw_outline = false;
    MouseDragMode = false;
    m_pickConstraint_M = 0;
	m_mouseOldX = 0;
	m_mouseOldY = 0;


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

		// fovy, aspect, zNear, zFar

		// you block out things inside the near plane

		// http://www.youtube.com/watch?v=ohksz3A00fk
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
    init_Vehicle();


}

game::~game()
{

	SDL_Quit();    // delete the sphere
}






void game::init_Vehicle()
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

    // implementation: m_childShapes.push_back(shape)
    compound->addChildShape(localTrans, chassisShape);


    tr.setOrigin(btVector3(20,5.f,0));
    m_carChassis = CreateRigidBody(800, tr, compound);


    /// This is causing it to crash...
    m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth, wheelRadius, wheelRadius));




	/// create vehicle

    {
        m_vehicleRaycaster = new btDefaultVehicleRaycaster(world);
        m_vehicle = new btRaycastVehicle(m_tuning, m_carChassis, m_vehicleRaycaster);

        m_carChassis->setActivationState(DISABLE_DEACTIVATION);

        world->addVehicle(m_vehicle);

        float connectionHeight = 1.2f;


        m_vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

        bool isFrontWheel = true;
        //choose coordinate system
		m_vehicle->setCoordinateSystem(rightIndex,upIndex,forwardIndex);
		btVector3 connectionPointCS0(CUBE_HALF_EXTENTS-(0.3*wheelWidth),connectionHeight,2*CUBE_HALF_EXTENTS-wheelRadius);

		m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,m_tuning,isFrontWheel);
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


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);


	world->addRigidBody(body);
    bodies.push_back(new bulletObject(body,0,1.0,0.0,0.0));
    body->setUserPointer(bodies[bodies.size()-1]);
	return body;




/*
    btScalar mass(0.);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic)
        groundShape->calculateLocalInertia(mass,localInertia);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,Shape,localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);


    // this is where you add the actual body
    //add the body to the dynamics world

	world->addRigidBody(body);

    return body;

*/
}


btVector3 game::getCameraPosition()
{
    return btVector3(player1->cam.getLocation().x, player1->cam.getLocation().y, player1->cam.getLocation().z);
}

btVector3 game::getCameraTargetPosition()
{
    return btVector3(player1->cam.getVector().x, player1->cam.getVector().y, player1->cam.getVector().z);
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

/*
    cout << "player1 getposition is " << player1->getPosition().x << " "
                                        << player1->getPosition().y << " "
                                        << player1->getPosition().z << endl;
*/

/*
    cout << "player1->cam Location is " << player1->cam.getLocation().x << " "
                                        << player1->cam.getLocation().y << " "
                                        << player1->cam.getLocation().z << endl << endl;
*/

/*
    cout << "player1->cam look direction is " << player1->cam.getVector().x << " "
                                        << player1->cam.getVector().y << " "
                                        << player1->cam.getVector().z << endl << endl;

*/

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
    init_Lighting();
    glColor3f(1,1,1);
    glEnable(GL_COLOR_MATERIAL);
  //  glDisable(GL_LIGHTING);
	for(int i=0;i<bodies.size();i++)
	{
        if(bodies[i]->body->getCollisionShape()->getShapeType()==STATIC_PLANE_PROXYTYPE)
                renderPlane(bodies[i]);
        else if(bodies[i]->body->getCollisionShape()->getShapeType()==SPHERE_SHAPE_PROXYTYPE)
                renderSphere(bodies[i]);
        else if(bodies[i]->body->getCollisionShape()->getShapeType()==CYLINDER_SHAPE_PROXYTYPE)
                renderCappedCylinder(bodies[i]);
                // renderCylinder(bodies[i]);

        else if(bodies[i]->body->getCollisionShape()->getShapeType()==CONE_SHAPE_PROXYTYPE)
                renderCone(bodies[i]);

        else if(bodies[i]->body->getCollisionShape()->getShapeType()==BOX_SHAPE_PROXYTYPE)
        {
            if(bodies[i]->pile == PILE)
                renderPileBox(bodies[i],i,draw_outline);
            else
                renderBox(bodies[i]);
        }
        else if(bodies[i]->body->getCollisionShape()->getShapeType()==COMPOUND_SHAPE_PROXYTYPE)
            renderCompound(bodies[i]);

	}

    for(int i=0; i<world->getSoftBodyArray().size(); i++)
        renderSoftbody(world->getSoftBodyArray()[i]);


    btScalar m[16];

    btVector3 wheelColor(1,0,0);
	for (int i=0;i<m_vehicle->getNumWheels();i++)
	{
		//synchronize the wheels with the (interpolated) chassis worldtransform
		m_vehicle->updateWheelTransform(i,true);
		//draw wheels (cylinders)
		m_vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);

		renderWheel(m, m_wheelShape);
	//	m_shapeDrawer->drawOpenGL(m,m_wheelShape,wheelColor,getDebugMode(),worldBoundsMin,worldBoundsMax);
	}



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

//    render_BasicDemo();


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
    if(obj1 != NULL && obj2 != NULL)
    {
        const btCollisionObject* obj3 = obj1->getCollisionObject();
        const btCollisionObject* obj4 = obj2->getCollisionObject();
     //   if(obj3 != NULL && obj4 != NULL)
     //   cout << ((bulletObject*)obj3->getUserPointer())->id <<  " " <<  ((bulletObject*)obj4->getUserPointer())->id <<  endl;
        ((bulletObject*)obj3->getUserPointer())->hit=true;

        ((bulletObject*)obj4->getUserPointer())->hit=true;
    }
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




    addCylinder(2,5,Cyl_x,Cyl_y,Cyl_z,1.0);
    addCone(2,5,Cone_x,Cone_y,Cone_z,1.0);
    addBox(10,2,3,Box_x,Box_y,Box_z,1.0);



// adding the pile of box

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


                        case SDLK_g:
                            Martin.m_enableshadows = !Martin.m_enableshadows;
                            break;

                        // draw red_outlines
                        case SDLK_z:
                            Martin.draw_outline = !Martin.draw_outline;
                            break;

                        case SDLK_o:
                            Martin.MouseDragMode = !Martin.MouseDragMode;
                            cout << "Martin.MouseDragMode is now " << Martin.MouseDragMode << endl;
                            Martin.player1->cam.mouseIn(false);
                            SDL_ShowCursor(SDL_ENABLE);
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
                case SDL_MOUSEBUTTONUP:
               //     if (Martin.MouseDragMode)
                    {
                        Martin.removePickingConstraint();
                        break;
                    }

				case SDL_MOUSEBUTTONDOWN:
                    if (Martin.MouseDragMode)
                    {
                        int tmpx,tmpy;
                        SDL_GetMouseState(&tmpx,&tmpy);

                        cout << "x is " << tmpx << " " << "y is " << tmpy << endl;

                        Martin.mouseFunc_BasicDemo(tmpx, tmpy);
                        break;
                    }

                    // player mode
                    else
                    {
                        int tmpx,tmpy;
                        SDL_GetMouseState(&tmpx,&tmpy);

                        cout << "x is " << tmpx << " " << "y is " << tmpy << endl;

                        Martin.mouseFunc_BasicDemo(tmpx, tmpy);

                        Martin.player1->cam.mouseIn(true);
                        SDL_ShowCursor(SDL_DISABLE);
                        break;

                    }


			}
		}

		// steps through the simluation to update
		// wating for the time elapsed then
		// argument: amount of time to step the simulation by
		// you can actually call this with varying variables, so you can actually mease the time since last update

        int tmpx,tmpy;
        SDL_GetMouseState(&tmpx,&tmpy);

        Martin.mouseMotionFunc_BasicDemo(tmpx, tmpy);


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
    setShadows(false);

    // this is the plane-box the demo had
    btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));

    m_collisionShapes.push_back(groundShape);
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-50,0));

    //We can also use DemoApplication::localCreateRigidBody, but for clarity it is provided here:
    /*
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
    */

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


					// add a box
                    addBox(SCALING*2,SCALING*2, SCALING*2,
                                                SCALING*btScalar(2.0*i + start_x),
                                                SCALING*btScalar(20+2.0*k + start_y),
                                                SCALING*btScalar(2.0*j + start_z),1, PILE );

				}
			}
		}
	}
}


void game::init_Lighting()
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


    if(world)
    {
        cout << "I'm inside render_BasicDemo" << endl;

/*
        if(m_enableshadows)
		{
			glClear(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			renderscene_BasicDemo(0);

			glDisable(GL_LIGHTING);
			glDepthMask(GL_FALSE);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_STENCIL_TEST);
			glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
			glStencilFunc(GL_ALWAYS,1,0xFFFFFFFFL);
			glFrontFace(GL_CCW);
			glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
			renderscene_BasicDemo(1);
			glFrontFace(GL_CW);
			glStencilOp(GL_KEEP,GL_KEEP,GL_DECR);
			renderscene_BasicDemo(1);
			glFrontFace(GL_CCW);

			glPolygonMode(GL_FRONT,GL_FILL);
			glPolygonMode(GL_BACK,GL_FILL);
			glShadeModel(GL_SMOOTH);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_LIGHTING);
			glDepthMask(GL_TRUE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
			glEnable(GL_CULL_FACE);
			glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

			glDepthFunc(GL_LEQUAL);
			glStencilFunc( GL_NOTEQUAL, 0, 0xFFFFFFFFL );
			glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
			glDisable(GL_LIGHTING);
			renderscene_BasicDemo(2);
			glEnable(GL_LIGHTING);
			glDepthFunc(GL_LESS);
			glDisable(GL_STENCIL_TEST);
			glDisable(GL_CULL_FACE);
		}
		else
		*/
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
	btVector3 wireColor(1,0,0);
    cout << "numObjects is " << numObjects << endl;

    glPushMatrix();

    for(int i=0; i<numObjects; i++)
    {

        btCollisionObject*  colObj=world->getCollisionObjectArray()[i];

        // check btRigidBody.h line 197
        btRigidBody*        body=btRigidBody::upcast(colObj);


        cout << "shape is " << colObj->getCollisionShape()->getShapeType() << endl;
  //      cout << "collision shape is " << body->getCollisionShape()->getShapeType()<< endl;

        if( colObj->getCollisionShape()->getShapeType() != 0)
            continue;


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


        btVector3 aabbMin,aabbMax;
		world->getBroadphase()->getBroadphaseAabb(aabbMin,aabbMax);

		aabbMin-=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);
		aabbMax+=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);



        switch(pass)
        {
            case	0:	m_shapeDrawer->drawOpenGL(m,colObj->getCollisionShape(),wireColor,0,aabbMin,aabbMax);break;
         //   case	1:	m_shapeDrawer->drawShadow(m,m_sundirection*rot,colObj->getCollisionShape(),aabbMin,aabbMax);break;
        //    case	2:	m_shapeDrawer->drawOpenGL(m,colObj->getCollisionShape(),wireColor*btScalar(0.3),0,aabbMin,aabbMax);break;
        }

 //        glBindTexture(GL_TEXTURE_2D, 0);
    }


        glPopMatrix();
        glColor3f(1,1,1);
    //    glDisable(GL_COLOR_MATERIAL);

}




void game::mouseFunc_BasicDemo(int x, int y)
{
    m_mouseOldX = x;
    m_mouseOldY = y;

    btVector3 rayTo = getMousePickingRay(x,y);
    if(world)
    {


        /// use this if using First Person Center Mode
        /*
        btVector3 CamPos = getCameraPosition_M();
        btVector3 direction = getCameraTargetPosition_M()*10000;
        btCollisionWorld::ClosestRayResultCallback rayCallback(temp3,direction);

        world->rayTest(CamPos, direction, rayCallback);
        */





        btVector3 rayFrom;
        rayFrom = getCameraPosition();
        btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom,rayTo);

        /*
        cout << "rayFrom is " << rayFrom.x() << " "
                                        << rayFrom.y() << " "
                                        << rayFrom.z() << endl << endl;

        cout << "rayTo is " << rayTo.x() << " "
                                        << rayTo.y() << " "
                                        << rayTo.z() << endl << endl;
        */


        // shoot a virtual laser between the two points, if it hits, we continue
        // detects collision with any object in the world
        world->rayTest(rayFrom, rayTo, rayCallback);

        if (rayCallback.hasHit())
        {
            cout << "hits" << endl;

            btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);


            if (body)
            {

                if (!(body->isStaticObject() || body->isKinematicObject()))
                {
                    cout << "inside Constraint" << endl;
                    pickedBody_M = body;
                    pickedBody_M->setActivationState(DISABLE_DEACTIVATION);


                    btVector3 pickPos = rayCallback.m_hitPointWorld;
                    printf("pickPos=%f,%f,%f\n",pickPos.getX(),pickPos.getY(),pickPos.getZ());

                    // the center of mass matrix
                    // WHY????
                    btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;

                    // Glut's version of detecing SHIFT or CTRL
                    // so here means if SHIFT is pressed or not

                    {
                        btTransform tr;
                        tr.setIdentity();
                        tr.setOrigin(localPivot);

                        /// http://bulletphysics.org/Bullet/BulletFull/classbtGeneric6DofConstraint.html#details
                        /// btGeneric6DofConstraint between two rigidbodies each with a pivotpoint that describes the axis location in local space
                        btGeneric6DofConstraint* dof6_M = new btGeneric6DofConstraint(*body, tr,false);

                        /// these are used to set constraints of the box, (angular or linear)
                        /// if you comment these lines out, your box which turn or whatever as it collides with other boxes
                        dof6_M->setLinearLowerLimit(btVector3(0,0,0));
                        dof6_M->setLinearUpperLimit(btVector3(0,0,0));
                        dof6_M->setAngularLowerLimit(btVector3(0,0,0));
                        dof6_M->setAngularUpperLimit(btVector3(0,0,0));

                        world->addConstraint(dof6_M,true);
                        m_pickConstraint_M = dof6_M;


                        // CFM: constraint force mixing
                        dof6_M->setParam(BT_CONSTRAINT_STOP_CFM,0.8,0);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_CFM,0.8,1);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_CFM,0.8,2);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_CFM,0.8,3);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_CFM,0.8,4);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_CFM,0.8,5);

                        // ERP: error reduction parameter
                        dof6_M->setParam(BT_CONSTRAINT_STOP_ERP,0.1,0);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_ERP,0.1,1);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_ERP,0.1,2);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_ERP,0.1,3);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_ERP,0.1,4);
                        dof6_M->setParam(BT_CONSTRAINT_STOP_ERP,0.1,5);
                    }

                    //save mouse position for dragging
                    gOldPickingPos_M = rayTo;
                    gHitPos_M = pickPos;

                    gOldPickingDist_M  = (pickPos-rayFrom).length();
                }
            }

        }
    }


}

void game::mouseMotionFunc_BasicDemo(int x, int y)
{


    if(m_pickConstraint_M)
    {
        cout << "m_pickConstraint_M" << endl;


		if (m_pickConstraint_M->getConstraintType() == D6_CONSTRAINT_TYPE)
		{
            btGeneric6DofConstraint* pickCon = static_cast<btGeneric6DofConstraint*>(m_pickConstraint_M);
            if (pickCon)
            {

                btVector3 newRayTo = getMousePickingRay(x,y);
				btVector3 rayFrom;
				btVector3 oldPivotInB = pickCon->getFrameOffsetA().getOrigin();

				btVector3 newPivotB;

				{
				    cout << "not in m_orth" << endl;
					rayFrom = getCameraPosition();
					btVector3 dir = newRayTo-rayFrom;
					dir.normalize();
					dir *= gOldPickingDist_M;

					newPivotB = rayFrom + dir;
				}

				pickCon->getFrameOffsetA().setOrigin(newPivotB);
			}

		}

	}

	float dx, dy;
    dx = btScalar(x) - m_mouseOldX;
    dy = btScalar(y) - m_mouseOldY;

	m_mouseOldX = x;
    m_mouseOldY = y;

}




// http://nehe.gamedev.net/article/using_gluunproject/16013/
btVector3 game::getMousePickingRay(int mouse_x, int mouse_y)
{

    btVector3	rayFrom = getCameraPosition();

    // Retrieve the Viewport
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);


    double modelview[16];                 // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);       // Retrieve The Modelview Matrix

    double projection[16];                // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
    glGetDoublev(GL_PROJECTION_MATRIX, projection);     // Retrieve The Projection Matrix

    float x = (float) mouse_x;
    float y = (float) mouse_y;
    float z;

    // reverse y, since screen y-axis is opposite of OpenGL y-axis coordinate
    y = (float)viewport[3] - y;

    // getting the z coordinate
    glReadPixels(x, (int)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

    // convert it to OpenGL world coordinate
    double posX, posY, posZ;
    gluUnProject( x, y, z, modelview, projection, viewport, &posX, &posY, &posZ);


    // calculating the ray from Camera to the Mouse Picked Ray
    btVector3 rayTo = btVector3(posX - rayFrom.x(), posY - rayFrom.y(), posZ - rayFrom.z());

    // Scale it by 10000
    rayTo = rayTo * 10000;

    return rayTo;

}



void game::removePickingConstraint()
{
    if (m_pickConstraint_M && world)
    {
        world->removeConstraint(m_pickConstraint_M);
        delete m_pickConstraint_M;
        m_pickConstraint_M = 0;
        pickedBody_M->forceActivationState(ACTIVE_TAG);
		pickedBody_M->setDeactivationTime( 0.f );
		pickedBody_M = 0;
    }
}
