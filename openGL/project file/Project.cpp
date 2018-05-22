/*
warbirdSimulation.cpp

465 utility include files:  shader465.hpp, triModel465.hpp

This program is constructed from the manyModelsStatic example the models are indexed
using arrays.

Keys:
'v' moves to the next camera 'x' moves to the previous camera
'w' warps the ship
's' changes ship speed
'f' fires missle
arrow keys move
ctrl movement modifier
't' change TQ
light toggles: 'p' point, 'h' headlight, 'd' debug color

Vanny Nguyen
12/10/2017
*/

// define your target operating system: __Windows__  __Linux__  __Mac__
# define __Windows__ 
# include "./includes465/include465.hpp"

//define constants/variables
//PHASE I
const int X = 0;
const int Y = 1;
const int Z = 2;
const int START = 0;
const int STOP = 1;
const int ruber = 0;
const int unum = 1;
const int duo = 2;
const int primus = 3;
const int secundus = 4;
const int ship = 5;

//camera swap
int currentCam = 1; // now starts in ship view
bool nextCam = false;
bool previousCam = false;
int speed = 10;

// PHASE II
//add new missile & base models
const int warbirdMissile = 6;
const int warbirdMissileSpeed = 10;

const int unumMissile = 7;
const int unumMissileBase = 8;
const int secundusMissileBase = 9;
const int secundusMissile = 10;
const int baseMissileSpeed = 5;

const int missileDetectionRadius = 3000;


//gravity
int gravity = 0;
float gravityForce;
const int gravityMax = 90000000;

//movement
int accelerate = 0;
int yaw = 0;
int pitch = 0;
int roll = 0;
bool warp = false;
int currentWarp = 1; // Warp set to Unum
int TQ = 5;

//missiles
bool warbirdShootMissile = false;
bool unumShootMissile = false;
bool secundusShootMissile = false;
bool warbirdCanShoot = true;

//collision detection
bool warbirdCollision = false;
bool unumBaseCollision = false;
bool secundusBaseCollision = false;

//PHASE III
const int skybox = 11;
bool initializing = true;
bool pointLightSetOn = true;
bool headLightSetOn = true;
bool debugSetOn = false;
bool ambientOn = true;


// load models, count vertices
const int nShapes = 12;
char * modelFile[nShapes] = { "Ruber.tri", "Unum.tri", "Duo.tri", "Primus.tri", "Secundus.tri", "Warbird.tri", "Missile.tri", "Missile.tri", "Missilebase.tri", "Missilebase.tri", "Missile.tri","" };
const int nVertices[nShapes] = { 264 * 3, 264 * 3, 264 * 3, 264 * 3, 264 * 3, 996 * 3, 252 * 3, 252 * 3, 12 * 3, 12 * 3, 252 * 3, 2 * 3 };
//excel sheet of bounding radius
float modelSize[nShapes] = { 2000.0f, 200.0f, 400.0f, 100.0f, 150.0f, 100.0f, 25.0f, 25.0f, 30.0f, 30.0f, 25.0f, 50000.0f }; //ship & missile bounding sphere radius +10 according to spec sheet
																															 //excel sheet of radian values
float modelRadians[nShapes] = { 0.0f, 0.004f, 0.002f, 0.004f, 0.002f, 0.0f, 0.0f, 0.0f, 0.004f, 0.002f, 0.0f, 0.0f };

// array for bounding radius of model
float boundingRadius[nShapes];

//scale values for models
float scaleValue[nShapes];

// display state and "state strings" for title display
// window title strings
char missileStr[50];
char baseStr[50] = "Warbird Simulation | ";
char updateStr[15];
char fpsStr[15];
char unumBaseStr[50] = "";
char secundusBaseStr[50] = "";
char gameStr[50] = "";
char viewStr[50] = "View: Warbird | ";
char titleStr[500];

int frameCount = 0;
double currentTime, lastTime, timeInterval;

int warbirdMissileCount = 9;
int unumMissileCount = 5;
int secundusMissileCount = 5;

int warbirdMissileTimer = 0;
int unumMissileTimer = 0;
int secundusMissileTimer = 0;

// number of textures
const int nTextures = 6;
char * textureFile = { "skybox.raw" };
char * vertexShaderFile = "phase3Vertex.glsl";
char * fragmentShaderFile = "phase3Fragment.glsl";
GLuint shaderProgram;
GLuint textures;
GLuint VAO[nShapes];      // Vertex Array Objects
GLuint buffer[nShapes];   // Vertex Buffer Objects
GLuint IBO, vTexCoord; //indexBufferObject

					   // Shader handles, matrices, etc
GLuint MVP;  // Model View Projection matrix's handle
GLuint MV; // ModelView handle
GLuint NM; // NormalMatrix handle
GLuint TEX; //Texture handle
GLuint POINTLOCATION;
GLuint POINTINTENSITY;
GLuint POINTON;
GLuint HEADON;
GLuint HEADLOCATION;
GLuint HEADINTENSITY;
GLuint DEBUGON;
GLuint AMBIENTON;

float textureRadians = 1.57079632679489661923f;
glm::vec3 scale[nShapes];       // set in init()

								//SET STARTING POSITION FOR MODELS
glm::vec3 translate[nShapes] = { glm::vec3(0, 0, 0), glm::vec3(4000, 0, 0), glm::vec3(9000, 0, 0), glm::vec3(-900, 0, 0), glm::vec3(-1750, 0, 0), glm::vec3(5000, 1000, 5000), glm::vec3(4900, 1000, 4850), glm::vec3(4900, 1050, 4850), glm::vec3(4000, 225, 0), glm::vec3(-1750, 175, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) };

glm::mat4 rotation[nShapes];
glm::mat4 texRotation; // used for rotating texture
glm::mat4 orientation[nShapes];

static const GLfloat skyboxPoints[] = {
	-1.0f, -1.0f,  1.0f, 1.0f, // 0 bottom left back corner
	-1.0f, -1.0f, -1.0f, 1.0f, // 1 bottom left forward corner
	-1.0f,  1.0f,  1.0f, 1.0f, // 2 top left back corner
	-1.0f,  1.0f, -1.0f, 1.0f, // 3 top left forward corner
};

static const unsigned int indices[] = {
	0, 1, 2, // 0 left square bottom
	1, 2, 3, // 1 left square top
};


static const GLfloat texCoords[] = {
	0.0f, 0.0f,     // 0 bottom left
	1.0f, 0.0f,     // 1 bottom right
	0.0f, 1.0f,     // 2 top left
	1.0f, 1.0f,     // 3 top right
};

// vPosition, vColor, vNormal handles for each model
GLuint vPosition[nShapes], vColor[nShapes], vNormal[nShapes];

glm::mat4 modelMatrix;          // set in display()
glm::mat4 viewMatrix;           // set in init()
glm::mat4 modelViewMatrix;		// set in display()
glm::mat3 normalMatrix;
glm::mat4 projectionMatrix;     // set in reshape()
glm::mat4 ModelViewProjectionMatrix; // set in display();

glm::mat4 identity(1.0f);



void reshape(int width, int height) {
	float aspectRatio = (float)width / (float)height;
	float FOVY = glm::radians(60.0f);
	glViewport(0, 0, width, height);
	projectionMatrix = glm::perspective(FOVY, aspectRatio, 1.0f, 100000.0f);
}

// update and display animation state in window title
void updateTitle() {
	strcpy(titleStr, baseStr);
	strcat(titleStr, missileStr);

	if (unumBaseCollision) {
		strcat(unumBaseStr, "(Destroyed) | ");
	}

	if (secundusBaseCollision) {
		strcat(secundusBaseStr, "(Destroyed) | ");
	}

	if (unumBaseCollision && secundusBaseCollision) {
		strcpy(gameStr, "Mission Accomplished | ");
	}
	if (warbirdCollision) {
		strcpy(gameStr, "Mission Failed | ");
	}

	sprintf(updateStr, " U/S %4d | ", (int)(1000.0f / TQ));

	strcat(titleStr, unumBaseStr);
	strcat(titleStr, secundusBaseStr);
	strcat(titleStr, updateStr);
	strcat(titleStr, fpsStr);
	strcat(titleStr, viewStr);
	strcat(titleStr, gameStr);

	//set title
	glutSetWindowTitle(titleStr);
}

// Collision testing for warbird and missiles
void collisionTesting() {
	// warbird tests
	// hit Ruber
	if (glm::distance(getPosition(orientation[ship]), getPosition(orientation[ruber])) < (modelSize[ruber] + modelSize[ship])) {
		warbirdCollision = true;
	}
	// hit Unum
	else if (glm::distance(getPosition(orientation[ship]), getPosition(orientation[unum])) < (modelSize[unum] + modelSize[ship])) {
		warbirdCollision = true;
	}
	// hit Duo
	else if (glm::distance(getPosition(orientation[ship]), getPosition(orientation[duo])) < (modelSize[duo] + modelSize[ship])) {
		warbirdCollision = true;
	}
	// hit Primus
	else if (glm::distance(getPosition(orientation[ship]), getPosition(orientation[primus])) < (modelSize[primus] + modelSize[ship])) {
		warbirdCollision = true;
	}
	// hit Secundus
	else if (glm::distance(getPosition(orientation[ship]), getPosition(orientation[secundus])) < (modelSize[secundus] + modelSize[ship])) {
		warbirdCollision = true;
	}
	// hit by Unum's missile
	else if (glm::distance(getPosition(orientation[ship]), getPosition(orientation[unumMissile])) < (modelSize[unumMissile] + modelSize[ship])) {
		warbirdCollision = true;
		unumShootMissile = false;
	}
	// hit by Secundus missile
	else if (glm::distance(getPosition(orientation[ship]), getPosition(orientation[secundusMissile])) < (modelSize[secundusMissile] + modelSize[ship])) {
		warbirdCollision = true;
		secundusShootMissile = false;
	}

	// tests for warbird missiles hitting targets
	// warbird missile hit Unum Base
	if (glm::distance(getPosition(orientation[warbirdMissile]), getPosition(orientation[unumMissileBase])) < (modelSize[unumMissileBase] + modelSize[warbirdMissile])) {
		unumBaseCollision = true;
		warbirdShootMissile = false;
		translate[unumMissile] = glm::vec3(0, 0, 0);
	}
	// warbird missile hit Secundus base
	if (glm::distance(getPosition(orientation[warbirdMissile]), getPosition(orientation[secundusMissileBase])) < (modelSize[secundusMissileBase] + modelSize[warbirdMissile])) {
		secundusBaseCollision = true;
		warbirdShootMissile = false;
		translate[secundusMissile] = glm::vec3(0, 0, 0);
	}
}

// Orient originObject toward targetObject
bool reorientToward(int originObject, int targetObject) {
	float radian;
	glm::vec3 originObjectAt = getIn(rotation[originObject]);
	glm::vec3 target = getPosition(orientation[targetObject]) - getPosition(orientation[originObject]);
	glm::vec3 normTarget = glm::normalize(target); // normalized target vector
	glm::vec3 rotationAxis = glm::vec3(0, 0, 0);

	rotationAxis = glm::cross(normTarget, originObjectAt);
	rotationAxis = glm::normalize(rotationAxis);

	float rotationAxisDirection = rotationAxis.x + rotationAxis.y + rotationAxis.z;
	float rotationRads = glm::dot(normTarget, originObjectAt);
	radian = (2 * PI) - glm::acos(rotationRads);


	if (colinear(originObjectAt, normTarget, .08)) {
		// poor colinearity
		if (distance(getPosition(orientation[originObject]) + originObjectAt, target) > distance(getPosition(orientation[originObject]), target)) {
			//reorient
			rotation[originObject] = glm::rotate(rotation[originObject], PI, getUp(rotation[originObject]));
		}
		return true;
	}
	else {
		if (distance(normTarget, originObjectAt) < .5) {
			//no rotation
		}
		else {
			rotation[originObject] = glm::rotate(rotation[originObject], radian, rotationAxis);
		}
		return false;
	}
}

void missileTracking(int missile) {

	int target = -1;

	// warbird missile: track closest base
	if (missile == warbirdMissile) {
		if (distance(getPosition(orientation[missile]), getPosition(orientation[unumMissileBase])) < distance(getPosition(orientation[missile]), getPosition(orientation[secundusMissileBase]))) {
			if (!unumBaseCollision) {
				target = unumMissileBase;
			}
		}
		else {
			if (!secundusBaseCollision) {
				target = secundusMissileBase;
			}
		}
	}
	// base missile: track warbird
	else {
		target = ship;
	}

	// base missile: ship outside of detection radius
	if (distance(getPosition(orientation[missile]), getPosition(orientation[target])) > missileDetectionRadius) {
		target = -1;
	}

	// base missile: target found, reorient toward warbird
	if (target > 0) {
		reorientToward(missile, target);
	}
}


glm::mat4 cameraSwap(int cam) {
	// forward
	if (nextCam == true) {
		nextCam = false;
		if (currentCam == 5) {
			currentCam = 1;
		}
		else {
			currentCam++;
		}
	}
	// backwards
	if (previousCam == true) {
		previousCam = false;
		if (currentCam == 1) {
			currentCam = 5;
		}
		else {
			currentCam--;
		}
	}

	// lower than zero
	if (cam <= 0) {
		cam = currentCam;
	}

	// default now Warbird
	if (cam == 1) {
		if (!warbirdCollision) {
			sprintf(viewStr, "View: Warbird | ");
			return (glm::lookAt(getPosition(orientation[ship]) - getIn(rotation[ship]) * 1000.0f + getUp(rotation[ship]) * 300.0f, getPosition(orientation[ship] * glm::translate(identity, glm::vec3(0.0f, 300.0f, 0.0f))), getUp(rotation[ship])));
		}
		else {
			// GAME OVER CAMERA
			currentCam++;
			sprintf(viewStr, "View: Top | ");
			return (glm::lookAt(glm::vec3(0.0f, 20000.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		}
	}
	else if (cam == 2) { //top view
		sprintf(viewStr, "View: Top | ");
		return (glm::lookAt(glm::vec3(0.0f, 20000.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	}
	else if (cam == 3) { //front view
		sprintf(viewStr, "View: Front | ");
		return (glm::lookAt(glm::vec3(0.0f, 10000.0f, 20000.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	else if (cam == 4) { //Unum
		sprintf(viewStr, "View: Unum | ");
		return (glm::lookAt(getPosition(orientation[unum]) + getIn(rotation[unum]) * 4000.0f, getPosition(orientation[unum]), glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	else if (cam == 5) { //Duo
		sprintf(viewStr, "View: Duo | ");
		return (glm::lookAt(getPosition(orientation[duo]) + getIn(rotation[duo]) * 4000.0f, getPosition(orientation[duo]), glm::vec3(0.0f, 1.0f, 0.0f)));
	}
}

void warpShip() {
	int warpPoint;
	if (currentWarp == 1) {
		currentWarp = 2;
		warpPoint = unum;
	}
	else {
		currentWarp = 1;
		warpPoint = duo;
	}
	translate[ship] = getPosition(orientation[warpPoint]) + getIn(rotation[warpPoint]) * 8000.0f; // warps ship to position. No rotation.
	rotation[ship] = identity;
	orientation[ship] = glm::translate(identity, translate[ship]) * rotation[ship] * glm::scale(identity, glm::vec3(scale[ship]));
	reorientToward(ship, warpPoint);
}

void animateModels() {
	//Iterate through models and perform transformations
	for (int model = 0; model < nShapes; model++) {

		// warbird updates
		if (model == ship) {
			rotation[model] = glm::rotate(rotation[model], float(pitch * 0.02), glm::vec3(1, 0, 0));
			rotation[model] = glm::rotate(rotation[model], float(yaw * 0.02), glm::vec3(0, 1, 0));
			rotation[model] = glm::rotate(rotation[model], float(roll * 0.02), glm::vec3(0, 0, 1));

			// starting or subsequent transformation?
			if (initializing) {
				// ok, now done with set-up
				initializing = false;
			}
			else {
				translate[model] = getPosition(orientation[model]) + getIn(rotation[model]) * float(accelerate * speed);
			}
			orientation[model] = glm::translate(identity, translate[model]) * rotation[model] * glm::scale(identity, glm::vec3(scale[model]));

			// if gravity switch on
			if (gravity == 1) {
				gravityForce = gravityMax / pow(glm::distance(translate[ship], translate[ruber]), 2);
				glm::vec3 gravityVector = getPosition(orientation[ruber]) - getPosition(orientation[ship]);
				gravityVector = glm::normalize(gravityVector);
				translate[model] = getPosition(orientation[model]) + gravityVector * gravityForce;
				orientation[model] = glm::translate(identity, translate[model]) * rotation[model] * glm::scale(identity, glm::vec3(scale[model]));
			}
			// check if warbird hit by missile or crashed; reposition for gameover
			if (warbirdCollision) {
				orientation[model] = rotation[model] * glm::translate(identity, translate[model]) * glm::scale(identity, glm::vec3(0, 0, 0));
			}
		}
		else {
			// PHASE I: Ruber System
			rotation[model] = glm::rotate(rotation[model], modelRadians[model], glm::vec3(0, 1, 0));
			orientation[model] = rotation[model] * glm::translate(identity, translate[model]) * glm::scale(identity, glm::vec3(scale[model]));

			if (model == unumMissileBase) {
				// check if warbird successfully hit Unum; reposition/hide from view
				if (unumBaseCollision) {
					orientation[model] = rotation[model] * glm::translate(identity, translate[model]) * glm::scale(identity, glm::vec3(0, 0, 0));
				}
			}
			if (model == primus || model == secundus || model == secundusMissileBase) {
				//orbital radians refactored from Shape3D and stored in array parallel to models array
				orientation[model] = glm::translate(identity, getPosition(orientation[duo])) * glm::rotate(rotation[model], modelRadians[model], glm::vec3(0, 1, 0)) * glm::translate(identity, translate[model]) * glm::scale(identity, glm::vec3(scale[model]));
				if (model == secundusMissileBase) {
					// check if warbird successfully hit Secundus; reposition/hide from view
					if (secundusBaseCollision) {
						orientation[model] = rotation[model] * glm::translate(identity, translate[model]) * glm::scale(identity, glm::vec3(0, 0, 0));
					}
				}
			}
		}

		//check if warping
		if (warp == true) {
			warp = false;
			warpShip();
		}


		/* MISSILE MOVEMENT */
		// warbird missile
		if (model == warbirdMissile) {
			if (warbirdShootMissile == false) {
				translate[model] = getPosition(orientation[ship]);
				rotation[model] = rotation[ship];
				orientation[model] = glm::translate(identity, translate[model]) * rotation[model] * glm::scale(identity, glm::vec3(0, 0, 0));
			}
			if (warbirdShootMissile == true) {
				// increment timer to keep track of # updates
				warbirdMissileTimer++;
				translate[model] = (translate[model] + getIn(orientation[model]) * (float)((1000 / TQ) * warbirdMissileSpeed));
				orientation[model] = glm::translate(identity, translate[model]) * rotation[model] * glm::scale(identity, glm::vec3(scale[model]));

				// smart tracking initiated after 200 updates
				if (warbirdMissileTimer > 200) {
					missileTracking(warbirdMissile);
				}

				// missile detonates after 2000 updates
				if (warbirdMissileTimer >= 2000) {
					warbirdMissileTimer = 0;
					warbirdShootMissile = false;
					if (warbirdMissileCount == 0) {
						warbirdCanShoot = false;
					}
				}
			}
		}
		// unum missile
		else if (model == unumMissile && !unumBaseCollision && unumMissileCount>0) {

			// translate & rotate missile to match base
			if (unumShootMissile == false || unumBaseCollision) {
				translate[model] = getPosition(orientation[unumMissileBase]);
				rotation[model] = rotation[unumMissileBase];
				orientation[model] = glm::translate(identity, translate[model]) * rotation[model] * glm::scale(identity, glm::vec3(0, 0, 0));
			}

			// check if within detection radius 
			if (distance(getPosition(orientation[unumMissileBase]), getPosition(orientation[ship])) < missileDetectionRadius && unumMissileTimer == 0) {
				unumShootMissile = true;
				unumMissileCount--;
			}

			if (unumShootMissile == true) {
				// increment timer to keep track of # updates
				unumMissileTimer++;
				translate[model] = translate[model] + getIn(orientation[model]) * (float)((1000 / TQ) * baseMissileSpeed);
				orientation[model] = glm::translate(identity, translate[model]) * rotation[model] * glm::scale(identity, scale[model]);
				missileTracking(unumMissile);

				// missile detonates after 2000 updates
				if (unumMissileTimer > 2000) {
					unumShootMissile = false;
					unumMissileTimer = 0;
				}
			}
		}

		// secundus missile
		else if (model == secundusMissile && !secundusBaseCollision && secundusMissileCount>0) { // moon missle updates

			if (secundusShootMissile == false || secundusBaseCollision) { // set missles translation and rotation matrix to that of the base
				translate[model] = getPosition(orientation[secundusMissileBase]);
				rotation[model] = rotation[secundusMissileBase];
				orientation[model] = glm::translate(identity, translate[model]) * rotation[model] * glm::scale(identity, glm::vec3(0, 0, 0));
			}

			// check if within detection radius 
			if (distance(getPosition(orientation[secundusMissileBase]), getPosition(orientation[ship])) < missileDetectionRadius && secundusMissileTimer == 0) {
				secundusShootMissile = true;
				secundusMissileCount--;
			}

			if (secundusShootMissile == true) {
				// increment timer to keep track of # updates
				secundusMissileTimer++;
				translate[model] = translate[model] + getIn(orientation[model]) * (float)((1000 / TQ) * baseMissileSpeed);
				orientation[model] = glm::translate(identity, translate[model]) * rotation[model] * glm::scale(identity, scale[model]);
				missileTracking(secundusMissile);

				// missile detonates after 2000 updates
				if (secundusMissileTimer > 2000) {
					secundusShootMissile = false;
					secundusMissileTimer = 0;
				}

			}
		}
	}

}

// Update models and their animation paths in view, as well as title
void update(int i) {
	glutTimerFunc(TQ, update, 1);

	//animate models
	animateModels();

	// check for collisions
	collisionTesting();

	//stop rotations on key release
	pitch = yaw = roll = accelerate = 0;

	// update dynamic cameras
	viewMatrix = cameraSwap(0);

	// update strings for title
	sprintf(missileStr, "Warbird: %d | ", warbirdMissileCount);
	sprintf(unumBaseStr, "Unum: %d ", unumMissileCount);
	sprintf(secundusBaseStr, " Secundus: %d ", secundusMissileCount);

	glutPostRedisplay();

}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		/* PHASE I */
		// Quit
	case 033: case 'q':  case 'Q': exit(EXIT_SUCCESS); break;

		// Switch cam
		// next cam
	case 'v':
		nextCam = true;
		break;
		// previous cam
	case 'x':
		previousCam = true;
		break;

		/* PHASE II */
		// Change ship speed
	case 's':
		if (speed == 10) {
			speed = 50;
		}
		if (speed == 50) {
			speed = 200;
		}
		else { //speed = 200
			speed = 10;
		}
		break;

		// warp to planet
	case 'w':
		if (!warbirdCollision) {
			warp = true;
		}
		break;

		// Warbird fire missile!
	case 'f':
		if (warbirdShootMissile == false && warbirdCanShoot && !warbirdCollision) {
			warbirdShootMissile = true;
			warbirdMissileCount--;
			if (warbirdMissileCount == 0) {
				warbirdCanShoot = false;
			}
		}
		break;

		// Change Time Quantum
	case 't':
		// ace -> pilot
		if (TQ == 5) {
			TQ = 40;
		}
		// pilot -> trainee
		else if (TQ == 40) {
			TQ = 100;
		}
		// trainee -> debug
		else if (TQ == 100) {
			TQ = 500;
		}
		// full circle: debug -> pilot
		else if (TQ == 500) {
			TQ = 5;
		}
		break;

		// Shift gravity switch
	case 'g':
		if (gravity == 1) {
			gravity = 0;
		}
		else {
			gravity = 1;
		}
		break;

		// PHASE III
	case 'p':
		if (pointLightSetOn) {
			pointLightSetOn = false;
		}
		else {
			pointLightSetOn = true;
		}
		break;
	case 'h':
		if (headLightSetOn) {
			headLightSetOn = false;
		}
		else {
			headLightSetOn = true;
		}
		break;
	case 'd':
		if (debugSetOn) {
			debugSetOn = false;
		}
		else {
			debugSetOn = true;
		}
		break;
	case 'a':
		if (ambientOn) {
			ambientOn = false;
		}
		else {
			ambientOn = true;
		}
		break;
	}
}

void specialKeys(int key, int x, int y) {
	int modifier = glutGetModifiers(); //checks for control key
	if (!warbirdCollision) {
		if (modifier == GLUT_ACTIVE_CTRL) {
			if (key == GLUT_KEY_UP) {
				pitch = 1;
			}
			else if (key == GLUT_KEY_DOWN) {
				pitch = -1;
			}
			if (key == GLUT_KEY_RIGHT) {
				roll = 1;
			}
			else if (key == GLUT_KEY_LEFT) {
				roll = -1;
			}
		}
		else {
			if (key == GLUT_KEY_UP) {
				accelerate = 1;
			}
			else if (key == GLUT_KEY_DOWN) {
				accelerate = -1;
			}
			if (key == GLUT_KEY_RIGHT) {
				yaw = -1;
			}
			else if (key == GLUT_KEY_LEFT) {
				yaw = 1;
			}
		}
	}
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// update model matrix
	glClearColor(0, 0, 0, 0);
	for (int m = 0; m < nShapes; m++) {
		if (m != skybox) {
			modelMatrix = orientation[m];
			modelViewMatrix = viewMatrix * modelMatrix;
			normalMatrix = glm::mat3(modelViewMatrix);
			ModelViewProjectionMatrix = projectionMatrix * modelViewMatrix;


			glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
			glUniformMatrix4fv(MV, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
			glUniformMatrix3fv(NM, 1, GL_FALSE, glm::value_ptr(normalMatrix));
			glUniform3fv(POINTLOCATION, 1, glm::value_ptr(glm::vec3(0, 0, 0) * normalMatrix));
			glUniform3fv(POINTINTENSITY, 1, glm::value_ptr(glm::vec3(.2, .2, .2)));
			glUniform3fv(HEADLOCATION, 1, glm::value_ptr(getPosition(viewMatrix)));
			glUniform3fv(HEADINTENSITY, 1, glm::value_ptr(glm::vec3(.5, .5, .5)));
			glUniform1f(AMBIENTON, ambientOn);
			glUniform1f(HEADON, headLightSetOn);
			glUniform1f(POINTON, pointLightSetOn);
			glUniform1f(DEBUGON, debugSetOn);
			glBindVertexArray(VAO[m]);
			glUniform1f(TEX, false);
			glDrawArrays(GL_TRIANGLES, 0, nVertices[m]);
		}
		else {
			for (int n = 0; n < nTextures; n++) { //rotate texture in to cube position
				if (n < 4) {
					texRotation = rotation[m];
					texRotation = glm::rotate(texRotation, textureRadians * n, glm::vec3(0, 1, 0));
					orientation[m] = texRotation * glm::translate(identity, translate[m]) * glm::scale(identity, glm::vec3(scale[m]));
				}
				else if (n == 4) {
					texRotation = rotation[m];
					texRotation = glm::rotate(texRotation, textureRadians, glm::vec3(0, 0, 1));
					orientation[m] = texRotation * glm::translate(identity, translate[m]) * glm::scale(identity, glm::vec3(scale[m]));
				}
				else { //n = 5
					texRotation = rotation[m];
					texRotation = glm::rotate(texRotation, textureRadians * -1, glm::vec3(0, 0, 1));
					orientation[m] = texRotation * glm::translate(identity, translate[m]) * glm::scale(identity, glm::vec3(scale[m]));
				}


				modelMatrix = orientation[m];
				modelViewMatrix = viewMatrix * modelMatrix;
				normalMatrix = glm::mat3(modelViewMatrix);
				ModelViewProjectionMatrix = projectionMatrix * modelViewMatrix;


				glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
				glUniformMatrix4fv(MV, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
				glUniformMatrix3fv(NM, 1, GL_FALSE, glm::value_ptr(normalMatrix));
				glUniform3fv(POINTLOCATION, 1, glm::value_ptr(glm::vec3(0, 0, 0) * normalMatrix));
				glUniform3fv(POINTINTENSITY, 1, glm::value_ptr(glm::vec3(.2, .2, .2)));
				glUniform3fv(HEADLOCATION, 1, glm::value_ptr(getPosition(viewMatrix)));
				glUniform3fv(HEADINTENSITY, 1, glm::value_ptr(glm::vec3(.5, .5, .5)));
				glUniform1f(AMBIENTON, ambientOn);
				glUniform1f(HEADON, headLightSetOn);
				glUniform1f(POINTON, pointLightSetOn);
				glUniform1f(DEBUGON, debugSetOn);
				glBindVertexArray(VAO[m]);
				glUniform1f(TEX, true);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
				glDrawElements(GL_TRIANGLES, nVertices[m], GL_UNSIGNED_INT, BUFFER_OFFSET(0));
			}
		}
	}
	glutSwapBuffers();
	frameCount++;
	// see if a second has passed to set estimated fps information
	currentTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time
	timeInterval = currentTime - lastTime;
	if (timeInterval >= 1000) {
		sprintf(fpsStr, "FPS: %4d | ", (int)(frameCount / (timeInterval / 1000.0f)));
		lastTime = currentTime;
		frameCount = 0;
		updateTitle();
	}
}

// load the shader programs, vertex data from model files, create the solids, set initial view
void init() {
	// load the shader programs
	shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
	glUseProgram(shaderProgram);

	//load in texture
	GLuint texture = loadRawTexture(textures, textureFile, 1080, 1080);

	// generate VAOs and VBOs
	glGenVertexArrays(nShapes, VAO);
	glGenBuffers(nShapes, buffer);
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// set up the indexed skybox vertex attributes
	glBindVertexArray(VAO[skybox]);

	// initialize a buffer object
	glEnableVertexAttribArray(buffer[skybox]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[skybox]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxPoints) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(skyboxPoints), skyboxPoints);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(skyboxPoints), sizeof(texCoords), texCoords);


	// set up vertex arrays (after shaders are loaded)
	vPosition[skybox] = glGetAttribLocation(shaderProgram, "vPosition");
	glVertexAttribPointer(vPosition[skybox], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition[skybox]);

	vTexCoord = glGetAttribLocation(shaderProgram, "vTexCoord");
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(skyboxPoints)));
	glEnableVertexAttribArray(vTexCoord);

	// load the buffers from the model files

	for (int i = 0; i < nShapes; i++) {
		if (i != skybox) {
			boundingRadius[i] = loadModelBuffer(modelFile[i], nVertices[i], VAO[i], buffer[i], shaderProgram,
				vPosition[i], vColor[i], vNormal[i], "vPosition", "vColor", "vNormal");
		}
		else { //skybox
			boundingRadius[i] = 1;
		}
		// set scale for models given bounding radius  
		scale[i] = glm::vec3(modelSize[i] * 1.0f / boundingRadius[i]);

	}

	viewMatrix = glm::lookAt(
		glm::vec3(0.0f, 20000.0f, 0.0f),  // eye position
		glm::vec3(0),                   // look at position
		glm::vec3(0.0f, 0.0f, -1.0f)); // up vector 

	MVP = glGetUniformLocation(shaderProgram, "ModelViewProjection");
	MV = glGetUniformLocation(shaderProgram, "ModelView");
	NM = glGetUniformLocation(shaderProgram, "NormalMatrix");
	TEX = glGetUniformLocation(shaderProgram, "isTexture");
	POINTLOCATION = glGetUniformLocation(shaderProgram, "PointLightPosition");
	POINTINTENSITY = glGetUniformLocation(shaderProgram, "PointLightIntensity");
	HEADLOCATION = glGetUniformLocation(shaderProgram, "HeadLightPosition");
	HEADINTENSITY = glGetUniformLocation(shaderProgram, "HeadLightIntensity");
	POINTON = glGetUniformLocation(shaderProgram, "PointLightOn");
	HEADON = glGetUniformLocation(shaderProgram, "HeadLightOn");
	AMBIENTON = glGetUniformLocation(shaderProgram, "AmbientOn");
	DEBUGON = glGetUniformLocation(shaderProgram, "DebugOn");


	// set render state values
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
# ifdef __Mac__
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
# endif
# ifndef __Mac__
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
# endif
	glutInitWindowSize(1000, 600);
	//glutInitWindowPosition(600, 600);
	// set OpenGL and GLSL versions to 3.3 for Comp 465/L, comment to see highest versions
# ifndef __Mac__
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
# endif
	glutCreateWindow(baseStr);
	// initialize and verify glew
	glewExperimental = GL_TRUE;  // needed my home system 
	GLenum err = glewInit();
	if (GLEW_OK != err)
		printf("GLEW Error: %s \n", glewGetErrorString(err));
	else {
		printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
		printf("OpenGL %s, GLSL %s\n",
			glGetString(GL_VERSION),
			glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
	// initialize scene
	init();
	// set glut callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	glutTimerFunc(5, update, 1);
	glutMainLoop();
	printf("done\n");
	return 0;
}