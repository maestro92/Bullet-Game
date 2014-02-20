
#ifndef _GL_FUNCTION_H_
#define _GL_FUNCTION_H_


#include <cstdlib>
#include <vector>
#include <iostream>
#include <sstream> //You'll need to include this header to use 'std::stringstream'.
#include <string>
#include <algorithm>
#include <fstream>
#include <cstdio>



#define NO_SDL_GLEXT
#include <GL/glew.h>

#include "gl/gl.h"
#include "gl/glu.h"
#include "gl/glext.h"


#include "SDL\SDL.h"
#include "SDL\SDL_image.h"
#include "objloader.h"
#include "text.h"

// #include "gl/Glee.h"

using namespace std;


extern unsigned int tex;



class GL_function
{
    public:
        static void Compile_tri_quad(int & objID1, int & objID2);

 //       static unsigned int loadTexture(const char* filename);
        static unsigned int loadTexture(string filename, int background = 0, bool generate=false);

        static void drawCube(float size);
        static void draw_ALL_IN_ONE_Cube(float size, unsigned int & id);

        /* tutorial 11*/
        static void init_Alpha_Blending();


        /* tutorial 15 */
        static void initskybox();
        static void killskybox();
        static void drawSkybox(float size);

        /* tutorial 16 17 */
        static bool raysphere(float xc, float yc, float zc, // center coord of sphere
                            float xd, float yd, float zd,   // direction vector
                            float xs, float ys, float zs,   // start point of the ray
                            float r, float* dist_time = NULL,    // radius of the sphere, and default argument
                            coordinate* collision_point = NULL);

        static bool rayplane(float nx, float ny, float nz,
                            float xs, float ys, float zs,
                            float xd, float yd, float zd,
                            coordinate p1,
                            coordinate p2,
                            coordinate p3,
                            coordinate p4,
                            float* dist_time = NULL,
                            coordinate* collision_point = NULL);


        static float triangle_area(coordinate p1, coordinate p2, coordinate p3);

        /* tutorial 18 */

        static bool spheresphere(coordinate& c1,float r1,coordinate c2,float r2);

        static float pointdistance(coordinate c1, coordinate c2);

        static bool sphereplane(coordinate& sp, coordinate vn, coordinate p1, coordinate p2,
                                                        coordinate p3, coordinate p4, float r);

        static coordinate camPos();
        static void moveTo(coordinate c);

        /* tutorial 20 */
        // the vector will contain all the id of the animation frames
        static void loadAnimation(vector<unsigned int>& frames, string filename,
                    unsigned int num_frame, objloader& obj);


        /* tutorial 21 */
        static void loadHeightmap(const char* name, vector<vector<float> >& heights);
        static void renderHeightmap(float size, float h, vector<vector<float> >& heights);




        /* to ensure the camera pitch stays with 90 -90
           to keep the cameraYaw between 0 and 360
        */
        static void lockCamera();

        /* */
        static void moveCamera(float, float);

        /* */
        static void moveCameraUp(float, float);

        static void Control(float, float, bool);

        static void UpdateCamera();





        /* all test files */

        static void test_textures_7(int textureID, int angle);

        static void test_glPushMatrix_10(int objID, int angle);

        static void test_Alpha_Blending_11();

        static void test_OBJ_loader_12_13(int objID, int angle);

        static void test_3D_FPS_camera_14(int objID, bool mi);

        static void test_skybox_15(int objID, bool mi, int angle);

        static void test_ray_tracing_sphere_16(int objID, bool mi, int angle,
                                        coordinate & sphere_center, coordinate & raystart);

        static void test_ray_tracing_plane_17(int objID, bool mi, int angle, coordinate & sphere_center,
                        coordinate & raystart, coordinate & p1, coordinate & p2,
                                                coordinate & p3, coordinate & p4);

        static void test_sphere_and_plane_18(int objID, bool mi, int angle, coordinate & sphere_center,
                        coordinate & raystart, coordinate & p1, coordinate & p2,
                                                coordinate & p3, coordinate & p4);

        static void test_3D_text_19(int objID, bool mi, int angle, coordinate & sphere_center,
                        coordinate & raystart, coordinate & p1, coordinate & p2,
                                                coordinate & p3, coordinate & p4, text* mytext);

        static void test_animation_20(int objID, bool mi, int angle, coordinate & sphere_center,
                        coordinate & raystart, coordinate & p1, coordinate & p2,
                                                coordinate & p3, coordinate & p4, text* mytext,
                                                vector<unsigned int>& frames, int & curframe, int & num_frames);


        static void test_heightmap_21(int objID, bool mi, int angle, coordinate & sphere_center,
                        coordinate & raystart, coordinate & p1, coordinate & p2,
                                                coordinate & p3, coordinate & p4, text* mytext,
                                                vector<unsigned int>& frames, int& curframe, int & num_frames,
                                                vector<vector<float> >& heights);

        static void test_imporved_loadTexture_22(int objID, bool mi, int angle, coordinate & sphere_center,
                        coordinate & raystart, coordinate & p1, coordinate & p2,
                                                coordinate & p3, coordinate & p4, text* mytext,
                                                vector<unsigned int>& frames, int& curframe, int & num_frames,
                                                vector<vector<float> >& heights, unsigned int textureID);

        static void test_mipmap_23(int objID, bool mi, int angle, coordinate & sphere_center,
                        coordinate & raystart, coordinate & p1, coordinate & p2,
                                                coordinate & p3, coordinate & p4, text* mytext,
                                                vector<unsigned int>& frames, int& curframe, int & num_frames,
                                                vector<vector<float> >& heights, unsigned int textureID);

        // vertex buffer object
        static void test_VBO_and_vertex_array_24(int objID, bool mi, int angle, coordinate & sphere_center,
                        coordinate & raystart, coordinate & p1, coordinate & p2,
                                                coordinate & p3, coordinate & p4, text* mytext,
                                                vector<unsigned int>& frames, int& curframe, int & num_frames,
                                                vector<vector<float> >& heights, unsigned int textureID,
                                                unsigned int& vbo, float f1[]);


        static void test_GLSL_1(bool mi);

        // GLSL

//        static void test_sphere_sphere_collision_18(int objID, bool mi, int angle);
/*
        static void test_sphere_plane_collision_detection_18(int objID, bool mi, int angle,
                                                                int
   */
};


/*
http://stackoverflow.com/questions/6733934/what-does-immediate-mode-mean-in-opengl

    one example of using "immedage mode" is using glBegin and glEnd with
glVertex in between them, or glDrawArrays

immediate mode is not optimized because it's linked directly with your program's flow.

*/





#endif



