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
#include "engineParticleSystem.h"


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
    WING_ANGLE=0, BODY_HEIGHT, BODY_MOVEMENT, YAW, PITCH, ROLL, COCKPIT_ANGLE, LANDING_GEAR_ANGLE, LANDING_GEAR_LENGTH, GUN_LENGTH, R2_ROTATION, PARTICLE_COUNT, NUMCONTROLS
};

void ground(float h);
void body();
void left_wing();
void right_wing();
void engine();
void gun(float h);
void gunBase();
void r2();
void r2_eye();
void r2_top();
void cockpit();
void landingGear(float l);
void landingSled();
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

#define BACK_LENGTH 3.0
#define TOTAL_LENGTH 10.0
#define BIG_HEX_SIZE 3.0
#define SMALL_HEX_SIZE 1.5

#define WING_WIDTH .2
#define WING_LENGTH 6.0
#define COCKPIT_SIZE 1.0
#define CYLINDER_R .5
#define GUN_BASE_R .3
#define GUN_BARREL_R .1
#define GEAR_W .4
#define GEAR_T .2
#define R2_SIZE .4
#define R2_TOP .3
#define EYE_WIDTH .4

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out RobotArm
void XWing::draw()
{
  int pss_index = 0;
	/* pick up the slider values */

  float body_height = VAL(BODY_HEIGHT);
  float body_mov = VAL(BODY_MOVEMENT);
  float yaw = VAL(YAW);
  float pitch = VAL(PITCH);
  float roll = VAL(ROLL);


  float wing_angle = VAL(WING_ANGLE);
  float cock_angle = VAL(COCKPIT_ANGLE);
  float landing_angle = VAL(LANDING_GEAR_ANGLE);
  float landing_length = VAL(LANDING_GEAR_LENGTH);
  float gun_length = VAL(GUN_LENGTH);
  float r2_rot = VAL(R2_ROTATION);
	float pc = VAL( PARTICLE_COUNT );
  vector<ParticleSystem*> *pss = ModelerApplication::Instance()->GetParticleSystems();
  for (vector<ParticleSystem*>::iterator iter = pss->begin(); iter != pss->end(); ++iter) {
    ParticleSystem* ps = *iter;
    ps->setPc(pc);
  }
  //TODO : set the mat right

  // This call takes care of a lot of the nasty projection 
  // matrix stuff
  ModelerView::draw();

  // Save the camera transform that was applied by
  // ModelerView::draw() above.
  // While we're at it, save an inverted copy of this matrix.  We'll
  // need it later.
  Mat4f matCam = glGetMatrix( GL_MODELVIEW_MATRIX );
  Mat4f matCamInverse = matCam.inverse();

	static GLfloat lmodel_ambient[] = {0.4,0.4,0.4,1.0};

	// define the model

	ground(-0.2);
  glPushMatrix();
    glTranslatef(0.0, body_height + BIG_HEX_SIZE / 2, 0.0); 
    glRotatef(yaw, 0.0, 1.0, 0.0);
    glRotatef(pitch, 1.0, 0.0, 0.0);
    glRotatef(roll, 0.0, 0.0, 1.0);
    glTranslatef(-BIG_HEX_SIZE / 2, -BIG_HEX_SIZE / 2, TOTAL_LENGTH / 2 - body_mov);
    body();
    // right upper wing
	  glPushMatrix();
      glTranslatef(1 * BIG_HEX_SIZE / 3.0, BIG_HEX_SIZE / 2, 0.0);
      glRotatef(wing_angle, 0, 0, -1.0);
      right_wing();
      glPushMatrix();
        glTranslatef(-BIG_HEX_SIZE / 3.0 - CYLINDER_R, WING_WIDTH + CYLINDER_R, -BACK_LENGTH - CYLINDER_R);
        engine();
        glPushMatrix();
          glTranslatef(0.0, 0.0, BACK_LENGTH + CYLINDER_R + 1.0);
          (*pss)[pss_index++]->glMat = matCamInverse * glGetMatrix(GL_MODELVIEW_MATRIX);
        glPopMatrix();
      glPopMatrix();
      glPushMatrix();
        glTranslatef(-(WING_LENGTH - GUN_BASE_R), WING_WIDTH + GUN_BASE_R, - BACK_LENGTH - gun_length - 2 * GUN_BASE_R - GUN_BARREL_R);
        gun(gun_length);
        glPushMatrix();
          glTranslatef(0.0, 0.0, GUN_BASE_R * 2.0 + GUN_BARREL_R + gun_length);
          gunBase();
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();
    // right lower wing
	  glPushMatrix();
      glTranslatef(1 * BIG_HEX_SIZE / 3.0, BIG_HEX_SIZE / 2 - WING_WIDTH, 0.0);
      glRotatef(wing_angle, 0, 0, 1.0);
      right_wing();
      glPushMatrix();
        glTranslatef(-BIG_HEX_SIZE / 3.0 - CYLINDER_R, -CYLINDER_R, -BACK_LENGTH - CYLINDER_R);
        engine();
        glPushMatrix();
          glTranslatef(0.0, 0.0, BACK_LENGTH + CYLINDER_R + 1.0);
          (*pss)[pss_index++]->glMat = matCamInverse * glGetMatrix(GL_MODELVIEW_MATRIX);
        glPopMatrix();
      glPopMatrix();
      glPushMatrix();
        glTranslatef(-(WING_LENGTH - GUN_BASE_R), -GUN_BASE_R, - BACK_LENGTH - gun_length - 2 * GUN_BASE_R - GUN_BARREL_R);
        gun(gun_length);
        glPushMatrix();
          glTranslatef(0.0, 0.0, GUN_BASE_R * 2.0 + GUN_BARREL_R + gun_length);
          gunBase();
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();
    // left upper wing
    glPushMatrix();
      glTranslatef(2 * BIG_HEX_SIZE / 3.0, BIG_HEX_SIZE / 2, 0.0);
      glRotatef(wing_angle, 0, 0, 1.0);
      left_wing();
      glPushMatrix();
        glTranslatef(BIG_HEX_SIZE / 3.0 + CYLINDER_R, WING_WIDTH + CYLINDER_R, -BACK_LENGTH - CYLINDER_R);
        engine();
        glPushMatrix();
          glTranslatef(0.0, 0.0, BACK_LENGTH + CYLINDER_R + 1.0);
          (*pss)[pss_index++]->glMat = matCamInverse * glGetMatrix(GL_MODELVIEW_MATRIX);
        glPopMatrix();
      glPopMatrix();
      glPushMatrix();
        glTranslatef((WING_LENGTH - GUN_BASE_R), WING_WIDTH + GUN_BASE_R, - BACK_LENGTH - gun_length - 2 * GUN_BASE_R - GUN_BARREL_R);
        gun(gun_length);
        glPushMatrix();
          glTranslatef(0.0, 0.0, GUN_BASE_R * 2.0 + GUN_BARREL_R + gun_length);
          gunBase();
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();
    // left lower wing
	  glPushMatrix();
      glTranslatef(2 * BIG_HEX_SIZE / 3.0, BIG_HEX_SIZE / 2 - WING_WIDTH, 0.0);
      glRotatef(wing_angle, 0, 0, -1.0);
      left_wing();
      glPushMatrix();
        glTranslatef(BIG_HEX_SIZE / 3.0 + CYLINDER_R, -CYLINDER_R, -BACK_LENGTH - CYLINDER_R);
        engine();
        glPushMatrix();
          glTranslatef(0.0, 0.0, BACK_LENGTH + CYLINDER_R + 1.0);
          (*pss)[pss_index++]->glMat = matCamInverse * glGetMatrix(GL_MODELVIEW_MATRIX);
        glPopMatrix();
      glPopMatrix();
      glPushMatrix();
        glTranslatef((WING_LENGTH - GUN_BASE_R), -GUN_BASE_R, - BACK_LENGTH - gun_length - 2 * GUN_BASE_R - GUN_BARREL_R);
        gun(gun_length);
        glPushMatrix();
          glTranslatef(0.0, 0.0, GUN_BASE_R * 2.0 + GUN_BARREL_R + gun_length);
          gunBase();
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();
    glPushMatrix();
      glTranslatef(BIG_HEX_SIZE / 3.0, BIG_HEX_SIZE, -BACK_LENGTH);
      glRotatef(cock_angle, 1.0, 0, 0);
      cockpit();
    glPopMatrix();
    glPushMatrix();
      glTranslatef(BIG_HEX_SIZE / 2 - GEAR_W / 2, 0.0, -TOTAL_LENGTH + 2.0);
      glRotatef(landing_angle, -1.0, 0, 0);
      landingGear(landing_length);
      glPushMatrix();
        glTranslatef(0.0, 0.0, -landing_length);
        glRotatef(landing_angle, 1.0, 0, 0);
        landingSled();
      glPopMatrix();
    glPopMatrix();
    glPushMatrix();
      glTranslatef(BIG_HEX_SIZE / 3, 0.0, -0.5);
      glRotatef(landing_angle, -1.0, 0, 0);
      landingGear(landing_length);
      glPushMatrix();
        glTranslatef(0.0, 0.0, -landing_length);
        glRotatef(landing_angle, 1.0, 0, 0);
        landingSled();
      glPopMatrix();
    glPopMatrix();
    glPushMatrix();
      glTranslatef(2 * BIG_HEX_SIZE / 3.0 - GEAR_W, 0.0, -.5);
      glRotatef(landing_angle, -1.0, 0, 0);
      landingGear(landing_length);
      glPushMatrix();
        glTranslatef(0.0, 0.0, -landing_length);
        glRotatef(landing_angle, 1.0, 0, 0);
        landingSled();
      glPopMatrix();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(BIG_HEX_SIZE / 2, BIG_HEX_SIZE, -2 * BACK_LENGTH / 3 );
        glRotatef(r2_rot, 0, 1.0, 0);
        r2();
        glPushMatrix();
          glTranslatef(0.0, 0.5*R2_SIZE,-R2_SIZE/10.0 -1.73205080757 / 2*R2_SIZE);
          glScalef(1.5, 1.5, 1.5);
          r2_eye();
        glPopMatrix();
        glPushMatrix();
          glRotatef(90.0, -1.0, 0.0, 0.0);
          r2_top();
        glPopMatrix();
    glPopMatrix();
  glPopMatrix();
  
	endDraw();
}

void r2_top() {
  setDiffuseColor( 0.10, 0.10, 0.70 );
  setAmbientColor( 0.10, 0.10, 0.70 );
  glPushMatrix();
    drawCylinder(R2_SIZE, R2_TOP, R2_TOP*0.3);
  glPopMatrix();
}

void r2_eye() {
  setDiffuseColor( 0.10, 0.10, 0.10 );
  setAmbientColor( 0.10, 0.10, 0.10 );
  glPushMatrix();
    drawCylinder(EYE_WIDTH, R2_SIZE/10.0, R2_SIZE/10.0);
  glPopMatrix();
}

void r2() {
  setDiffuseColor( 0.80, 0.80, 0.80 );
	setAmbientColor( 0.80, 0.80, 0.80 );
  glPushMatrix();
    drawSphere(R2_SIZE);
  glPopMatrix();
}

void landingGear(float length) {
  setDiffuseColor( 0.50, 0.50, 0.50 );
	setAmbientColor( 0.50, 0.50, 0.50 );
  glPushMatrix();
    drawBox(GEAR_W, GEAR_T, -length);
  glPopMatrix();
}

void landingSled() {
  setDiffuseColor( 0.50, 0.50, 0.50 );
	setAmbientColor( 0.50, 0.50, 0.50 );
  glPushMatrix();
    drawBox(GEAR_W, GEAR_T, -GEAR_W * 2);
  glPopMatrix();
}

void gun(float gun_length) {
  setDiffuseColor( 0.50, 0.50, 0.50 );
	setAmbientColor( 0.50, 0.50, 0.50 );
  glPushMatrix();
    drawCylinder(GUN_BASE_R * 2.0, GUN_BARREL_R / 2.0, 3.0 * GUN_BARREL_R / 5.0);
    glTranslatef(0.0, 0.0, GUN_BASE_R * 2.0);
    drawCylinder(GUN_BARREL_R, GUN_BASE_R, GUN_BARREL_R);
    glTranslatef(0.0, 0.0, GUN_BARREL_R);
    drawCylinder(gun_length, GUN_BARREL_R, GUN_BARREL_R);
  glPopMatrix();
}

void gunBase() {
    drawCylinder(GUN_BARREL_R, GUN_BARREL_R, GUN_BASE_R);
    glTranslatef(0.0, 0.0, GUN_BARREL_R);
    drawCylinder(BACK_LENGTH - GUN_BARREL_R, GUN_BASE_R, GUN_BASE_R);
    glTranslatef(0.0, 0.0, BACK_LENGTH - GUN_BARREL_R);
    drawCylinder(GUN_BARREL_R, GUN_BASE_R, GUN_BARREL_R); 
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

void left_wing() { 
  setDiffuseColor( 0.50, 0.50, 0.50 );
	setAmbientColor( 0.50, 0.50, 0.50 );
	glPushMatrix();
    drawBox(WING_LENGTH, WING_WIDTH, -BACK_LENGTH);
  glPopMatrix();

}
void right_wing() {
  setDiffuseColor( 0.50, 0.50, 0.50 );
	setAmbientColor( 0.50, 0.50, 0.50 );
	glPushMatrix();
    drawBox(-WING_LENGTH, WING_WIDTH, -BACK_LENGTH);
  glPopMatrix();
}

void engine() {
  setDiffuseColor(.35, .35, .35);
  setAmbientColor(.35, .35, .35);
  glPushMatrix();
    drawCylinder(1.0 + BACK_LENGTH + CYLINDER_R, CYLINDER_R, CYLINDER_R);
  glPopMatrix();
}

void cockpit() {
  setDiffuseColor( 0.80, 0.80, 1.0 );
	setAmbientColor( 0.80, 0.80, 1.0 );
  double a [] = {0.0, 0.0, 0.0};
  double b [] = {BIG_HEX_SIZE / 6.0 - SMALL_HEX_SIZE / 6.0, -(BIG_HEX_SIZE - SMALL_HEX_SIZE), -1.0};
  double c [] = {BIG_HEX_SIZE / 6.0 - SMALL_HEX_SIZE / 6.0, -(BIG_HEX_SIZE - SMALL_HEX_SIZE), -1.0 - COCKPIT_SIZE};
  double d [] = {BIG_HEX_SIZE / 6.0 - SMALL_HEX_SIZE / 6.0, 0.0, -1.0};
  double e [] = {BIG_HEX_SIZE / 3, 0.0, 0.0};
  double f [] = {BIG_HEX_SIZE / 6.0 + SMALL_HEX_SIZE / 6.0, -(BIG_HEX_SIZE - SMALL_HEX_SIZE), -1.0};
  double g [] = {BIG_HEX_SIZE / 6.0 + SMALL_HEX_SIZE / 6.0, -(BIG_HEX_SIZE - SMALL_HEX_SIZE), -1.0 - COCKPIT_SIZE};
  double h [] = {BIG_HEX_SIZE / 6.0 + SMALL_HEX_SIZE / 6.0, 0, -1.0};

  drawTriangle(d[0], d[1], d[2],
               e[0], e[1], e[2],
               h[0], h[1], h[2]);
  drawTriangle(d[0], d[1], d[2],
               h[0], h[1], h[2],
               c[0], c[1], c[2]);
  drawTriangle(c[0], c[1], c[2],
               h[0], h[1], h[2],
               g[0], g[1], g[2]);
  drawTriangle(a[0], a[1], a[2],
               e[0], e[1], e[2],
               d[0], d[1], d[2]);
  // right side
  drawTriangle(a[0], a[1], a[2],
               d[0], d[1], d[2],
               c[0], c[1], c[2]);
  drawTriangle(a[0], a[1], a[2],
               c[0], c[1], c[2],
               b[0], b[1], b[2]);
  
  drawTriangle(g[0], g[1], g[2],
               h[0], h[1], h[2],
               e[0], e[1], e[2]);
  drawTriangle(g[0], g[1], g[2],
               e[0], e[1], e[2],
               f[0], f[1], f[2]);
}

void body() {
	double f_a [] = {BIG_HEX_SIZE / 2 - (SMALL_HEX_SIZE / 6), 0.0, -TOTAL_LENGTH};
  double f_b [] = {BIG_HEX_SIZE / 2 - (SMALL_HEX_SIZE / 2), SMALL_HEX_SIZE / 2, -TOTAL_LENGTH};
	double f_c [] = {BIG_HEX_SIZE / 2 - (SMALL_HEX_SIZE / 6), SMALL_HEX_SIZE, -TOTAL_LENGTH};
	double f_d [] = {BIG_HEX_SIZE / 2 + (SMALL_HEX_SIZE / 6), SMALL_HEX_SIZE, -TOTAL_LENGTH};
  double f_e [] = {BIG_HEX_SIZE / 2 + (SMALL_HEX_SIZE / 2), SMALL_HEX_SIZE / 2, -TOTAL_LENGTH};
  double f_f [] = {BIG_HEX_SIZE / 2 + (SMALL_HEX_SIZE / 6), 0.0, -TOTAL_LENGTH};
  double f_m [] = {BIG_HEX_SIZE / 2, SMALL_HEX_SIZE / 2, -TOTAL_LENGTH - 1.0};

  double b_a [] = {BIG_HEX_SIZE / 3, 0.0, 0.0};
  double b_b [] = {0, BIG_HEX_SIZE / 2, 0.0};
	double b_c [] = {BIG_HEX_SIZE / 3, BIG_HEX_SIZE, 0.0};
	double b_d [] = {2 * BIG_HEX_SIZE / 3, BIG_HEX_SIZE, 0.0};
  double b_e [] = {BIG_HEX_SIZE, BIG_HEX_SIZE / 2, 0.0};
  double b_f [] = {2 * BIG_HEX_SIZE / 3, 0.0, 0.0};
  double b_m [] = {BIG_HEX_SIZE / 2, BIG_HEX_SIZE / 2, 0.0};

  setDiffuseColor( 0.50, 0.50, 0.50 );
	setAmbientColor( 0.50, 0.50, 0.50 );
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
  drawTriangle(b_a[0], b_a[1], b_a[2],
               b_m[0], b_m[1], b_m[2],
               b_f[0], b_f[1], b_f[2]);
  drawTriangle(b_b[0], b_b[1], b_b[2],
               b_m[0], b_m[1], b_m[2],
               b_a[0], b_a[1], b_a[2]);
	drawTriangle(b_c[0], b_c[1], b_c[2],
               b_m[0], b_m[1], b_m[2],
               b_b[0], b_b[1], b_b[2]);
  drawTriangle(b_d[0], b_d[1], b_d[2],
               b_m[0], b_m[1], b_m[2],
               b_c[0], b_c[1], b_c[2]);
  drawTriangle(b_e[0], b_e[1], b_e[2],
               b_m[0], b_m[1], b_m[2],
               b_d[0], b_d[1], b_d[2]);
  drawTriangle(b_f[0], b_f[1], b_f[2],
               b_m[0], b_m[1], b_m[2],
               b_e[0], b_e[1], b_e[2]);
  // front cone to mid hex
  drawTriangle(f_a[0], f_a[1], f_a[2],
               f_b[0], f_b[1], f_b[2],
               f_a[0], f_a[1], -1.0 - BACK_LENGTH);
  drawTriangle(f_b[0], f_b[1], f_b[2],
               f_b[0], f_b[1], -1.0 - BACK_LENGTH,
               f_a[0], f_a[1], -1.0 - BACK_LENGTH);
  
  
  setDiffuseColor( 0.80, 0.20, 0.20 );
	setAmbientColor( 0.80, 0.20, 0.20 );
  drawTriangle(f_b[0], f_b[1], f_b[2],
               f_c[0], f_c[1], f_c[2],
               f_b[0], f_b[1], -1.0 - BACK_LENGTH);
  drawTriangle(f_c[0], f_c[1], f_c[2],
               f_c[0], f_c[1], -1.0 - BACK_LENGTH,
               f_b[0], f_b[1], -1.0 - BACK_LENGTH);
  
  drawTriangle(f_d[0], f_d[1], f_d[2],
               f_e[0], f_e[1], f_e[2],
               f_d[0], f_d[1], -1.0 - BACK_LENGTH);
  drawTriangle(f_e[0], f_e[1], f_e[2],
               f_e[0], f_e[1], -1.0 - BACK_LENGTH,
               f_d[0], f_d[1], -1.0 - BACK_LENGTH);
  setDiffuseColor( 0.80, 0.80, 0.80 );
	setAmbientColor( 0.80, 0.80, 0.80 );



  drawTriangle(f_c[0], f_c[1], f_c[2],
               f_d[0], f_d[1], f_d[2],
               f_c[0], f_c[1], -1.0 - BACK_LENGTH);
  drawTriangle(f_d[0], f_d[1], f_d[2],
               f_d[0], f_d[1], -1.0 - BACK_LENGTH,
               f_c[0], f_c[1], -1.0 - BACK_LENGTH);

  setDiffuseColor( 0.50, 0.50, 0.50 );
	setAmbientColor( 0.50, 0.50, 0.50 );

  drawTriangle(f_e[0], f_e[1], f_e[2],
               f_f[0], f_f[1], f_f[2],
               f_e[0], f_e[1], -1.0 - BACK_LENGTH);
  drawTriangle(f_f[0], f_f[1], f_f[2],
               f_f[0], f_f[1], -1.0 - BACK_LENGTH,
               f_e[0], f_e[1], -1.0 - BACK_LENGTH);
  drawTriangle(f_f[0], f_f[1], f_f[2],
               f_a[0], f_a[1], f_a[2],
               f_f[0], f_f[1], -1.0 - BACK_LENGTH);
  drawTriangle(f_a[0], f_a[1], f_a[2],
               f_a[0], f_a[1], -1.0 - BACK_LENGTH,
               f_f[0], f_f[1], -1.0 - BACK_LENGTH);
  // mid hex transition
  drawTriangle(f_a[0], f_a[1], -1.0 - BACK_LENGTH,
               f_b[0], f_b[1], -1.0 - BACK_LENGTH,
               b_a[0], b_a[1], -BACK_LENGTH);
  drawTriangle(f_b[0], f_b[1], -1.0 - BACK_LENGTH,
               b_b[0], b_b[1], -BACK_LENGTH,
               b_a[0], b_a[1], -BACK_LENGTH);
  
  drawTriangle(f_b[0], f_b[1], -1.0 - BACK_LENGTH,
               f_c[0], f_c[1], -1.0 - BACK_LENGTH,
               b_b[0], b_b[1], -BACK_LENGTH);
  drawTriangle(f_c[0], f_c[1], -1.0 - BACK_LENGTH,
               b_c[0], b_c[1], -BACK_LENGTH,
               b_b[0], b_b[1], -BACK_LENGTH);
  
  drawTriangle(f_c[0], f_c[1], -1.0 - BACK_LENGTH,
               f_d[0], f_d[1], -1.0 - BACK_LENGTH,
               b_c[0], b_c[1], -BACK_LENGTH);
  drawTriangle(f_d[0], f_d[1], -1.0 - BACK_LENGTH,
               b_d[0], b_d[1], -BACK_LENGTH,
               b_c[0], b_c[1], -BACK_LENGTH);
  
  drawTriangle(f_d[0], f_d[1], -1.0 - BACK_LENGTH,
               f_e[0], f_b[1], -1.0 - BACK_LENGTH,
               b_d[0], b_d[1], -BACK_LENGTH);
  drawTriangle(f_e[0], f_b[1], -1.0 - BACK_LENGTH,
               b_e[0], b_b[1], -BACK_LENGTH,
               b_d[0], b_d[1], -BACK_LENGTH);
 
  drawTriangle(f_e[0], f_e[1], -1.0 - BACK_LENGTH,
               f_f[0], f_f[1], -1.0 - BACK_LENGTH,
               b_e[0], b_e[1], -BACK_LENGTH);
  drawTriangle(f_f[0], f_f[1], -1.0 - BACK_LENGTH,
               b_f[0], b_f[1], -BACK_LENGTH,
               b_e[0], b_e[1], -BACK_LENGTH);
   
  drawTriangle(f_f[0], f_f[1], -1.0 - BACK_LENGTH,
               f_a[0], f_a[1], -1.0 - BACK_LENGTH,
               b_f[0], b_f[1], -BACK_LENGTH);
  drawTriangle(f_a[0], f_a[1], -1.0 - BACK_LENGTH,
               b_a[0], b_a[1], -BACK_LENGTH,
               b_f[0], b_f[1], -BACK_LENGTH);

  // to back hex
  drawTriangle(b_a[0], b_a[1], -BACK_LENGTH,
               b_b[0], b_b[1], -BACK_LENGTH,
               b_a[0], b_a[1], 0.0);
  drawTriangle(b_b[0], b_b[1], -BACK_LENGTH,
               b_b[0], b_b[1], 0.0,
               b_a[0], b_a[1], 0.0);
  
  drawTriangle(b_b[0], b_b[1], -BACK_LENGTH,
               b_c[0], b_c[1], -BACK_LENGTH,
               b_b[0], b_b[1], 0.0);
  drawTriangle(b_c[0], b_c[1], -BACK_LENGTH,
               b_c[0], b_c[1], 0.0,
               b_b[0], b_b[1], 0.0);
  
  drawTriangle(b_c[0], b_c[1], -BACK_LENGTH,
               b_d[0], b_d[1], -BACK_LENGTH,
               b_c[0], b_c[1], 0.0);
  drawTriangle(b_d[0], b_d[1], -BACK_LENGTH,
               b_d[0], b_d[1], 0.0,
               b_c[0], b_c[1], 0.0);
 
  drawTriangle(b_d[0], b_d[1], -BACK_LENGTH,
               b_e[0], b_e[1], -BACK_LENGTH,
               b_d[0], b_d[1], 0.0);
  drawTriangle(b_e[0], b_e[1], -BACK_LENGTH,
               b_e[0], b_e[1], 0.0,
               b_d[0], b_d[1], 0.0);
   
  drawTriangle(b_e[0], b_e[1], -BACK_LENGTH,
               b_f[0], b_f[1], -BACK_LENGTH,
               b_e[0], b_e[1], 0.0);
  drawTriangle(b_f[0], b_f[1], -BACK_LENGTH,
               b_f[0], b_f[1], 0.0,
               b_e[0], b_e[1], 0.0);
  
  drawTriangle(b_f[0], b_f[1], -BACK_LENGTH,
               b_a[0], b_a[1], -BACK_LENGTH,
               b_f[0], b_f[1], 0.0);
  drawTriangle(b_a[0], b_a[1], -BACK_LENGTH,
               b_a[0], b_a[1], 0.0,
               b_f[0], b_f[1], 0.0);
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
// min max step default
    controls[BODY_HEIGHT] = ModelerControl("body height (body_height)", 0.0, 100.0, 0.1, 1.0 );
    controls[BODY_MOVEMENT] = ModelerControl("body movement (body_mov)", 0.0, 100.0, 0.1, 0.0 );
    controls[YAW] = ModelerControl("yaw (yaw)", -180.0, 180.0, 0.1, 0.0 );
    controls[PITCH] = ModelerControl("pitch (pitch)", -180.0, 180.0, 0.1, 0.0 );
    controls[ROLL] = ModelerControl("roll (roll)", -180.0, 180.0, 0.1, 0.0 );
    
    controls[WING_ANGLE] = ModelerControl("wing angle (wing_angle)", 0.0, 30.0, 0.1, 0.0 );
	  controls[COCKPIT_ANGLE] = ModelerControl("cockpit angle (cock_angle)", 0.0, 90.0, 0.1, 0.0 );
    controls[LANDING_GEAR_ANGLE] = ModelerControl("landing gear angle (landing_angle)", 0.0, 90.0, 0.1, 0.0 );
    controls[LANDING_GEAR_LENGTH] = ModelerControl("landing gear length (landing_length)", .1, 1.5, 0.01, 1.0 );
    controls[GUN_LENGTH] = ModelerControl("gun length (gun_length)", 0.0, 5.0, 0.1, 5.0 );
    controls[R2_ROTATION] = ModelerControl("r2 rotation (r2_rot)", -180.0, 180.0, 0.1, 0.0 );
    controls[PARTICLE_COUNT] = ModelerControl("particle count (pc)", 0.0, 5.0, 0.1, 5.0 );

	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.
    ParticleSystem *ps = new EngineSystem();
    ps->setPc(1.0);
    ModelerApplication::Instance()->PutParticleSystem(ps);
    ps = new EngineSystem();
    ps->setPc(1.0);
    ModelerApplication::Instance()->PutParticleSystem(ps);
    ps = new EngineSystem();
    ps->setPc(1.0);
    ModelerApplication::Instance()->PutParticleSystem(ps);
    ps = new EngineSystem();
    ps->setPc(1.0);
    ModelerApplication::Instance()->PutParticleSystem(ps);

    ModelerApplication::Instance()->Init(&createXWing, controls, NUMCONTROLS);
    return ModelerApplication::Instance()->Run();
}
