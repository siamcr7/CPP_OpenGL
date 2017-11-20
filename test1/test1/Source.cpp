#include <windows.h>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Shlwapi.h>					/// for getting the working directory
#pragma comment(lib, "shlwapi.lib")		/// for getting the working directory
#include <stdarg.h>                       // Header File For Variable Argument Routines   ( ADD )
#include <gl/gl.h>                        // Header File For The OpenGL32 Library
#include <gl/glu.h>                       // Header File For The GLu32 Library
#include <GL/glut.h>
#include "imageloader.h"
#include <sstream>

/**
Features:
1. Car runs along Z axis. further is minus, closer is plus. carZ was reset when the car reached the end of the path.
2. Car can move left or right.
3. Car can speed up and slow down. Also a hardbrake option is there.
4. Randomly generated cubes on the roads. 
5. Mathematical caculation of total distance travelled from kmph and seconds the car ran. 
6. Mathematical Collision Detection of two cubes.
7. Multiple screen features such as : gameOver screen, ranking Screen, Exit Conformation screeen.
8. Detailed instruction in every screen.
9. Use of mouse in some screens.
10. Check of status in the main gameplay screen. This updates dynamically.
11. Texture mapping used : grass, raod, sky.
12. Dynamically loads the textures. No need to specify the path of Img. just load and RUN!
13. Basic camera rotation. left,right and 360 degree.
14. Basic lighting features when rotating.
15. Ranking system using database. Database was made using file system.
16. Database query and update handling.
17. Showing ranking System in a separete screen.
18. Difficulty can be changed with the number of cubes on road. Change cntCube.
19. Replayable option.
**/


/// Ranking File is the database
#define READ() 	freopen("Ranking.txt","r",stdin)
#define WRITE() freopen("Ranking.txt","w",stdout)
#define MAX		101

#define SSTR( x ) static_cast< std::ostringstream & >( \
	( std::ostringstream() << std::dec << x ) ).str()

using namespace std;

void initEverything(); 
float _cameraAngle = 0.0;
float carZ = -100.0; /// car Y axis
float viewCam = 0; /// front/back 

float carX =0.0; /// car X axis
float moveCam =0.0; /// camera movement with car movement along X axis

float objectStartPosition = -200; /// how close

/// update loop and overall speed
float speed1 = 100;
float speed2 = 100;

float totatDistTravel = 0.0;
clock_t t1,t2;
void calDist() /// how much km travelled
{
	t2=clock();
    float diff ((float)t2-(float)t1);
    float seconds = diff / CLOCKS_PER_SEC;
	totatDistTravel += (((100-speed1)/3600.0)*seconds);
}

/// image load var
GLuint _textureId; /// grass
GLuint _textureId2; /// road
GLuint _textureId3; /// sky

//lighting starts
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 100.0 };
GLfloat light_position[] = { .1, .1, 1, 0 };
GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 0.2 }; 
void lightInit(GLfloat white_light[])
{
	// glClearColor (0.4,0.9,0.9 ,0.0);
	glShadeModel (GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}
void light_red()
{
	GLfloat white_light[] = { 1, 0, 0, 0 };
	//GLfloat mat_ambiant[]= {0.2f, 0.2f, 0.2f, 1.0f};
	lightInit(white_light);
}
void light_yellow()
{
	GLfloat white_light[] = { 1, 1, 0, 0 };

	lightInit(white_light);
}
void light_black()
{
	GLfloat white_light[] = { 0, 0, 0, 0 };

	lightInit(white_light);
}
void light_white()
{
	GLfloat white_light[] = { 1, 1, 1, 0 };

	lightInit(white_light);
}
void light_blue()
{
	GLfloat white_light[] = { 0, 0, 1, 0 };

	lightInit(white_light);
}
void light_wood_tree()
{
	GLfloat white_light[] = { 0.9,0.3,0 };

	lightInit(white_light);
}
void light_green_tree()
{
	GLfloat white_light[] = { 0,1,0.3,1 };

	lightInit(white_light);
}
void lightOff()
{
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
}
//lighting ends

/// drawing starts
float cubeSz[10]; /// size of ith cube
float cubeX[10];  /// x axis of ith cube
float cubeZ[10]; /// z axis of ith cube 
int cntCube = 3; /// count number of cubes
void genRandomCube()
{
	for(int i=0;i<cntCube;i++)
	{
		cubeSz[i] = rand()%2; cubeSz[i]++;

		cubeX[i] = rand()%4;
		int Minus = rand()%2;
		if(Minus)cubeX[i] *= -1;

		cubeZ[i] = rand()%70; cubeZ[i]+=5;
	}
}

void drawCube(double x,double y,double z,double sz)
{
	glPushMatrix();
		glColor3f(0.9,0.3,0);
		glTranslatef(0+x,0+y,0+z);
		glutSolidCube(sz);
	glPopMatrix();
}

void drawRandomCube()
{
	for(int i=0;i<cntCube;i++)drawCube(cubeX[i],-1,cubeZ[i],cubeSz[i]);
}

void drawGrass()
{
	glPushMatrix();
		glTranslatef(0.0, 0.0, 5.0f);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1.0,1.0,1.0); /// this is important! naile shob color er upor pore! 
		double ym = -2;

			/// left side
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);
				glVertex3f(-50, ym, -300.0);

				glTexCoord2f(0.0, 5.0);
				glVertex3f(-5.0, ym, -300.0);

				glTexCoord2f(5.0, 5.0);
				glVertex3f(-5.0, ym,100.0);

				glTexCoord2f(5.0, 0.0);
				glVertex3f(-50, ym, 100.0);
			glEnd();

			/// right side
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);
				glVertex3f(5, ym, -300.0);

				glTexCoord2f(0.0, 5.0);
				glVertex3f(50.0, ym, -300.0);

				glTexCoord2f(5.0, 5.0);
				glVertex3f(50.0, ym,100.0);

				glTexCoord2f(5.0, 0.0);
				glVertex3f(5, ym, 100.0);
			glEnd();

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawMainRoad()
{
	glPushMatrix();

		glTranslatef(0.0, 0.0, 5.0f);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId2);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1.0,1.0,1.0);
		int ym = -2;
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);
				glVertex3f(-5.0, ym, -300.0);

				glTexCoord2f(1.0, 0.0);
				glVertex3f(5.0, ym, -300.0);

				glTexCoord2f(1.0, 1.0);
				glVertex3f(5.0, ym, 100.0);

				glTexCoord2f(0.0, 1.0);
				glVertex3f(-5.0, ym, 100.0);
			glEnd();

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawSky()
{
	glPushMatrix();

		glTranslatef(0.0, 0.0, -carZ-150);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId3);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glColor3f(1.0,1.0,1.0);
		double y = 90.0;
		double x = 180.0;
		glRotatef(180, 0, 0, 1);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);
				glVertex2f(-x, y);

				glTexCoord2f(1.0, 0.0);
				glVertex2f(x, y);

				glTexCoord2f(1.0, 1.0);
				glVertex2f(x, -y);

				glTexCoord2f(0.0, 1.0);
				glVertex2f(-x, -y);
			glEnd();

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void round_tree(double x,double y,double z)
{
	// nicher part suru
	glPushMatrix();
		light_wood_tree();
		//glColor3f(0.9,0.3,0);
		glTranslatef(0+x,0+y,0+z);
		glRotatef(90,1,0,0);
		glScalef(3,.75,20);
		glutSolidTorus(.03,.03,90,100);
	glPopMatrix();
	// nicher part shesh

	//uporer part starts
	glPushMatrix();
		light_green_tree();
		//glColor3f(0,1,0.3);
		glTranslatef(0+x,.6+y,0+z);
		glScalef(6,6,6);
		glutSolidSphere(.1,30,30);
	glPopMatrix();
	//uporer part shesh

	lightOff();
}

void cone_tree(double x, double y, double z)
{
	///tree er botton///
	glPushMatrix();
	glPushMatrix();
		///init();
		//glColor3f(0.9,0.3,0);
		light_wood_tree();
		glTranslatef(0+x,0+y,0+z);
		glRotatef(90,1,0,0);
		glScalef(2,.75,15);
		glutSolidTorus(.03,.03,90,100);
	glPopMatrix();

	///tree er top///
	glPushMatrix();
		///init();
		//glColor3f(0,1,0.3);
		light_green_tree();
		glTranslatef(0+x,.3+y,0+z);
		glRotatef(90,-1,0,0);
		glutSolidCone(.6,1.5,30,30);
	glPopMatrix();
	lightOff();
	glPopMatrix();
}

void drawRoad()
{
	drawGrass();

	//gach-pala boshano starts 
	glPushMatrix();
		cone_tree(10,-1,-10);
		round_tree(-10,-1,40); /// aro gach pala boshao
		cone_tree(-10,-1,80);
		cone_tree(-10,-1,-40);

		round_tree(10,-1,90);
		cone_tree(10,-1,60);
		round_tree(10,-1,-60);
	glPopMatrix();
	//gach-pala boshano ends

	
	drawMainRoad();
	 

	//road er moddhe shada shada divider boshano start
	glPushMatrix();
		glTranslatef(0,-1.5,0);
		glColor3f(1,1,1);
		//	light_white();
		glScalef(.70,.2,7);

		glTranslatef(0.0,0.0,-20);
		for(int i = 0; i < 20 ; i++)
		{
			glutSolidCube(.5);
			glTranslatef(0.0,0.0,2);
		}
	glPopMatrix();

	//road er moddhe shada shada divider boshano shesh
}

void car_3D_red()
{
	glPushMatrix();
		//glRotatef(45,0,1,0);
		//glTranslatef(0,-.2,0);
		glTranslatef(0,-1.3,0);
		glScalef(.8,.8,.5);

		///main body//
		glPushMatrix();
			glTranslatef(0,0,-.4);
			//glColor3f(1,0,0);
			light_red();
			glScalef(1,.4,2);
			glutSolidCube(1);
		glPopMatrix();

		///car er roof//
		glPushMatrix();
			glTranslatef(0,.3,-.4);
			//glColor3f(0,0,1);
			light_blue();
			glScalef(2,.4,2);
			glutSolidCube(.5);
		glPopMatrix();

		///windshild//
		glPushMatrix();
			glTranslatef(0,.3,.13);
			//glRotatef(25,-1,0,0);
			//glColor3f(.9,.9,.9);
			light_white();
			glScalef(2,.4,.1);
			glutSolidCube(.5);
		glPopMatrix();

		///car er light
		glPushMatrix();
			glTranslatef(-0.2,0,0.6);
			//glColor3f(1,1,0);
			light_yellow();
			glRotatef(90,0,1,0);
			//glScalef(1,.5,1);
			glutSolidSphere(.1,20,20);
		glPopMatrix();

		///car er light right
		glPushMatrix();
			glTranslatef(0.2,0,0.6);
			//glColor3f(1,1,0);
			light_yellow();
			glRotatef(90,0,1,0);
			//glScalef(1,.5,1);
			glutSolidSphere(.1,20,20);
		glPopMatrix();
		///car er wheel front//

		glPushMatrix();
			//glColor3f(0.01,0.01,0.01);
			light_black();
			glTranslatef(-0.5,-.2,.1);
			glRotatef(90,0,1,0);
			glutSolidTorus(.07,.15,20,30);
		glPopMatrix();

		///car wheel front//
		glPushMatrix();
			//glColor3f(0.01,0.01,0.01);
			light_black();
			glTranslatef(0.5,-.2,.1);
			glRotatef(90,0,1,0);
			glutSolidTorus(.07,.15,20,30);
		glPopMatrix();

		///car wheel back///
		glPushMatrix();
			//glColor3f(0.01,0.01,0.01);
			light_black();
			glTranslatef(-0.5,-.2,-.85);
			glRotatef(90,0,1,0);
			glutSolidTorus(.07,.15,20,30);
		glPopMatrix();

		///car wheel back///
		glPushMatrix();
			//glColor3f(0.01,0.01,0.01);
			light_black();
			glTranslatef(0.5,-.2,-.85);
			glRotatef(90,0,1,0);
			glutSolidTorus(.07,.15,20,30);
		glPopMatrix();
	glPopMatrix();


	lightOff();

	//glDisable(GL_DEPTH_TEST);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_DEPTH_TEST);

}

void carRun()
{
	glPushMatrix();
		glTranslatef(0,0,objectStartPosition);
		glColor3f(1,0,0);
		car_3D_red();                              //CAR1
		glTranslatef(0.0,0.0,70);
	glPopMatrix();   
}

void drawCar()
{
	glPushMatrix();
		glTranslatef(0,-.63,0);
		glTranslatef(0,0,1);
		glRotatef(180, 0.0, 1.0, 0.0);
		glScalef(.5,.5,.5);
		glTranslatef(carX,0.0,0.0);
		car_3D_red();
	glPopMatrix();
	carRun();
}
/// drawing ends

/// rotation
bool rotate360 = false;
bool leftRotate = false;
bool rightRotate = false;
bool pause = false;
bool lookFront = true;
float angleCAM = 0.0;

/// text draw starts
void drawtext(const char *text,int length,int x,int y)
{
	glMatrixMode(GL_PROJECTION);
	double* matrix=new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX,matrix);
	glLoadIdentity();
	glOrtho(0,800,0,600,-5,5);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x,y);
	for(int i=0; i<length; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15,(int)text[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}

string statusArr[10];
void makeStatusArr()
{
	int i = 0;
	if(pause)statusArr[i++] = "Stopped";
	else statusArr[i++] = "Moving";

	if(lookFront)statusArr[i++] = "off" , statusArr[i++] = "on";
	else statusArr[i++] = "on" , statusArr[i++] = "off";

	if(rotate360)statusArr[i++] = "on";
	else statusArr[i++] = "off";

	if(leftRotate)statusArr[i++] = "on";
	else statusArr[i++] = "off";

	if(rightRotate)statusArr[i++] = "on";
	else statusArr[i++] = "off";
}

void DrawAllTheText()
{
	glPushMatrix();
		glColor3f(1,0,0);
		string text;
		makeStatusArr();

		int x = 600;
		int y = 600;
		int i = 0;
		

		text="Car Status   : " + statusArr[i++];
		drawtext(text.data(),text.size(),x,y-=20);

		text="Hard Brake   : h";
		drawtext(text.data(),text.size(),x,y-=20);

		text="Speed Up     : w";
		drawtext(text.data(),text.size(),x,y-=20);

		text="Speed Down   : s";
		drawtext(text.data(),text.size(),x,y-=20);

		text="Move Left    : a";
		drawtext(text.data(),text.size(),x,y-=20);

		text="Move Right   : d";
		drawtext(text.data(),text.size(),x,y-=20);

		text="Current Speed: "+SSTR( 100-speed1 )+"Kmph";
		drawtext(text.data(),text.size(),x,y-=20);

		calDist();
		text="KM Travelled : " +SSTR( totatDistTravel )+"Km";
		drawtext(text.data(),text.size(),x,y-=20);

		text="Look Behind  : b | status: " + statusArr[i++];
		drawtext(text.data(),text.size(),x,y-=20);

		text="Look Front   : b | status: " + statusArr[i++];
		drawtext(text.data(),text.size(),x,y-=20);

		text="Rotate 360   : o | status: " + statusArr[i++];
		drawtext(text.data(),text.size(),x,y-=20);

		text="Rotate left  : l | status: " + statusArr[i++];
		drawtext(text.data(),text.size(),x,y-=20);

		text="Rotate right : r | status: " + statusArr[i++];
		drawtext(text.data(),text.size(),x,y-=20);

		text="Reset Rotate : z ";
		drawtext(text.data(),text.size(),x,y-=20);

		text="Quit : q ";
		drawtext(text.data(),text.size(),x,y-=20);

		glTranslatef(0.0, 2.5, 0.0);

	glPopMatrix();
}

void exitScreen()
{
	float x = 2;
	float y = 2;
	glClearColor (1,1,1 ,0.0);

	glColor3f(0,0,0);
	x = 350;
	y = 400;

	string text;
	text="Do You Want to EXIT?";
	drawtext(text.data(),text.size(),x,y);

	y -= 50;

	text="YES!";
	drawtext(text.data(),text.size(),x+20,y);

	text="NO!";
	drawtext(text.data(),text.size(),x+80,y);
}

bool rankingTime = false;
int iCnt = 0;

string curName = "";
bool replaying = false;
struct info
{
	float pts;
	string nm;
	bool curPlay;
	info(){}
	info(float p,string s,bool cp)
	{
		pts = p;
		nm = s;
		curPlay = cp;
	}
};
info pArr[MAX];

bool cmp(info x1,info x2)
{
	return x1.pts > x2.pts;
}

void getRanking()
{
	if(!replaying)
	{
		iCnt = 0;
		string s; float pts;
		int cp;
		READ();
		while(cin >> s >> pts >> cp)
		{
			pArr[iCnt++] = info(pts,s,cp);
		}

		for(int i=0;i<iCnt;i++)pArr[i].curPlay = 0;

		pArr[iCnt++] = info(totatDistTravel,curName,1);
		replaying = true;
	}
	else
	{
		for(int i=0;i<iCnt;i++)
		{
			if(pArr[i].curPlay == true)
			{
				pArr[i].pts = totatDistTravel;
				break;
			}
		}
	}
	sort(pArr,pArr+iCnt,cmp);
	if(iCnt > 0)
	{
		WRITE();
		for(int i=0;i<iCnt;i++)cout << pArr[i].nm << " " << pArr[i].pts << " " << pArr[i].curPlay << endl;
	}
}

void showRanking()
{
	float x = 2;
	float y = 2;
	glClearColor (1,1,1 ,0.0);

	glColor3f(0,0,0);
	x = 200;
	y = 550;

	string text;

	text="Rank";
	drawtext(text.data(),text.size(),x,y);

	text="Total Distant Travelled";
	drawtext(text.data(),text.size(),x+50,y);

	text="Name";
	drawtext(text.data(),text.size(),x+250,y);

	for(int i=0;i<iCnt;i++)
	{
		y -= 20;

		text=SSTR(i+1);
		drawtext(text.data(),text.size(),x,y);

		text=SSTR(pArr[i].pts);
		drawtext(text.data(),text.size(),x+50,y);

		text=pArr[i].nm;
		drawtext(text.data(),text.size(),x+250,y);
	}

	x = 550;
	y = 550;
	text="Press U to Quit!";
	drawtext(text.data(),text.size(),x,y);

	y -= 20;
	text="Press K to play again!";
	drawtext(text.data(),text.size(),x,y);
}

void gameOver()
{
	float x = 2;
	float y = 2;
	glClearColor (1,1,1 ,0.0);

	glColor3f(0,0,0);
	x = 350;
	y = 400;

	string text;
	text="Do You Want to Try Again?";
	drawtext(text.data(),text.size(),x,y);

	y -= 50;

	text="YES!";
	drawtext(text.data(),text.size(),x+20,y);

	text="NO!";
	drawtext(text.data(),text.size(),x+80,y);

	y -= 50;
	text="Press ''m'' to See the rankings!";
	drawtext(text.data(),text.size(),x,y);
}

/// text draw ends
bool exitScreenPressed = false; /// check to see if exit button is pressed
bool collide = false; /// checking collision

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(rankingTime)
	{
		glPushMatrix();
		glTranslatef(0,0,-carZ); // cancel out all the movement
			showRanking();
		glPopMatrix(); 
	}
	else if(collide)
	{
		glPushMatrix();
		glTranslatef(0,0,-carZ); // cancel out all the movement
			gameOver();
		glPopMatrix(); 
	}
	else if(exitScreenPressed)
	{
		glPushMatrix();
		glTranslatef(0,0,-carZ); // cancel out all the movement
			exitScreen();
		glPopMatrix();   
	}
	else
	{
		glClearColor (0.4,0.9,0.9 ,0.0); /// SKY yooo
		drawSky();
		
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
		glLoadIdentity(); //Reset the drawing perspective
		

		glRotatef(-_cameraAngle, 0.0, 1.0, 0.0); //Rotate the camera
		glTranslatef(0.0, 0.0, -7.0); //Move forward 5 units

		DrawAllTheText();

		glTranslatef(0.0, -0.5, 0.0);
		glTranslatef(moveCam,0.0,0.0);
		glRotatef(-3, 1.0, 0.0, 0.0);
		glRotatef(viewCam, 0.0, 1.0, 0.0);
		glRotatef(angleCAM, 0.0, 1.0, 0.0);

		drawCar();
	
		glTranslatef(0.0, 0.0, carZ); // start
		drawRoad();

		drawRandomCube();
	}

	glFlush();
	glutSwapBuffers();
}

bool detectCollision()
{
	for(int i=0;i<cntCube;i++)
	{
		float cubeX1 = -2*cubeX[i]+((2.5*cubeSz[i])/2.0);
		float cubeX2 = -2*cubeX[i]-((2.5*cubeSz[i])/2.0);

		float cubeZ1 = cubeZ[i] - .5;
		float cubeZ2 = cubeZ1 - cubeSz[i];
		cubeZ1 *= -1;
		cubeZ2 *= -1;

		if(carZ <= cubeZ2 && carZ >= cubeZ1)
		{
			if(carX <= cubeX1 && carX >= cubeX2)return true;
		}
		//cout << "YALA: " << cubeX1 << " " << cubeX2 << endl;
	}
	return false;
}

/// texture load suru
string curDirectory;
GLuint loadTexture(Image* image) 
{
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}

char ss[1000];
string sArrForImgLoc[5];
void remakeSS(int ii)
{
	for(int i=0;i<1000;i++)ss[i] = '\0';

	int iCntSS = 0;
	for(int i=0;i<curDirectory.size();i++)
	{
		ss[iCntSS++] = curDirectory[i];
		int x = curDirectory[i];
		if(x == 92) // 92 is '\'
		{
			char c = x;
			ss[iCntSS++] = c;
		}
	}
	for(int i=0;i<sArrForImgLoc[ii].size();i++)
	{
		ss[iCntSS++] = sArrForImgLoc[ii][i];
		int x = sArrForImgLoc[ii][i];
		if(x == 92) // 92 is '\'
		{
			char c = x;
			ss[iCntSS++] = c;
		}
	}
	ss[iCntSS++] = '\0';
}

void initializeTexture() {

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45.0, 1.00, 1.0, 200.0);

	sArrForImgLoc[0] = "images\\grass2.bmp";
	sArrForImgLoc[1] = "images\\road3.bmp";
	sArrForImgLoc[2] = "images\\cloud.bmp";

	//Image* image = loadBMP("C:\\Users\\Jamil\\Desktop\\Final\\CG\\Project Work\\test1\\test1\\images\\grass2.bmp");
	//Image* image2 = loadBMP("C:\\Users\\Jamil\\Desktop\\Final\\CG\\Project Work\\test1\\test1\\images\\road3.bmp");
	//Image* image3 = loadBMP("C:\\Users\\Jamil\\Desktop\\Final\\CG\\Project Work\\test1\\test1\\images\\cloud.bmp");

	remakeSS(0);
	Image* image = loadBMP(ss);

	remakeSS(1);
	Image* image2 = loadBMP(ss);

	remakeSS(2);
	Image* image3 = loadBMP(ss);

	_textureId = loadTexture(image);
	_textureId2 = loadTexture(image2);
	_textureId3 = loadTexture(image3);

	delete image;
	delete image2;
	delete image3;
}
/// texture load shesh

void update(int value)
{
	//cout << carX << " " << carZ << endl;
	if(detectCollision())
	{
		//cout << "Collide" << endl;
		speed1 = 100;
		collide = true;
		pause = true;
	}
	if(!pause && !exitScreenPressed) /// jodi pause na  thake then eita cholte thakbe
	{
		//if(SCORE == 20 && speed1 > 25) speed1 = 25;
		carZ += .5;
		if(carZ > 3) /// path end hoile notun cube generate
		{
			genRandomCube();
			carZ = -100;
		}
	}

	if(rotate360)
	{
		if(leftRotate)angleCAM +=.5;
		else if(rightRotate)angleCAM -=.5;
	}
	else if(leftRotate)angleCAM +=.5 , leftRotate = false;
	else if(rightRotate)angleCAM -=.5 , rightRotate = false;

	if(angleCAM > 360)angleCAM -= 360;
	if(angleCAM < 0)angleCAM += 360;

	glutPostRedisplay();
	glutTimerFunc(speed1, update, 0);
}

void updateBeforeExit()
{
	if(iCnt > 0)
	{
		WRITE();
		for(int i=0;i<iCnt;i++)cout << pArr[i].nm << " " << pArr[i].pts << " " << 0 << endl;
	}
	
}

void keyboardHandle(unsigned char key, int x, int y)
{
	if(key == 'u' || key == 'U')
	{
		updateBeforeExit();
		exit(0);
	}
	if(key == 'k' || key == 'K')initEverything();

	if( key == 'q' || key == 'Q' )
	{
		if(!exitScreenPressed)exitScreenPressed = true;
		else exitScreenPressed = false;
	}

	if( key == 'h' || key == 'H') /// pause or play
	{
		speed1 = 100;
		pause = true;
	}

	if( (key == 'w' || key == 'W') && speed1 > 5)speed1 -= 2;

	if( (key == 's' || key == 'S') && speed1 < 100)speed1 += 2;

	if(100.0-speed1 == 0.0)
	{
		pause = true;
	}
	else pause = false;

	if( ( key == 'a' || key == 'A' ) /*&&!pause*/ ) /// baam e
	{
		if(carX < 7.5)
		{
			carX +=.25;
			moveCam +=.12;
			angleCAM +=.2;
		}
	}

	if(( key == 'd' || key == 'D' ) /*&&!pause*/ ) /// daan e
	{
		if(carX > -7.5)
		{
			carX -=.25;
			moveCam -=.12;
			angleCAM -=.2;
		}
	}

	if( key == 'b' || key == 'B') /// look back or look front
	{
		if(viewCam != 180)viewCam = 180 , lookFront = false;
		else viewCam = 0 , lookFront = true;
	}

	if( key == 'z' || key == 'Z' ) /// reset rotate
	{
		viewCam = 0;
		angleCAM = 0;
		rotate360 = false;
		leftRotate = false;
		rightRotate = false;
	}

	if( key == 'o' || key == 'O' ) /// rotate 360
	{
		if(rotate360)rotate360 = false;
		else rotate360 = true;
	}

	if(key == 'l' || key == 'L')
	{
		if(!leftRotate)leftRotate = true;
		else leftRotate = false;
	}

	if(key == 'r' || key == 'R')
	{
		if(!rightRotate)rightRotate = true;
		else rightRotate = false;
	}

	if( (key == 'm' || key == 'M') && rankingTime == false)
	{
		getRanking();
		rankingTime = true;
	}

	makeStatusArr();
	glutPostRedisplay();
}

void mouseHandle(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			//printf("clicked at (%d, %d)\n", x, y);
			if(collide) /// game over screen
			{
				if(y >= 225 && y <= 237)
				{
					if(x >= 556 && x <= 598) /// yes
					{
						initEverything();
					}
					else if(x >= 658 && x <= 680) /// no
					{
						initEverything();
						exitScreenPressed = true;
					}
				}
			}
			else if(exitScreenPressed)
			{
				if(y >= 225 && y <= 237)
				{
					if(x >= 556 && x <= 598) /// yes
					{
						updateBeforeExit();
						exit(0);
					}
					else if(x >= 658 && x <= 680) /// no
					{
						exitScreenPressed = false;
					}
				}
			}
			
			glutPostRedisplay();
		}
	}
}

//Called when the window is resized
void handleResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void initEverything()
{
	pause = true;

	t1=clock(); /// time starts
	genRandomCube();

	exitScreenPressed = false;
	collide = false;

	rotate360 = false;
	leftRotate = false;
	rightRotate = false;
	lookFront = true;
	angleCAM = 0.0;

	_cameraAngle = 0.0;
	carZ = -100.0;
	viewCam = 0;
	
	carX =0.0, moveCam =0.0;
	
	objectStartPosition = -200;
	
	speed1 = 100;
	speed2 = 100;

	totatDistTravel = 0.0;

	rankingTime = false;
	//iCnt = 0;

	//replaying = false;
}

std::string GetWorkingDir() {
	char path[MAX_PATH] = "";
	GetCurrentDirectoryA(MAX_PATH, path);
	PathAddBackslashA(path);
	return (string)path;
}

int main(int argc, char** argv)
{
	curDirectory = GetWorkingDir();
	//cout << curDirectory << endl;

	cout <<"Enter Name: " ;
	cin >> curName;
	
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1224, 568);
	srand(time(NULL));
	//Create the window
	glutCreateWindow("CAR Game Beta");
	//Set handler functions
	glEnable(GL_DEPTH_TEST);
	initializeTexture();

	initEverything();

	glutDisplayFunc(draw);
	glutReshapeFunc(handleResize);

	glutKeyboardFunc(keyboardHandle);
	glutMouseFunc(mouseHandle);

	glutTimerFunc(speed2, update, 0); //Add a timer

	glutMainLoop();


	return 0;
}