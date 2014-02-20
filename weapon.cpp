#include "weapon.h"

// frame animation length
// 1 frame for normal
// 16 frames for fire
// 20 frames for reload

weapon::weapon(vector<unsigned int>& anim,
                unsigned int outside_view,
                unsigned int normal_anim,
                unsigned int fire_anim,
                unsigned int reload_anim,
                vector3d pos,
                vector3d rot,
                vector3d aim_pos,
                vector3d aim_rot,
                float prec,
                float aim_prec,
                int weapon_damage,
                unsigned int max_bullets,
                unsigned int all_bullets,
                unsigned int _speed,
                const char* weapon_name,
                bool isauto)
{

    frames = anim;
    outerview = outside_view;

    normal_animation = normal_anim;
    fire_animation = fire_anim;
    reload_animation = reload_anim;

    precision = (prec!=0 ? prec : 0.00001);
    aimprecision = (aim_prec!=0 ? aim_prec : 0.001);

    position = pos;
    rotation = rot;

    aimposition = aim_pos;
    aimrotation = aim_rot;

    damage = weapon_damage;
    allbullets = all_bullets;
    maxBulletsInMag = max_bullets;
    bulletsInMag = maxBulletsInMag;
    name = weapon_name;

    // whenver we start, we don't aim
    position_expected = position;
    rotation_expected = rotation;

    curpos = position;
    currot = rotation;

    isaim = false;
    isreloading = false;
    isautomatic = isauto;
    isfired = false;
    istest = true;

    lastshot = 1000;
    speed = _speed;

    curframe = 0;
    curmode = 1;
}

weapon::~weapon()
{

}





bool weapon::fire(vector3d& bullet_direction,
                vector3d cam_direction)
{
  //  cout << "i'm inside Weapon fire1" << endl ;
    if(isreloading)
        return false;

 //   cout << "i'm inside Weapon fire2" << endl;

 //   cout << "speed is " << speed << endl;
 //   cout << "lastshot is " << lastshot << endl;
    // if not automatic and we haven't fired
    // or it's fired
    if((!isautomatic && !isfired) || isautomatic)
    {
 //       cout << "i'm inside weapon fire" << endl;

        // speed: how many frames is needed to shoot the weapon
        // is if 10 frames have elapsed but it takes 20 frames to complete
        // a fire then we can't shoot
        if(lastshot >= speed)
        {
            // if we have bullets
            if(bulletsInMag > 0)
            {
                if(isaim)
                {
                    // the second half is that it won't be shooting exactly at the center
                    bullet_direction.x = cam_direction.x + ((float)(rand()%2-1) / aimprecision);
                    bullet_direction.y = cam_direction.y + ((float)(rand()%2-1) / aimprecision);
                    bullet_direction.z = cam_direction.z + ((float)(rand()%2-1) / aimprecision);
                }
                else
                {
                    bullet_direction.x = cam_direction.x + ((float)(rand()%2-1) / precision);
                    bullet_direction.y = cam_direction.y + ((float)(rand()%2-1) / precision);
                    bullet_direction.z = cam_direction.z + ((float)(rand()%2-1) / precision);
                }
                isfired = true;
                lastshot = 0;
                bulletsInMag--;
                curframe = normal_animation;

    //            cout << "bulletsInMage" << bulletsInMag << endl;

                curmode = FIRE_MODE;
                    cout << endl;
                return true;
            }
            // else we don't have bullets, try to reload
            else
            {
                reload();
                    cout << endl;
                return false;
            }
        }
    }
    cout << endl;
    return false;
}


void weapon::stopfire()
{
    cout << "I stopped the firing" << endl;
    isfired = false;
}

void weapon::reload()
{
    // if it's not reloading and the bullets in the clip is less than max
    if(!isreloading && maxBulletsInMag != bulletsInMag)
    {
        isreloading = true;
        // if our total bullets is more than the difference
        if(allbullets > maxBulletsInMag - bulletsInMag)
        {
            allbullets -= maxBulletsInMag - bulletsInMag;
            bulletsInMag = maxBulletsInMag;
        }
        else
        {
            bulletsInMag = allbullets + bulletsInMag;
            allbullets = 0;
        }
        curframe = normal_animation + fire_animation;
        curmode = RELOAD_MODE;

    }
}


// called to reverse the aim
void weapon::aim()
{
    isaim = !isaim;
    if(isaim)
    {
        rotation_expected = aimrotation;
        position_expected = aimposition;
    }
    else
    {
        rotation_expected = rotation;
        position_expected = position;
    }
}


void weapon::test()
{
    // to adjust the position
    if(istest)
    {
		Uint8* keys=SDL_GetKeyState(NULL);
		if(keys[SDLK_j])
		{
            cout << "j" << endl;
			position_expected.x-=0.02;
		}
		if(keys[SDLK_l])
        {
            cout << "l" << endl;
			position_expected.x+=0.02;
        }
		if(keys[SDLK_k])
        {
            cout << "k" << endl;
			position_expected.z-=0.02;
        }
		if(keys[SDLK_i])
		{
            cout << "i" << endl;
			position_expected.z+=0.02;
		}
		if(keys[SDLK_n])
        {
            cout << "k" << endl;
			position_expected.y-=0.02;
        }
		if(keys[SDLK_m])
		{
            cout << "i" << endl;
			position_expected.y+=0.02;
		}

		if(keys[SDLK_u])
		{
            cout << "u" << endl;
            rotation_expected.y-=0.2;
		}
		if(keys[SDLK_o])
		{
            cout << "o" << endl;
			rotation_expected.y+=0.2;
		}
		cout << "weapon position is " << curpos.x << " " << curpos.y << " " << curpos.z << endl;
		cout << "weapon rotation is " << currot.x << " " << currot.y << " " << currot.z << endl;
    }

}


unsigned int weapon::getOuterView()
{
    return outerview;
}

void weapon::show()
{
    // not to screw up other transformation matrix outside
    glPushMatrix();
        glTranslatef(curpos.x, curpos.y, curpos.z);
        glRotatef(currot.x,1,0,0);
        glRotatef(currot.y,0,1,0);
        glRotatef(currot.z,0,0,1);

        // this is where we show the weapon
        glCallList(frames[curframe]);
    glPopMatrix();

}




void weapon::update()
{
    // to test where is the best position for the weapon
    test();

    // increase the frame
    lastshot++;
    curframe++;

    // checking the curframe
  //  cout << "curframe is " << curframe << endl;
  //  cout << "curmode is " << curmode << endl;
  //  cout << endl;

    // normal mode
    if(curmode == 1)
    {
    //    cout << "i'm in normal mode" << endl;
        if(curframe >= normal_animation)
            curframe = 0;

        // fire mode
    }
    else if(curmode == 2)
    {
  //      cout << "i'm in fire mode" << endl;
        if(curframe > normal_animation + fire_animation)
        {
            if(isautomatic && isfired)          // May Be a bug
            {
                curframe = normal_animation;
            }
            else
            {
                curmode = 1;
                curframe = 0;
            }
        }
    }
    else if(curmode == 3)
    {
        cout << "i'm in reload mode" << endl;
        if(curframe > normal_animation + fire_animation + reload_animation)
        {
            curmode = 1;
            curframe = 0;
            isreloading = false;
        }
    }

        // a direction where the weapon should move to
        vector3d tmp_pos_Vec(position_expected - curpos);
        tmp_pos_Vec.normalize();
        tmp_pos_Vec*= 0.3;
        curpos += tmp_pos_Vec;

        // example
        // position_expected {0,0,0}
        // curpos {-0.1, -0.1, -0.1}

        // to avoid getting stuck in a loop
        if(abs(position_expected.x - curpos.x) < 0.3 &&
            abs(position_expected.y - curpos.y) < 0.3 &&
            abs(position_expected.z - curpos.z) < 0.3)

            curpos = position_expected;


        vector3d tmp_rot_Vec(rotation_expected - currot);
        tmp_rot_Vec.normalize();
        tmp_rot_Vec*= 0.3;
        currot += tmp_rot_Vec;

        // example
        // position_expected {0,0,0}
        // curpos {-0.1, -0.1, -0.1}

        // to avoid getting stuck in a loop
        if(abs(rotation_expected.x - currot.x) < 0.3 &&
            abs(rotation_expected.y - currot.y) < 0.3 &&
            abs(rotation_expected.z - currot.z) < 0.3)

            currot = rotation_expected;

   //     cout << "weapon Current position" << curpos.x;
   //     cout << " " << curpos.y;
   //     cout << " " << curpos.z << endl;
}




void weapon::addBullets(unsigned int num)
{
	allbullets+=num;
}

void weapon::setBullets(unsigned int num)
{
	allbullets=num;
}


int weapon::getDamage()
{
	return damage;
}


