#include "gl_functions.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.1415926
#endif


#define HEIGHT_MAP_SIZE     0.1
#define HEIGHT_MAP_HEIGHT   3

#include <iostream>
#include <vector>
using namespace std;

bool free_move = true;
char* displayed_text = "MARTIN\nMARTIN MARTIN LEARNING OPENGL\nYOUR MOM";

float camX = 0.0, camY = 0.0, camZ=5.0;
float camYaw = 0.0;
float camPitch = 0.0;

enum {  SKY_LEFT = 0,
        SKY_BACK,
        SKY_RIGHT,
        SKY_FRONT,
        SKY_TOP,
        SKY_BOTTOM};
unsigned int skybox[6];
// mouse is in the window (The user clicked into the window)
bool mousein = false;

//enum
// pi may not necessarily defined in <cmath> file



void GL_function::initskybox()
{
    //skybox[SKY_LEFT] = loadTexture("./images/Skybox pictures/Skybox_Left_512.bmp",1);
    skybox[SKY_LEFT] = loadTexture("./images/Skybox pictures/Skybox1_Left.bmp",1);
    if(skybox[SKY_LEFT] == -1)
    {
        cout << "Load skybox left failed " << endl;
    }

    //skybox[SKY_BACK] = loadTexture("./images/Skybox pictures/Skybox_Back_512.bmp",1);
    skybox[SKY_BACK] = loadTexture("./images/Skybox pictures/Skybox1_Back.bmp",1);
    if(skybox[SKY_BACK] == -1)
    {
        cout << "Load skybox left failed " << endl;
    }

    //skybox[SKY_RIGHT] = loadTexture("./images/Skybox pictures/Skybox_Right_512.bmp",1);
    skybox[SKY_RIGHT] = loadTexture("./images/Skybox pictures/Skybox1_Right.bmp",1);
    if(skybox[SKY_RIGHT] == -1)
    {
        cout << "Load skybox left failed " << endl;
    }

    // skybox[SKY_FRONT] = loadTexture("./images/Skybox pictures/Skybox_Front_512.bmp",1);
    skybox[SKY_FRONT] = loadTexture("./images/Skybox pictures/Skybox1_Front.bmp",1);
    if(skybox[SKY_FRONT] == -1)
    {
        cout << "Load skybox left failed " << endl;
    }

    //skybox[SKY_TOP] = loadTexture("./images/Skybox pictures/Skybox_Top_512.bmp",1);
    skybox[SKY_TOP] = loadTexture("./images/Skybox pictures/Skybox1_Top.bmp",1);
    if(skybox[SKY_TOP] == -1)
    {
        cout << "Load skybox left failed " << endl;
    }

    //skybox[SKY_BOTTOM] = loadTexture("./images/Skybox pictures/Skybox_Bottom_512.bmp",1);
    skybox[SKY_BOTTOM] = loadTexture("./images/Skybox pictures/Skybox1_Bottom.bmp",1);
    if(skybox[SKY_BOTTOM] == -1)
    {
        cout << "Load skybox left failed " << endl;
    }
}

void GL_function::killskybox()
{
    glDeleteTextures(6, &skybox[0]);
}


void GL_function::drawSkybox(float size)
{
    // can't use glBindTexture in between glBegin and glEnd
    // so bind 6 separate textures

    // back face


    bool b1 = glIsEnabled(GL_TEXTURE_2D);
    bool b2 = glIsEnabled(GL_LIGHTING);
    glDisable(GL_LIGHTING);
  // glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_BACK]);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex3f(size/2, size/2, size/2);
        glTexCoord2f(1,0);
        glVertex3f(-size/2, size/2, size/2);
        glTexCoord2f(1,1);
        glVertex3f(-size/2, -size/2, size/2);
        glTexCoord2f(0,1);
        glVertex3f(size/2, -size/2, size/2);
    glEnd();


    // left face
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_LEFT]);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex3f(-size/2, size/2, size/2);
        glTexCoord2f(1,0);
        glVertex3f(-size/2, size/2, -size/2);
        glTexCoord2f(1,1);
        glVertex3f(-size/2, -size/2, -size/2);
        glTexCoord2f(0,1);
        glVertex3f(-size/2, -size/2, size/2);
    glEnd();


    // front face
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_FRONT]);
    glBegin(GL_QUADS);
        glTexCoord2f(1,0);
        glVertex3f(size/2,size/2,-size/2);
        glTexCoord2f(0,0);
        glVertex3f(-size/2,size/2,-size/2);
        glTexCoord2f(0,1);
        glVertex3f(-size/2,-size/2,-size/2);
        glTexCoord2f(1,1);
        glVertex3f(size/2,-size/2,-size/2);
    glEnd();


    // right face
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_RIGHT]);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex3f(size/2, size/2, -size/2);
        glTexCoord2f(1,0);
        glVertex3f(size/2, size/2, size/2);
        glTexCoord2f(1,1);
        glVertex3f(size/2, -size/2, size/2);
        glTexCoord2f(0,1);
        glVertex3f(size/2, -size/2, -size/2);
    glEnd();


    // top face
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_TOP]);
    glBegin(GL_QUADS);
        glTexCoord2f(1,0);
        glVertex3f(size/2,size/2,size/2);
        glTexCoord2f(0,0);
        glVertex3f(-size/2,size/2,size/2);
        glTexCoord2f(0,1);
        glVertex3f(-size/2,size/2,-size/2);
        glTexCoord2f(1,1);
        glVertex3f(size/2,size/2,-size/2);
    glEnd();

    // bottom face
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_BOTTOM]);
    glBegin(GL_QUADS);
        glTexCoord2f(1,1);
        glVertex3f(size/2, -size/2, size/2);
        glTexCoord2f(0,1);
        glVertex3f(-size/2, -size/2, size/2);
        glTexCoord2f(0,0);
        glVertex3f(-size/2, -size/2, -size/2);
        glTexCoord2f(1,0);
        glVertex3f(size/2, -size/2, -size/2);
    glEnd();


 //   glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    if(!b1)
        glDisable(GL_TEXTURE_2D);
    if(b2)
        glIsEnabled(GL_LIGHTING);
}



void GL_function::Compile_tri_quad(int & objID1, int & objID2)
{
 // this will compile the list, triangle is just the ID of this list
    glNewList(objID1, GL_COMPILE);
        glBegin( GL_TRIANGLES );
            // colors are interpolated linearly
            // if you define a color at a vertex
            glColor3f(1.0, 0.0, 0.0); glVertex3f(0.0, 2.0, -5 );
            glColor3f(0.0, 1.0, 0.0); glVertex3f(-2.0, -2.0, -5 );
            glColor3f(0.0, 0.0, 1.0); glVertex3f(2.0, -2.0, -5);
        glEnd();
    glEndList();

 //   glColor3f(1.0, 0.5f, 0.5f);
    glNewList(objID2, GL_COMPILE);
        glBegin( GL_QUADS );
            // colors are interpolated linearly
            // if you define a color at a vertex

            // left Upper
            glTexCoord2f(0.0, 1.0);
            glVertex3f(-2.0, 2.0, 0.0 );

            // left Bottom
            glTexCoord2f(0.0, 0.0);
            glVertex3f(-2.0, -2.0,0.0 );

            // left Upper
            glTexCoord2f(1.0, 0.0);
            glVertex3f(2.0, -2.0, 0.0);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(2.0, 2.0, 0.0);
        glEnd();
    glEndList();
}

void GL_function::lockCamera()
{
    //http://www-rohan.sdsu.edu/~stewart/cs583/LearningXNA3_figs/Fig11-5_YawPitchRoll_usingCameraUpDir.png
    // keeping the pitch between -90 and 90
    // so when you look up and look down, it's between straight up and straight down
    if(camPitch > 90)
        camPitch = 90;
    if(camPitch < -90)
        camPitch = -90;

    // always keeping the camYaw between 360 and 360
    if(camYaw < 0.0)
        camYaw += 360.0;
    if(camYaw > 360.0)
        camYaw -= 360;
}


void GL_function::moveCamera(float dist, float dir)
{
    // dir: w s a d, depending on direction key
    // radians
    float rad = (camYaw + dir) * M_PI / 180.0;
    // sin and cos are calcuated in radians in C++
    camX -= sin(rad)*dist;
    camZ -= cos(rad)*dist;
}


void GL_function::moveCameraUp(float dist, float dir)
{
    float rad=(camPitch + dir)*M_PI/180.0;
    camY+=sin(rad)*dist;
}




// move velocity,
// mouse velocity (for shooting in FPS)
// whether the mouse in the game, MouseIN
void GL_function::Control(float move_vel, float mouse_vel, bool mouse_in)
{
    // if mouse is in the window
    if(mouse_in)
    {
        // center of the window
        // MidX MidY = 640/2, 480/2
        int MidX = 320;
        int MidY = 240;

        // hide the cursor
        SDL_ShowCursor(SDL_DISABLE);
        int tmpx, tmpy;

        // fill the current mouse position in tmpx and tmpy
        SDL_GetMouseState(&tmpx, &tmpy);

        // mouse_vel is pretty much mouse sensitivty where you change the settings of it
        // camYaw += mouse_vel * (MidX - tmpx);
        camYaw += mouse_vel * (MidX - tmpx);
        camPitch += mouse_vel * (MidY - tmpy);
        lockCamera();

        // places the curosr at the center
        SDL_WarpMouse(MidX, MidY);
        Uint8* state = SDL_GetKeyState(NULL);


/*
in free_Move mode
1. pressing w,s does not change the pitch angle, but gives a direction like ( +1 or -1 )
so it passes 0.0 or 180 into movecameraUp to see whether camY increases or decrease
so:
SDLK_w - 0.0 to moveCameraUp
SDLK_s - 180 to moveCameraDown
*/
        if(state[SDLK_w])
        {
            // if your 3D camera is moves in 3D (under water) use free_move mode
            if (free_move)
            {
                if(camPitch!=90 && camPitch != -90)
                    moveCamera(move_vel, 0.0);
                // camY moves up, so 0.0
                moveCameraUp(move_vel, 0.0);
            }
            else
                moveCamera(move_vel, 0.0);
        }
        else if(state[SDLK_s])
        {
            if (free_move)
            {
                if(camPitch!=90 && camPitch != -90)
                    moveCamera(move_vel, 180.0);
                // camY moves down, so 180.0
                moveCameraUp(move_vel, 180.0);
            }
            else moveCamera(move_vel, 180.0);
        }

        // left
        if(state[SDLK_a])
        {
            moveCamera(move_vel, 90.0);
        }
        // right
        else if(state[SDLK_d])
        {
            moveCamera(move_vel, 270);
        }
    }
    // the camera itself doesn't move, but we move everything else in opposite direction
    // the glRotatef function rotates to angle "camPitch", not rotating camPitch degress
    glRotatef(-camPitch, 1.0, 0.0, 0.0);
    glRotatef(-camYaw, 0.0, 1.0, 0.0);
}
void GL_function::UpdateCamera()
{
    // the camera can't move, so move everything else
    glTranslatef(-camX, -camY, -camZ);
}


void GL_function::drawCube(float size)
{
//    float difamb[] ={1.0, 0.5, 0.3, 1.0};
    glBegin(GL_QUADS);
        // OpenGL use normal vectors to calculate light
        // if we don't ENABLE(COLOR_MATERIAL)

        // we can manually create a material color here
//       glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, difamb);

        // front face
        glNormal3f(0.0,0.0,1.0);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(size/2, size/2, size/2);
        glVertex3f(-size/2, size/2, size/2);
        glVertex3f(-size/2, -size/2, size/2);
        glVertex3f(size/2, -size/2, size/2);

        // back face
        glNormal3f(0.0,0.0,-1.0);
        glColor3f(0.0,0.0,1.0);
        glVertex3f(size/2, size/2, -size/2);
        glVertex3f(-size/2, size/2, -size/2);
        glVertex3f(-size/2, -size/2, -size/2);
        glVertex3f(size/2, -size/2, -size/2);

        // left face
        glNormal3f(-1.0,0.0,0.0);
        glColor3f(0.0,1.0,0.0);
        glVertex3f(-size/2, size/2, size/2);
        glVertex3f(-size/2, size/2, -size/2);
        glVertex3f(-size/2, -size/2, -size/2);
        glVertex3f(-size/2, -size/2, size/2);

        // right face
        glNormal3f(1.0,0.0,0.0);
        glColor3f(1.0,1.0,0.0);
        glVertex3f(size/2, size/2, -size/2);
        glVertex3f(size/2, size/2, size/2);
        glVertex3f(size/2, -size/2, size/2);
        glVertex3f(size/2, -size/2, -size/2);

        // top face
        glNormal3f(0.0,1.0,0.0);
        glColor3f(1.0,0.0,1.0);
        glVertex3f(size/2, size/2, size/2);
        glVertex3f(-size/2, size/2, size/2);
        glVertex3f(-size/2, size/2, -size/2);
        glVertex3f(size/2, size/2, -size/2);

        // bottom face
        glNormal3f(0.0,-1.0,0.0);
        glVertex3f(size/2, -size/2, size/2);
        glVertex3f(-size/2, -size/2, size/2);
        glVertex3f(-size/2, -size/2, -size/2);
        glVertex3f(size/2, -size/2, -size/2);
    glEnd();

}


void GL_function::draw_ALL_IN_ONE_Cube(float size, unsigned int & id)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        // top face
        glNormal3f(0.0,1.0,0.0);
        glColor3f(1.0,0.0,1.0);
        glVertex3f(size/2, size/2, size/2);
        glVertex3f(-size/2, size/2, size/2);
        glVertex3f(-size/2, size/2, -size/2);
        glVertex3f(size/2, size/2, -size/2);

        // bottom face
        glNormal3f(0.0,-1.0,0.0);
        glColor3f(0.0,1.0,1.0);
        glVertex3f(size/2, -size/2, size/2);
        glVertex3f(-size/2, -size/2, size/2);
        glVertex3f(-size/2, -size/2, -size/2);
        glVertex3f(size/2, -size/2, -size/2);


        // left face
        glNormal3f(-1.0,0.0,0.0);
        glColor3f(0.0,1.0,0.0);
        glVertex3f(-size/2, size/2, size/2);
        glVertex3f(-size/2, size/2, -size/2);
        glVertex3f(-size/2, -size/2, -size/2);
        glVertex3f(-size/2, -size/2, size/2);

        // right face
        glNormal3f(1.0,0.0,0.0);
        glColor3f(1.0,1.0,0.0);
        glVertex3f(size/2, size/2, -size/2);
        glVertex3f(size/2, size/2, size/2);
        glVertex3f(size/2, -size/2, size/2);
        glVertex3f(size/2, -size/2, -size/2);

    glEnd();


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, id);
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_QUADS);
        // signal to use the texture
        // front face
        glNormal3f(0.0,0.0,1.0);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-size/2, -size/2, size/2);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(size/2, -size/2, size/2);
    	glTexCoord2f(1.0f, 1.0f);
        glVertex3f(size/2, size/2, size/2);
    	glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-size/2, size/2, size/2);

        // back face
        glNormal3f(0.0,0.0,-1.0);
    	glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-size/2, -size/2, -size/2);
    	glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-size/2, size/2, -size/2);
    	glTexCoord2f(1.0f, 1.0f);
        glVertex3f(size/2, size/2, -size/2);
    	glTexCoord2f(0.0f, 1.0f);
        glVertex3f(size/2, -size/2, -size/2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}



void GL_function::init_Alpha_Blending()
{
    //(A*source) + (B*Destination)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}






/* tutorial 16 17 */

bool GL_function::raysphere(float xc, float yc, float zc,   // center coord of sphere
                            float xd, float yd, float zd,   // direction vector of the ray
                            float xs, float ys, float zs,   // start point of the ray
                            float r,                        // radius of the sphere, and default argument
                            float* dist_time,         // distance between the collision
                            coordinate* collision_point)    // position, where the collision happens
{
    // solving the sphere equation
    // formula ax^2 + bx + c = 0;
    float b = 2*(xd*(xs-xc)+yd*(ys-yc)+zd*(zs-zc));

    float c = xs*xs-2*xs*xc+xc*xc +
              ys*ys-2*ys*yc+yc*yc +
              zs*zs-2*zs*zc+zc*zc - r*r;

    // discriminant
    float disc = (b*b-4*c);

    // if less than zero, no collision, return false
    if(disc < 0)
        return false;

    // then we have collision
    // we inner_dist_time and point are NULL(we don't pass these two arguments)
    // then we don't care this part, just return true straight away
    // if we do pass these two arguments, then we want to find out where the collision happened
    if(dist_time != NULL)
    {
        // set distance, (-b + sqrt(b*b - 4*c))/2
        (*dist_time) = (-b + disc)/2;
        if(collision_point != NULL)
        {
            // calculating the collision point
            // xs + t*xd
            collision_point->x = xs + (*dist_time) * xd;
            collision_point->y = ys + (*dist_time) * yd;
            collision_point->z = zs + (*dist_time) * zd;
        }
    }
    //(x+y)^2=x^2+2xy+y^2
    return true;

}



// we have to change the camera
void GL_function::moveTo(coordinate c)
{
    camX = c.x;
    camY = c.y;
    camZ = c.z;
}





/*
void GL_function::loadAnimation(vector<unsigned int>& frames, string filename,
            unsigned int num_frame, objloader& obj)
{


    char tmp[200];
    unsigned int id;
    // from 1 to 250 because the filenames are written that way
    for(int i=1; i<= num_frame; i++)
    {
    //    if(i<10)
        sprintf(tmp, "_%06d.obj", i);
    //    else if (i<100)

        string tmp2(filename+tmp);
 //       tmp2 += ".obj";
        cout << tmp2 << endl;
      //  id = obj.load("./3D_models/sphere_radius1.obj");
        unsigned int id = obj.load(tmp2);
        frames.push_back(id);
    }



        char tmp[200];
        for(int i=1;i<=num_frame;i++)
        {
                if(i<10)
                        sprintf(tmp,"_00000%d",i);
                else if(i<100)
                        sprintf(tmp,"_0000%d",i);
                else if(i<1000)
                        sprintf(tmp,"_000%d",i);
                else if(i<10000)
                        sprintf(tmp,"_00%d",i);
                else if(i<100000)
                        sprintf(tmp,"_0%d",i);
                else if(i<1000000)
                        sprintf(tmp,"_%d",i);
                std::string tmp2(filename+tmp);
                tmp2+=".obj";
                std::cout << tmp2 << std::endl;
    //            unsigned int id = obj.load(tmp2);
                unsigned int id = obj.load("./3D_models/sphere_radius1.obj");
                frames.push_back(id);
        }

}
*/

void GL_function::loadHeightmap(const char* name, vector<vector<float> >& heights)
{
    SDL_Surface* img = SDL_LoadBMP(name);
    if(!img)
    {
        cout << "heightmap img not loaded" << endl;
        return;
    }

    // temp vector to push into heights
    vector<float> temp1;
    // going through row
    for(int i = 0; i<img->h; i++)
    {
        temp1.clear();
        // going through each column
        for(int j=0; j<img->w; j++)
        {
            uint32_t pixel = ((uint32_t*)img->pixels)[i*img->pitch/4 + j];
            uint8_t r,g,b;
            SDL_GetRGB(pixel, img->format,&r,&g,&b);
            temp1.push_back((float)r/255.0);    // normalize it so r is between 0 and 1
        }
        heights.push_back(temp1);
    }

}

// on a gird map, size is the size of the grid

void GL_function::renderHeightmap(float size, float h, vector<vector<float> >& heights)
{
    for(int i=0; i<heights.size()-1; i++)
    {
        for(int j=0; j<heights[0].size()-1; j++)
        {
            // Triangle Strips can have different heights
            glBegin(GL_TRIANGLE_STRIP);
                glColor3f(heights[i][j],heights[i][j],heights[i][j]);
                // look at 15:40 and 16:25 of tutorial 21 height map for this
                glVertex3f(i*size, heights[i][j]*h, j*size);
                glVertex3f((i+1)*size, heights[i+1][j]*h, j*size);
                glVertex3f(i*size, heights[i][j+1]*h, (j+1)*size);
                glVertex3f((i+1)*size, heights[i+1][j+1]*h, (j+1)*size);
            glEnd();
        }
    }
}


float GL_function::triangle_area(coordinate p1, coordinate p2, coordinate p3)
{
    // basic trig, distance between two points in 3D axis
    //area of the triangle with the heron fomula
    float a=sqrt((p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y)+(p2.z-p1.z)*(p2.z-p1.z));
    float b=sqrt((p3.x-p2.x)*(p3.x-p2.x)+(p3.y-p2.y)*(p3.y-p2.y)+(p3.z-p2.z)*(p3.z-p2.z));
    float c=sqrt((p1.x-p3.x)*(p1.x-p3.x)+(p1.y-p3.y)*(p1.y-p3.y)+(p1.z-p3.z)*(p1.z-p3.z));

    float s=(a+b+c)/2.;

    return (sqrt(s*((s-a)*(s-b)*(s-c))));
}



/* tutorial testing functions */


void GL_function::test_textures_7(int textureID, int angle)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0.0,0.0,-5.0);
        glRotatef(angle,1.0,1.0,1.0);   // angle, x-axis, y-axis, z-axis
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBegin(GL_QUADS);
                glTexCoord2f(0.0,2.0);
                glVertex3f(-2.0,2.0,0.0);
                glTexCoord2f(0.0,0.0);
                glVertex3f(-2.0,-2.0,0.0);
                glTexCoord2f(2.0,0.0);
                glVertex3f(2.0,-2.0,0.0);
                glTexCoord2f(2.0,2.0);
                glVertex3f(2.0,2.0,0.0);
        glEnd();

}






void GL_function::test_glPushMatrix_10(int objID, int angle)
{
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        float pos[]={-1.0,1.0,-2.0,1.0};
        glLightfv(GL_LIGHT0,GL_POSITION,pos);
     // this saves the Matrix (your specific transformation)
        glPushMatrix();
            glTranslatef(-2.0,0.0,-10.0);
            glRotatef(angle, 1.0,1.0,1.0);

            // as soon you enable the texture
            // the byte type you enabled in image2D function affects
            // everything else
            glCallList(objID);   // draw the 3D mesh
        // this restores the identity matrix
        glPopMatrix();


        glTranslatef(2.0,0.0,-8.0);
        glRotatef(-angle, 1,1,1);
        glCallList(objID);


}






void GL_function::test_OBJ_loader_12_13(int objID, int angle)
{

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        float pos[]={-1.0,1.0,-2.0,1.0};
        glLightfv(GL_LIGHT0,GL_POSITION,pos);

        glPushMatrix();
            glTranslatef(0.0, 0.0,-5.0);
            glRotatef(angle, 1.0,1.0,1.0);

            // as soon you enable the texture
            // the byte type you enabled in image2D function affects
            // everything else
            glCallList(objID);   // draw the 3D mesh
        // this restores the identity matrix
        glPopMatrix();




}


void GL_function::test_3D_FPS_camera_14(int objID, bool mi)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    Control(0.2, 0.2, mi);
    UpdateCamera();
    float pos[]={-1.0,1.0,-2.0,1.0};
    glLightfv(GL_LIGHT0,GL_POSITION,pos);

  //  glPushMatrix();
  //      glTranslatef(0.0, 0.0,-5.0);
        glCallList(objID);   // draw the 3D mesh
  //  glPopMatrix();

}


void GL_function::test_skybox_15(int objID, bool mi, int angle)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    Control(0.2, 0.2, mi);

    drawSkybox(50.0);
    UpdateCamera();
    float pos[]={-1.0,1.0,-2.0,1.0};
    glLightfv(GL_LIGHT0,GL_POSITION,pos);

//    glCallList(objID);   // draw the 3D mesh


    glPushMatrix();
        glTranslatef(0.0, 0.0,-5.0);
        glRotatef(angle, 0.0,1.0,0.0);

        // as soon you enable the texture
        // the byte type you enabled in image2D function affects
        // everything else
        glCallList(objID);   // draw the 3D mesh
    // this restores the identity matrix
    glPopMatrix();



}

void GL_function::test_ray_tracing_sphere_16(int objID, bool mi, int angle, coordinate & sphere_center, coordinate & raystart)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    Control(0.2, 0.2, mi);

    drawSkybox(50.0);
    UpdateCamera();
    float pos[]={-1.0,1.0,-2.0,1.0};
    glLightfv(GL_LIGHT0,GL_POSITION,pos);

    glDisable(GL_LIGHTING);
    // if there's collision
    if(raysphere(sphere_center.x,
                sphere_center.y,
                sphere_center.z,
                0.0, 0.0, 1.0,          // first set the direction vector parallel to the z plane
                raystart.x, raystart.y, raystart.z,
                1.0))                   // radius of the blender model is 1
    {
        glColor3f(1.0, 0.0, 0.0);
    }
    else
        glColor3f(1.0, 1.0, 1.0);



    // draw a line as trajectory
    glBegin(GL_LINES);
        // Xs + t * Xd
        glVertex3f(raystart.x, raystart.y, raystart.z);
        glVertex3f(raystart.x + 100*0.0,
                    raystart.y + 100*0.0,
                    raystart.z + 100*1.0);
    glEnd();
    glEnable(GL_LIGHTING);

    glPushMatrix();
    //    glTranslatef(0.0, 0.0,-5.0);
        glRotatef(angle, 0.0,1.0,0.0);
        // as soon you enable the texture
        // the byte type you enabled in image2D function affects
        // everything else
  //      glColor3f(1.0, 1.0, 1.0);
        glCallList(objID);   // draw the 3D mesh
    // this restores the identity matrix
    glPopMatrix();

    // after using the texture, we need to, set color buffer back to full color
    glColor3f(1.0, 1.0, 1.0);
}



/*
unsigned int GL_function::loadTexture(string filename, int background, bool generate)
{

    std::ifstream in(filename.c_str());

    if(!in.is_open())
    {
        std::cout << "Nor oepened" << std::endl;
        return -1;
    }

        unsigned int num;
        glGenTextures(1,&num);
        SDL_Surface* img=IMG_Load(filename.c_str());
        if(img==NULL)
        {
                std::cout << "img was not loaded" << std::endl;
                return -1;
        }
        SDL_PixelFormat form={NULL,32,4,0,0,0,0,0,0,0,0,0xff000000,0x00ff0000,0x0000ff00,0x000000ff,0,255};
        SDL_Surface* img2=SDL_ConvertSurface(img,&form,SDL_SWSURFACE);
        if(img2==NULL)
        {
                std::cout << "img2 was not loaded" << std::endl;
                return -1;
        }
        glBindTexture(GL_TEXTURE_2D,num);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        //opengl 1.4 way:
        //if(generate)
        //      glTexParameteri(GL_TEXTURE_2D,GL_GENERATE_MIPMAP,GL_TRUE);

        //openGL 1.1 way:

    //    if(generate)
    //            gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,img2->w,img2->h,GL_RGBA,GL_UNSIGNED_INT_8_8_8_8,img2->pixels);
    //    else
    //            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img2->w,img2->h,0,GL_RGBA,GL_UNSIGNED_INT_8_8_8_8,img2->pixels);

        if(generate)
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        else
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

        if(generate)
              glGenerateMipmap(GL_TEXTURE_2D);        //OpenGL 3.0+ way

        SDL_FreeSurface(img);
        SDL_FreeSurface(img2);
        return num;
}
*/




// OpenGL has no function to load an image
unsigned int GL_function::loadTexture(string filename, int background, bool generate)
{

    std::ifstream in(filename.c_str());

    if(!in.is_open())
    {
        std::cout << "Nor oepened" << std::endl;
        return -1;
    }

    string path = filename.substr(0,(filename.find_last_of('/') != string::npos ?
            filename.find_last_of('/')+1 : 0));

    unsigned int num;
    glGenTextures(1,&num);
    SDL_Surface* img = IMG_Load(filename.c_str());
    if(img == NULL)
    {
        cout << "Loadtexture img failed" << endl;
        return -1;
    }

    SDL_PixelFormat pixel_format = {NULL,
                                    32,             // converting to 32 bit pixel
                                    4,              // number of bytes
                                    0,0,0,0,        // byte lost
                                    0,0,0,0,        // byte shift
                                    0xff000000,     // red mask
                                    0x00ff0000,     // green mask
                                    0x0000ff00,     // blue mask
                                    0x000000ff,     // alpha mask
                                    0,              // color code
                                    255};           // alpha code

    // SDL_SWSURFACE means it's a software surface so we don't store it
    // in the video card
    // we're converting im2 to into this new SDL_PixelFormat format
    SDL_Surface* img2 = SDL_ConvertSurface(img, &pixel_format ,SDL_SWSURFACE);
    if(img2 == NULL)
    {
        cout << "img2 was not loaded" << std :: endl;
        return -1;
    }


    // tell OpenGL we want to use this texture
    glBindTexture(GL_TEXTURE_2D,num);       //and use the texture, we have just generated




    // load to video card
    // http://stackoverflow.com/questions/9296503/texture-has-only-blue-hue-when-using-glteximage2d
    // 2nd argumentL level. Level n is the nth mipmap reduction image



/*
    if(generate)
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
*/


    //    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,img->w,img->h,GL_RGBA,GL_UNSIGNED_INT_8_8_8_8,img2->pixels);
    // no else
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img->w,img->h,0,GL_RGBA,GL_UNSIGNED_INT_8_8_8_8,img2->pixels);        //we make the actual texture
//    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img->w,img->h,0,GL_BGR,GL_UNSIGNED_BYTE,img->pixels);        //we make the actual texture


    if(generate)
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); //if the texture is smaller, than the image, we get the avarege of the pixels next to it
    else
        // no need to Magnify the MAG_FILTER
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);


    // if you comment these two lines out, you will see the edges of the cube
    if(background)
    {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);      //we repeat the pixels in the edge of the texture, it will hide that 1px wide line at the edge of the cube, which you have seen in the video
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);      //we do it for vertically and horizontally (previous line)
    }

    // openGL 3.0
    if(generate)
        glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(img);   //we delete the image, we don't need it anymore
    SDL_FreeSurface(img2);
    return num;
}




