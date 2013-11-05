// The sample robotarm model.  You should build a file
// very similar to this for when you make your model.
#pragma warning (disable : 4305)
#pragma warning (disable : 4244)
#pragma warning(disable : 4786)
#pragma warning (disable : 4312)


#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "particleSystem.h"



#include "mat.h"
#include <FL/gl.h>
#include <cstdlib>

using namespace std;

#define M_DEFAULT 2.0f
#define M_OFFSET 3.0f
#define P_OFFSET 0.3f
#define MAX_VEL 200
#define MIN_STEP 0.1

// This is a list of the controls for the RobotArm
// We'll use these constants to access the values 
// of the controls from the user interface.
enum XWingControls
{ 
    WING_ANGLE=0, BODY_HEIGHT, BODY_ROTATION, COCKPIT_ANGLE, LANDING_GEAR_ANGLE, LANDING_GEAR_LENGTH, 
    LOWER_LANDING_GEAR_ANGLE, GUN_LENGTH, R2_ROTATION, PARTICLE_COUNT, NUMCONTROLS
};

void ground(float h);
void body(float h, float r);
void left_lower_wing(float h);
void left_upper_wing(float h);
void right_lower_wing(float h);
void right_upper_wing(float h);
void engine();
void gun(float h);
void r2(float h);
void cockpit(float h);
void landingGear(float l, float rot);
void landingSled(float h);
void y_box(float h);
Mat4f glGetMatrix(GLenum pname);
Vec3f getWorldPoint(Mat4f matCamXforms);

// To make a RobotArm, we inherit off of ModelerView
class XWing : public ModelerView 
{
public:
    XWing(int x, int y, int w, int h, char *label) 
        : ModelerView(x,y,w,h,label) {}
    virtual void draw();
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createXWing(int x, int y, int w, int h, char *label)
{ 
    return new XWing(x,y,w,h,label); 
}

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))

// Utility function.  Use glGetMatrix(GL_MODELVIEW_MATRIX) to retrieve
//  the current ModelView matrix.
Mat4f glGetMatrix(GLenum pname)
{
    GLfloat m[16];
    glGetFloatv(pname, m);
    Mat4f matCam(m[0],  m[1],  m[2],  m[3],
                 m[4],  m[5],  m[6],  m[7],
                 m[8],  m[9],  m[10], m[11],
                 m[12], m[13], m[14], m[15]);

    // because the matrix GL returns is column major...
    return matCam.transpose();
}

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out RobotArm
void XWing::draw()
{
	/* pick up the slider values */

  float body_height = VAL(BODY_HEIGHT);
  float body_rot = VAL(BODY_ROTATION);
  float wing_angle = VAL(WING_ANGLE);
  float cock_angle = VAL(COCKPIT_ANGLE);
  float landing_angle = VAL(LANDING_GEAR_ANGLE);
  float landing_length = VAL(LANDING_GEAR_LENGTH);
  float sled_angle = VAL(LOWER_LANDING_GEAR_ANGLE);
  float gun_length = VAL(GUN_LENGTH);
  float r2_rot = VAL(R2_ROTATION);
	float pc = VAL( PARTICLE_COUNT );

  // This call takes care of a lot of the nasty projection 
  // matrix stuff
  ModelerView::draw();

  // Save the camera transform that was applied by
  // ModelerView::draw() above.
  // While we're at it, save an inverted copy of this matrix.  We'll
  // need it later.
  Mat4f matCam = glGetMatrix( GL_MODELVIEW_MATRIX );
  //Mat4f matCamInverse = matCam.inverse();

	static GLfloat lmodel_ambient[] = {0.4,0.4,0.4,1.0};

	// define the model

	ground(-0.2);
	body(body_height, body_rot);

/*
    glTranslatef( 0.0, 0.8, 0.0 );			// move to the top of the base
    glRotatef( theta, 0.0, 1.0, 0.0 );		// turn the whole assembly around the y-axis. 
	  rotation_base(h1);						// draw the rotation base

    glTranslatef( 0.0, h1, 0.0 );			// move to the top of the base
	  glPushMatrix();
			glTranslatef( 0.5, h1, 0.6 );	
	glPopMatrix();
    glRotatef( phi, 0.0, 0.0, 1.0 );		// rotate around the z-axis for the lower arm
	glTranslatef( -0.1, 0.0, 0.4 );
	lower_arm(h2);							// draw the lower arm

    glTranslatef( 0.0, h2, 0.0 );			// move to the top of the lower arm
    glRotatef( psi, 0.0, 0.0, 1.0 );		// rotate  around z-axis for the upper arm
	upper_arm(h3);							// draw the upper arm

	glTranslatef( 0.0, h3, 0.0 );
	glRotatef( cr, 0.0, 0.0, 1.0 );
	claw(1.0);
*/



	//*** DON'T FORGET TO PUT THIS IN YOUR OWN CODE **/
	endDraw();
}

void ground(float h) 
{
	glDisable(GL_LIGHTING);
	glColor3f(0.65,0.45,0.2);
	glPushMatrix();
	glScalef(30,0,30);
	y_box(h);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

#define BACK_LENGTH 3.0
#define TOTAL_LENGTH 10.0
#define BIG_HEX_SIZE 3.0
#define SMALL_HEX_SIZE 1.5


void body(float h, float r) {
	double f_a [] = {BIG_HEX_SIZE / 2 - (SMALL_HEX_SIZE / 6), 0.0, -TOTAL_LENGTH};
  double f_b [] = {BIG_HEX_SIZE / 2 - (SMALL_HEX_SIZE / 2), SMALL_HEX_SIZE / 2, -TOTAL_LENGTH};
	double f_c [] = {BIG_HEX_SIZE / 2 - (SMALL_HEX_SIZE / 6), SMALL_HEX_SIZE, -TOTAL_LENGTH};
	double f_d [] = {BIG_HEX_SIZE / 2 + (SMALL_HEX_SIZE / 6), SMALL_HEX_SIZE, -TOTAL_LENGTH};
  double f_e [] = {BIG_HEX_SIZE / 2 + (SMALL_HEX_SIZE / 2), SMALL_HEX_SIZE / 2, -TOTAL_LENGTH};
  double f_f [] = {BIG_HEX_SIZE / 2 + (SMALL_HEX_SIZE / 6), 0.0, -TOTAL_LENGTH};
  double f_m [] = {BIG_HEX_SIZE / 2, SMALL_HEX_SIZE / 2, -TOTAL_LENGTH - 0.5};

  double b_a [] = {BIG_HEX_SIZE / 3, 0.0, 0.0};
  double b_b [] = {0, BIG_HEX_SIZE / 2, 0.0};
	double b_c [] = {BIG_HEX_SIZE / 3, BIG_HEX_SIZE, 0.0};
	double b_d [] = {2 * BIG_HEX_SIZE / 3, BIG_HEX_SIZE, 0.0};
  double b_e [] = {BIG_HEX_SIZE, BIG_HEX_SIZE / 2, 0.0};
  double b_f [] = {2 * BIG_HEX_SIZE / 3, 0.0, 0.0};
  double b_m [] = {BIG_HEX_SIZE / 2, BIG_HEX_SIZE / 2, 0.0};

  setDiffuseColor( 0.50, 0.50, 0.50 );
	setAmbientColor( 0.50, 0.50, 0.50 );
	glPushMatrix();
    glTranslate(0.0, h, 0.0);
		glPushMatrix();
      // front cone
      drawTriangle(f_f[0], f_f[1], f_f[2],
                   f_m[0], f_m[1], f_m[2],
                   f_a[0], f_a[1], f_a[2]);
      drawTriangle(f_a[0], f_a[1], f_a[2],
                   f_m[0], f_m[1], f_m[2],
                   f_b[0], f_b[1], f_b[2]);
		  drawTriangle(f_b[0], f_b[1], f_b[2],
                   f_m[0], f_m[1], f_m[2],
                   f_c[0], f_c[1], f_c[2]);
      drawTriangle(f_c[0], f_c[1], f_c[2],
                   f_m[0], f_m[1], f_m[2],
                   f_d[0], f_d[1], f_d[2]);
      drawTriangle(f_d[0], f_d[1], f_d[2],
                   f_m[0], f_m[1], f_m[2],
                   f_e[0], f_e[1], f_e[2]);
      drawTriangle(f_e[0], f_e[1], f_e[2],
                   f_m[0], f_m[1], f_m[2],
                   f_f[0], f_f[1], f_f[2]);
      // back plane
      drawTriangle(b_f[0], b_f[1], b_f[2],
                   b_m[0], b_m[1], b_m[2],
                   b_a[0], b_a[1], b_a[2]);
      drawTriangle(b_a[0], b_a[1], b_a[2],
                   b_m[0], b_m[1], b_m[2],
                   b_b[0], b_b[1], b_b[2]);
		  drawTriangle(b_b[0], b_b[1], b_b[2],
                   b_m[0], b_m[1], b_m[2],
                   b_c[0], b_c[1], b_c[2]);
      drawTriangle(b_c[0], b_c[1], b_c[2],
                   b_m[0], b_m[1], b_m[2],
                   b_d[0], b_d[1], b_d[2]);
      drawTriangle(b_d[0], b_d[1], b_d[2],
                   b_m[0], b_m[1], b_m[2],
                   b_e[0], b_e[1], b_e[2]);
      drawTriangle(b_e[0], b_e[1], b_e[2],
                   b_m[0], b_m[1], b_m[2],
                   b_f[0], b_f[1], b_f[2]);

    glPopMatrix();
	glScalef(4.0f, h, 4.0f);
	glPopMatrix();
}

void rotation_base(float h) {
	setDiffuseColor( 0.85, 0.75, 0.25 );
	setAmbientColor( 0.95, 0.75, 0.25 );
	glPushMatrix();
		glPushMatrix();
			glScalef(4.0, h, 4.0);
			y_box(1.0f); // the rotation base
		glPopMatrix();
		setDiffuseColor( 0.15, 0.15, 0.65 );
		setAmbientColor( 0.15, 0.15, 0.65 );
		glPushMatrix();
			glTranslatef(-0.5, h, -0.6);
			glScalef(2.0, h, 1.6);
			y_box(1.0f); // the console
		glPopMatrix();
		setDiffuseColor( 0.65, 0.65, 0.65 );
		setAmbientColor( 0.65, 0.65, 0.65 );
		glPushMatrix();
			glTranslatef( 0.5, h, 0.6 );
			glRotatef( -90.0, 1.0, 0.0, 0.0 );
			drawCylinder( h, 0.05, 0.05 ); // the pipe
		glPopMatrix();
	glPopMatrix();
}

void lower_arm(float h) {					// draw the lower arm
	setDiffuseColor( 0.85, 0.75, 0.25 );
	setAmbientColor( 0.95, 0.75, 0.25 );
	y_box(h);
}

void upper_arm(float h) {					// draw the upper arm
	setDiffuseColor( 0.85, 0.75, 0.25 );
	setAmbientColor( 0.95, 0.75, 0.25 );
	glPushMatrix();
	glScalef( 1.0, 1.0, 0.7 );
	y_box(h);
	glPopMatrix();
}

void claw(float h) {
	setDiffuseColor( 0.25, 0.25, 0.85 );
	setAmbientColor( 0.25, 0.25, 0.85 );

	glBegin( GL_TRIANGLES );

	glNormal3d( 0.0, 0.0, 1.0);		// +z side
	glVertex3d( 0.5, 0.0, 0.5);
	glVertex3d(-0.5, 0.0, 0.5);
	glVertex3d( 0.5,   h, 0.5);

	glNormal3d( 0.0, 0.0, -1.0);	// -z side
	glVertex3d( 0.5, 0.0, -0.5);
	glVertex3d(-0.5, 0.0, -0.5);
	glVertex3d( 0.5,   h, -0.5);

	glEnd();

	glBegin( GL_QUADS );

	glNormal3d( 1.0,  0.0,  0.0);	// +x side
	glVertex3d( 0.5, 0.0,-0.5);
	glVertex3d( 0.5, 0.0, 0.5);
	glVertex3d( 0.5,   h, 0.5);
	glVertex3d( 0.5,   h,-0.5);

	glNormal3d( 0.0,-1.0, 0.0);		// -y side
	glVertex3d( 0.5, 0.0, 0.5);
	glVertex3d( 0.5, 0.0,-0.5);
	glVertex3d(-0.5, 0.0,-0.5);
	glVertex3d(-0.5, 0.0, 0.5);

	glEnd();
}

void y_box(float h) {

	glBegin( GL_QUADS );

	glNormal3d( 1.0 ,0.0, 0.0);			// +x side
	glVertex3d( 0.25,0.0, 0.25);
	glVertex3d( 0.25,0.0,-0.25);
	glVertex3d( 0.25,  h,-0.25);
	glVertex3d( 0.25,  h, 0.25);

	glNormal3d( 0.0 ,0.0, -1.0);		// -z side
	glVertex3d( 0.25,0.0,-0.25);
	glVertex3d(-0.25,0.0,-0.25);
	glVertex3d(-0.25,  h,-0.25);
	glVertex3d( 0.25,  h,-0.25);

	glNormal3d(-1.0, 0.0, 0.0);			// -x side
	glVertex3d(-0.25,0.0,-0.25);
	glVertex3d(-0.25,0.0, 0.25);
	glVertex3d(-0.25,  h, 0.25);
	glVertex3d(-0.25,  h,-0.25);

	glNormal3d( 0.0, 0.0, 1.0);			// +z side
	glVertex3d(-0.25,0.0, 0.25);
	glVertex3d( 0.25,0.0, 0.25);
	glVertex3d( 0.25,  h, 0.25);
	glVertex3d(-0.25,  h, 0.25);

	glNormal3d( 0.0, 1.0, 0.0);			// top (+y)
	glVertex3d( 0.25,  h, 0.25);
	glVertex3d( 0.25,  h,-0.25);
	glVertex3d(-0.25,  h,-0.25);
	glVertex3d(-0.25,  h, 0.25);

	glNormal3d( 0.0,-1.0, 0.0);			// bottom (-y)
	glVertex3d( 0.25,0.0, 0.25);
	glVertex3d(-0.25,0.0, 0.25);
	glVertex3d(-0.25,0.0,-0.25);
	glVertex3d( 0.25,0.0,-0.25);

	glEnd();
}

int main()
{
    ModelerControl controls[NUMCONTROLS ];

	  controls[BASE_ROTATION] = ModelerControl("base rotation (theta)", -180.0, 180.0, 0.1, 0.0 );
    controls[LOWER_TILT] = ModelerControl("lower arm tilt (phi)", 15.0, 95.0, 0.1, 55.0 );
    controls[UPPER_TILT] = ModelerControl("upper arm tilt (psi)", 0.0, 135.0, 0.1, 30.0 );
	  controls[CLAW_ROTATION] = ModelerControl("claw rotation (cr)", -30.0, 180.0, 0.1, 0.0 );
    controls[BASE_LENGTH] = ModelerControl("base height (h1)", 0.5, 10.0, 0.1, 0.8 );
    controls[LOWER_LENGTH] = ModelerControl("lower arm length (h2)", 1, 10.0, 0.1, 3.0 );
    controls[UPPER_LENGTH] = ModelerControl("upper arm length (h3)", 1, 10.0, 0.1, 2.5 );
    controls[PARTICLE_COUNT] = ModelerControl("particle count (pc)", 0.0, 5.0, 0.1, 5.0 );
    

	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.

    ModelerApplication::Instance()->Init(&createRobotArm, controls, NUMCONTROLS);
    return ModelerApplication::Instance()->Run();
}
