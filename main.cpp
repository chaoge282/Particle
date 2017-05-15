#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cmath>
#include <vector>
#include "Camera.h"
#include "Vector.h"
#include "gauss.h"
#include "Particle.h"

using namespace std;


#define WIDTH           1000	// initial window dimensions
#define HEIGHT          800
#define TimerDelay       30
#define RADIUS           2
#define OBJECTRADIUS    2
Camera *camera;
void TimerCallback(int value);

enum COLOR{MAT_WHITE, MAT_RED, MAT_ORANGE, MAT_YELLOW, MAT_GREEN, MAT_BLUE, MAT_VIOLET};
// colors indexed for cycling material colors
float hues[][3] = { {1, 1, 1},    // white
  {1, 0, 0},    // red
  {1, 0.5, 0},  // orange
  {1, 1, 0},    // yellow
  {0, 1, 0},    // green
  {0, 0, 1},    // blue
  {0.5, 0, 1}   // violet
};

static bool expSign = false;
static bool stop;
static double Tstep=0.05;
static int Color=0;
double particleDense = 0.001;
double particleSize = 3;

Vector3d Ball(0,0,-30);
Vector3d Velocity(0,20,0);
Vector3d G(0,-10,0);
Vector3d particlePo;
Vector3d particleVe;
Vector3d pG(0,-1,0);
Vector3d sphere(0,14,0);

std::vector<Particle> particles;
std::vector<Particle>::iterator it;


void drawInsect() {

					glPushMatrix();
                        glColor3f(0.0,0.0,1.0);
							glTranslatef(10.0, 0.0, 0.0);
							glScalef (1.5, 0.5, 0.7);
							glutSolidSphere(OBJECTRADIUS,10,10);
						glPopMatrix();
						glPushMatrix();
                            glColor3f(1.0,0.0,0.0);
				            glBegin(GL_POLYGON);
                                glVertex3f(13.0,0.0,0.0);
                                glVertex3f(16.0,2.0,0.0);
                                glVertex3f(16.0,-2.0,0.0);
                            glEnd();

                	glPopMatrix();

}



void drawBall(Vector3d& p){
	glColor3f(hues[Color+1][0],hues[Color+1][1],hues[Color+1][2]);
	glTranslatef(p.x, p.y, p.z);
	glutSolidSphere(RADIUS,30,40);
}

bool detectSphereCollision(Vector3d& particlePosition, Vector3d& particleVelocity,  Vector3d& particlePo  , Vector3d&  particleVe)
{

    Vector3d LVector = sphere - particlePosition;
    double L = LVector.norm();
    Vector3d VDirection = (particlePo - particlePosition).normalize();
    double tc = LVector * VDirection;
    if (tc < 0.0) {
        return  false;
    }
    double d = sqrt((L*L)-(tc*tc));
    if (d > RADIUS) {
        return false;
    }

    double t1c = sqrt(RADIUS*RADIUS-d*d);

    double t1 = tc - t1c;
    double t2 = tc + t1c;

    Vector3d collisionPoint = particlePosition + t1*VDirection;


    Vector3d normalVector = (collisionPoint - sphere).normalize();
    double d1 = (particlePosition - collisionPoint)*normalVector;
    double d2 = (particlePo - collisionPoint)*normalVector;
    if (d1*d2<0) {
        particlePo = particlePo - 1.7*d2*normalVector;
        Vector3d vn = (particleVelocity*normalVector)*normalVector;
        Vector3d vt = particleVelocity - vn;
        particleVe = -0.3*vn + 0.3*vt;
        it->setColor(Vector4d(1,1,1,1));
    }

    return true;
}

void particlesGenerator(){
	for(float i=-2; i<2; i+=particleDense){
		float x =gauss(0,1,1);
		if(x<0) x=0;
		if(x>1) x=1;
		particles.push_back(Particle(Vector3d(i,20,i),
            Vector3d(gauss(0,3,1),gauss(0,1,1),gauss(0,1,1)),
            Vector4d(0,x+0.4,1,1),1,gauss(0,1,1)*10+25,
            particleSize));//position,velocity,color,mass,lifespan,pointsize,stopSign
	}
}

void Simulate(){
	if(!expSign){
		Ball= Ball + Velocity* Tstep;
		Velocity = Velocity + G*Tstep;

		if(Velocity.y <= 0) {
			expSign= true;
			particlesGenerator();
		}
	}
		for (it = particles.begin(); it!=particles.end();++it){
		it->setLifeSpan(it->getLifeSpan()+1);
		particlePo = it->getPosition()+it->getVelocity()*Tstep;
		particleVe =it->getVelocity()+ pG*Tstep;
		if (particlePo.y >10) {
                Vector3d PTmp = it->getPosition();
                Vector3d VTmp = it->getVelocity();
		if (PTmp.y>(sphere.y-RADIUS)&&PTmp.y<(sphere.y+RADIUS)             && PTmp.x>(sphere.x-RADIUS)&&PTmp.x<(sphere.x+RADIUS)           &&PTmp.z>(sphere.z-RADIUS)&&PTmp.z<(sphere.z+RADIUS)) {
        if(detectSphereCollision(PTmp, VTmp, particlePo, particleVe))
                         {

                         }
            	}
			}
		it->setPosition(particlePo);
		it->setVelocity(particleVe);

		if(it->getLifeSpan()>100){
			it =particles.erase(it);
			--it;

			}

		}
		if(particles.size()<=3000 && Velocity.y<=0 ){
			Ball.set(0,0,0);
			Velocity.set(0,20,0);
			expSign=false;
			}
	}
void TimerCallback(int value){
	if(value==0)
{
	Simulate();
	glutPostRedisplay();
	if(!stop){
		glutTimerFunc(TimerDelay,TimerCallback,0);
		}
	}
}

void explore(){
		for(int i=0;i< particles.size(); i++){
			glPointSize(particles[i].getPointSize());
			glColor4f(particles[i].getColor().x, particles[i].getColor().y, particles[i].getColor().z, particles[i].getColor().w);
        glBegin(GL_POINTS);
		glVertex3f(particles[i].getPosition().x, particles[i].getPosition().y, particles[i].getPosition().z);
        glEnd();
	}
}

void init(){
	 // set up camera
  // parameters are eye point, aim point, up vector
  camera = new Camera(Vector3d(0,0,150), Vector3d(0, 0, 0),
              Vector3d(0, 1, 0));

  // grey background for window
  glClearColor(0, 0, 0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_NORMALIZE);
  particles.reserve(10000);
//  particlesGenerator();
}

void PerspDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

  // draw the camera created in perspective
  camera->PerspectiveDisplay(WIDTH, HEIGHT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

//	glColor4f(0,1,0,0.5);
//    glTranslatef(sphere.x,sphere.y,sphere.z);
//    glutSolidSphere(RADIUS, 20, 20);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
	explore();
    drawInsect();
	if(!expSign)
		drawBall(Ball);
  glutSwapBuffers();
}



//
// Mouse Button Callback
//
void handleButtons(int button, int state, int x, int y){
  camera->HandleMouseEvent(button,state,x,y);
  glutPostRedisplay();
}

void handleMotion(int x,int y){
	camera->HandleMouseMotion(x,y);
	glutPostRedisplay();
}

void handleKey(unsigned char key, int x, int y) {
  switch (key) {
  case 't': case 'T':
		Ball.set(0,0,0);
		Velocity.set(0,20,0);
		stop=false;
	glutTimerFunc(TimerDelay,TimerCallback,0);
//	Simulate();
	break;
  case 's': case 'S':
		Tstep +=0.01;
		break;
  case 'l' : case 'L':
 		Tstep -=0.01;
		break;
  case 'r': case 'R':
    // reset the camera to its initial position
//    camera->Reset();
	particles.clear();
	stop=true;
	Ball.set(0,0,0);
//	Velocity.set(0,0,0);
//	Simulate();
//	glutTimerFunc(TimerDelay,TimerCallback,0);
    break;
  case 'f': case 'F':
    camera->SetCenterOfFocus(Vector3d(0, 0, 0));
    break;
  case 'm':           // M -- cycle through material colors
    case 'M':
      Color = (Color == MAT_VIOLET? MAT_WHITE: Color + 1);
      glutPostRedisplay();
      break;

  case 'q': case 'Q':   // q or esc - quit
    exit(0);
  }

  glutPostRedisplay();
}



int main(int argc, char* argv[]){

  // start up the glut utilities
  glutInit(&argc, argv);

  // create the graphics window, giving width, height, and title text
  // and establish double buffering, RGBA color
  // Depth buffering must be available for drawing the shaded model
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(50,50);
  glutCreateWindow("Particle System");

  init();
  // register callback to draw graphics when window needs updating
  glutDisplayFunc(PerspDisplay);
  glutMouseFunc(handleButtons);
  glutMotionFunc(handleMotion);
  glutKeyboardFunc(handleKey);
  glutMainLoop();
  return(0);
}
