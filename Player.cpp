#include "player.h"

using namespace std;

// player constructor
player::player(string n,collisionsphere sphere,
                float sprints,
                float normals,
                float looks,
                weapon* default_weapon)
{
    // name is name;
	name=n;

	// players sphere set to sphere
	player_sphere=sphere;

    sprintspeed = sprints;
    normalspeed = normals;
    lookspeed = looks;

    // set the gravity
  //  gravity.change(0.0, -0.3, 0.0);

      gravity.change(0.0, 0.0, 0.0);
    // set player initial position
    setPosition(vector3d(sphere.center.x, sphere.center.y, sphere.center.z));

    // set the player/camera speed
    cam.setSpeed(normalspeed, looks);
    points = 0;
    energy = 50;
    isground = iscollision = issprint = false;

    health = 100;

    weapons.push_back(default_weapon);
    cur_weapon = 0;
    isWeapon = true;
    headPosition = 0;
    Net_ID = -1;
/*
	health=100;
	curweapon=0;

	force.change(0.0,-0.3,0.0);
	direction.change(0.0,0.0,0.0);
	setPosition(vector3d(0.0,10,0));
	cam.setSpeed(speed,looks);
	sprintspeed=ss;
	normalspeed=speed;
	lookspeed=looks;
	energy=10;
	issprint=false;
	points=0;
	isWeapon=true;
	*/
}





void player::setjump()
{
    cout << "jump1" << endl;
    // if we are the ground
	if(isground)
	{
		isground=false;
		velocity.y = 2;
//		velocity.change(0.0,2,0.0);
	}
}


void player::setSprint()
{
    if(energy > 2.0)
    {
        issprint = true;
        cam.setSpeed(sprintspeed, cam.getMousevel());
    }
    else
    {
        issprint = false;
    }
}


void player::stopSprint()
{
    cout << "i'm in stop sprint" << endl;
    issprint = false;
    cam.setSpeed(normalspeed, cam.getMousevel());
}


collisionsphere player::getCollisionSphere()
{
	return player_sphere;
}

std::string player::getName()
{
	return name;
}

// set the player position
void player::setPosition(vector3d pos)
{
	player_sphere.center = pos;
	cam.setLocation(pos);
}

// update player's position
void player::update(std::vector<collisionplane>& PlaneList)
{

    {
  //  cout << "player1 position " << player_sphere.center.x << " " << player_sphere.center.y << " " << player_sphere.center.z << " " << endl;

    // look at gravity
	if(velocity.y >= gravity.y)
		velocity += gravity;

    // update the camera location, and change it
	vector3d newpos(cam.getLocation());
	// update the newpos
	newpos+=velocity;
	vector3d oldpos(newpos);

	// go through all collision planes
	for(int i=0;i<PlaneList.size();i++)
	{
		collision::sphereplane(newpos,
                                PlaneList[i].normal,
                                PlaneList[i].p[0],
                                PlaneList[i].p[1],
                                PlaneList[i].p[2],
                                PlaneList[i].p[3],
                                player_sphere.radius);
	}
	//std::cout << "\n" << newpos;

    // check the player's on the ground
//	if(newpos.y>tmp.y)

 //   cout << "cam.getLocation().y " << cam.getLocation().y << endl;
 //   cout << "newpos.y " << newpos.y << endl;
    if(oldpos.y < newpos.y)
 //   if(cam.getLocation().y < newpos.y)
		isground = true;
    else
        isground = false;

    // set the position
	setPosition(newpos);

    if(isWeapon)
        weapons[cur_weapon]->update();


//	weapons[curweapon]->update();

	if(issprint)
		energy-=0.05;
	else if(energy<50)
        energy+=0.05;
    if(issprint && energy <= 0)
        stopSprint();
//		setSprint(false);
//	std::cout << curweapon << std::endl;

    if(cam.isMoved())
    {
        if(issprint)
        {
            if(head_Move_Up)
            {
                headPosition+=15;
                if(headPosition >= 50)
                    head_Move_Up = false;
            }
            else
            {
                headPosition-=15;
                if(headPosition <= -50)
                    head_Move_Up = true;
            }
            cam.lookAt(cam.getPitch() + (float) headPosition/300, cam.getYaw()+ (float) headPosition/300);
        }
        else
        {
            if(head_Move_Up)
            {
                headPosition+=10;
                if(headPosition >= 50)
                    head_Move_Up = false;
            }
            else
            {
                headPosition-=10;
                if(headPosition <= -50)
                    head_Move_Up = true;
            }
            cam.lookAt(cam.getPitch() + (float) headPosition/1000, cam.getYaw());

        }
    }
    }
}


vector3d player::getPosition()
{
    return player_sphere.center;

}

void player::show()
{
    // if the player current has a weapon
glPushMatrix();
  //  glEnable(GL_LIGHTING);

    //glEnable(GL_LIGHT2);
    glEnable(GL_COLOR_MATERIAL);
    float pos2[] = {0.0f, -0.0f, 0.0f, 1.0};
    glLightfv(GL_LIGHT2, GL_POSITION, pos2);
    float dif[]={1.0,1.0,1.0,1.0};
  //  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, dif);

    float amb[] = {0.2,0.2,0.2,1.0};
    glLightfv(GL_LIGHT2, GL_AMBIENT, amb);

    if(isWeapon)
        weapons[cur_weapon]->show();

   //     glDisable(GL_LIGHT2);
 //   glDisable(GL_LIGHTING);
    glPopMatrix();
}





void player::addWeapon(weapon* w)
{
	weapons.push_back(w);
}


weapon* player::getCurWeapon()
{
	return weapons[cur_weapon];
}

void player::changeWeapon(unsigned int num)
{
	if(num<weapons.size() && num>=0)
		cur_weapon=num;
}

// change weapon through mouse
void player::changeWeaponUp()
{
	cur_weapon++;
	if(cur_weapon >= weapons.size())
		cur_weapon=0;
	cout << cur_weapon << std::endl;
}

void player::changeWeaponDown()
{
	cur_weapon--;
	if(cur_weapon < 0)
		cur_weapon=weapons.size()-1;
	cout << cur_weapon << std::endl;
}





/*

camera* player::getCamera()
{
	return &cam;
}
*/
void player::decreaseHealth(int num)
{
	health-=num;
}

int player::getHealth()
{
	return health;
}


void player::setHealth(int h)
{
	health=h;
}


void player::addHealth(int h)
{
	health+=h;
}

/*
void player::setSprint(bool b)
{
	if(energy>3)
	{
		if(b && !weapons[curweapon]->isaimed())
		{
			issprint=true;
			cam.setSpeed(sprintspeed,lookspeed);
		}else{
			issprint=b;
			cam.setSpeed(normalspeed,lookspeed);
		}
	}else{
		issprint=false;
		cam.setSpeed(normalspeed,lookspeed);
	}
}
*/

bool player::isSprinting()
{
	return issprint;
}

void player::addPoints(int num)
{
	points+=num;
}

int player::getPoints()
{
	return points;
}


void player::setNetID(int i)
{
    isready = true;
    Net_ID = i;
}
int player::getNetID()
{
    return Net_ID;
}

bool player::isReady()
{
    return isready;
}

int player::getCurframe()
{
    return 0;
}


vector3d player::getRotation()
{
    return vector3d(0,cam.getYaw(),0);

}

int player::getWeaponIndex()
{
    return 0;
}


/*
int player::getNumWeapons()
{
	return weapons.size();
}

void player::holdWeapon(bool b)
{
	isWeapon=b;
}
*/
