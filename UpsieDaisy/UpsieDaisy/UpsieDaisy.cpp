// ----------------------------------------------------------------------------
// Simple sample to prove that Assimp is easy to use with OpenGL.
// It takes a file name as command line parameter, loads it using standard
// settings and displays it.
//
// If you intend to _use_ this code sample in your app, do yourself a favour 
// and replace immediate mode calls with VBOs ...
//
// The vc8 solution links against assimp-release-dll_win32 - be sure to
// have this configuration built.
// ----------------------------------------------------------------------------


// assimp include files. These three are usually needed.
#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"

#include "GL/glut.h"
#include <IL/il.h>

//to map image filenames to textureIds
#include <string.h>
#include <map>

#include <time.h>
#include <vector>
#include <math.h>

//for text
#include <iostream>
#include <sstream>

//for file												//ADDEDDDDDDDDDDDD
#include <fstream>

//for sounds

// ----------------------------------------------------------------------------
// UPSIE DAISY
// 
// Group 11
// 
// Creators: Gravile Andrea, Lupini Gianmario, Parente Chiara
// 
// Special thanks to Andrea Parente (soundtrack)
// 
// Politecnico di Torino - Informatica Grafica (2020/2021)
// 
// Course's main lecturer: Professor F. Lamberti
// Course's assistant lecturer: A. Cannavò
// ----------------------------------------------------------------------------


//assimp include files. These three are usually needed.
#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"

#include "GL/glut.h"
#include <IL/il.h>

//to map image filenames to textureIds
#include <string.h>
#include <map>

//for text
#include <iostream>
#include <sstream>

//for file												
#include <fstream>

//for sounds


#include <time.h>
#include <vector>
#include <math.h>

#define XRES 1600
#define YRES 900
#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)
#define TRUE                1
#define FALSE               0
#define NUMNOTE 5

int SCORE = 0;
int numnote = 4;
int catched[NUMNOTE];
int life = 5;
int game_page = 1; //pagina del menu iniziale 
float timer = 30;

//coordinate della nota che si muove con le frecce
float x_coord = 0.0;
float y_coord = -3.0;
float z_coord = -20.0;

float spin = 0.0;

//counter bonus spawn, coord bonus obj spawn
int count_bonus = 0;

//coordinate dell'oggetto bonus (grammofono)
float bonus_obj_x = 0.0, bonus_obj_y = 0.0, bonus_obj_z = -15.0;

int flag_hit = 0; //flag per verificare di aver colpito l'oggetto bonus 

//coord di FINE caduta note
float boarderX = 0.0, boarderY = -15, boarderZ = -15.0;
float point_zoneY = -6.0;
//coord inizio caduta note
float Ystart = 30;
//velocità di caduta delle note
float note_speed = 0.01;
float initial_note_speed = 0.01;

//coord sull'asse x dei tasti
float do_x = -10.0, re_x = -5.2, mi_x = 0.0, fa_x = 4.7, sol_x = 10.0, la_x = 15.0, si_x = 20.0;
//coord delle idle note
float noteX[NUMNOTE], noteY[NUMNOTE], noteZ = -20;



static const std::string basepath = "./models/";

static const std::string path = "./models/models.obj"; // models path

// the global Assimp scene object
const struct aiScene* scene = NULL;
GLuint scene_list = 0;
struct aiVector3D scene_min, scene_max, scene_center;



// images / texture
std::map<std::string, GLuint*> textureIdMap;	// map image filenames to textureIds
GLuint* textureIds;							// pointer to texture Array

GLfloat LightAmbient[] = { 0.05f, 0.05f, 0.05f, 1.0f }; //rgba
GLfloat LightDiffuse[] = { 2.0f, 2.0f, 2.0f, 1.0f };  //rgba
GLfloat LightPosition[] = { 0.0f, -30.0f, -15.0f, 1.0f }; //xyzw

// ----------------------------------------------------------------------------
void reshape(int width, int height)
{
	const double aspectRatio = (float)XRES / YRES, fieldOfView = 45.0;
	glutReshapeWindow(XRES, YRES);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, aspectRatio, 1.0, 1000.0);  // Znear and Zfar
	glViewport(0, 0, XRES, YRES);

}

// ----------------------------------------------------------------------------
void color4_to_float4(const struct aiColor4D* c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

// ----------------------------------------------------------------------------
void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

// ----------------------------------------------------------------------------
void apply_material(const struct aiMaterial* mtl)
{
	float c[4];

	GLenum fill_mode;
	int ret1, ret2;
	struct aiColor4D diffuse;
	struct aiColor4D specular;
	struct aiColor4D ambient;
	struct aiColor4D emission;
	float shininess, strength;
	int two_sided;
	int wireframe;
	int max;

	int texIndex = 0;
	aiString texPath;	//contains filename of texture
	if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath))
	{
		//bind texture
		unsigned int texId = *textureIdMap[texPath.data];
		glBindTexture(GL_TEXTURE_2D, texId);
	}

	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, (unsigned int*)&max);
	max = 1;
	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, (unsigned int*)&max);
	if ((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}

	max = 1;
	if (AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, (unsigned int*)&max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	max = 1;
	if ((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, (unsigned int*)&max)) && two_sided)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

// ----------------------------------------------------------------------------
void Color4f(const struct aiColor4D* color)
{
	glColor4f(color->r, color->g, color->b, color->a);
}

// ----------------------------------------------------------------------------
void recursive_render(const struct aiScene* sc, const struct aiNode* nd, float scale)
{
	unsigned int i;
	unsigned int n = 0, t;
	struct aiMatrix4x4 m = nd->mTransformation;

	// update transform
	m.Transpose();
	glPushMatrix();
	glMultMatrixf((float*)&m);

	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n)
	{
		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		apply_material(sc->mMaterials[mesh->mMaterialIndex]);


		if (mesh->HasTextureCoords(0))
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);
		if (mesh->mNormals == NULL)
		{
			glDisable(GL_LIGHTING);
		}
		else
		{
			glEnable(GL_LIGHTING);
		}

		if (mesh->mColors[0] != NULL)
		{
			glEnable(GL_COLOR_MATERIAL);
		}
		else
		{
			glDisable(GL_COLOR_MATERIAL);
		}

		for (t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch (face->mNumIndices)
			{
			case 1: face_mode = GL_POINTS; break;
			case 2: face_mode = GL_LINES; break;
			case 3: face_mode = GL_TRIANGLES; break;
			default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for (i = 0; i < face->mNumIndices; i++)		// go through all vertices in face
			{
				int vertexIndex = face->mIndices[i];	// get group index for current index
				if (mesh->mColors[0] != NULL)
					Color4f(&mesh->mColors[0][vertexIndex]);
				if (mesh->mNormals != NULL)

					if (mesh->HasTextureCoords(0))		//HasTextureCoords(texture_coordinates_set)
					{
						glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, 1 - mesh->mTextureCoords[0][vertexIndex].y); //mTextureCoords[channel][vertex]
					}

				glNormal3fv(&mesh->mNormals[vertexIndex].x);
				glVertex3fv(&mesh->mVertices[vertexIndex].x);
			}
			glEnd();
		}
	}

	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n)
	{
		recursive_render(sc, nd->mChildren[n], scale);
	}

	glPopMatrix();
}

// ----------------------------------------------------------------------------
int loadasset(const char* path)
{
	// we are taking one of the postprocessing presets to avoid
	// writing 20 single postprocessing flags here.
	scene = aiImportFile(path, aiProcessPreset_TargetRealtime_Quality);

	if (scene) { return 0; }
	return 1;
}

// ----------------------------------------------------------------------------
int LoadGLTextures(const aiScene* scene)
{
	ILboolean success;

	/* Before calling ilInit() version should be checked. */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		ILint test = ilGetInteger(IL_VERSION_NUM);
		/// wrong DevIL version ///
		std::string err_msg = "Wrong DevIL version. Old devil.dll in system32/SysWow64?";
		char* cErr_msg = (char*)err_msg.c_str();

		return -1;
	}

	ilInit(); /* Initialization of DevIL */

	/* getTexture Filenames and Numb of Textures */
	for (unsigned int m = 0; m < scene->mNumMaterials; m++)
	{
		int texIndex = 0;
		aiReturn texFound = AI_SUCCESS;

		aiString path;	// filename

		while (texFound == AI_SUCCESS)
		{
			texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			textureIdMap[path.data] = NULL; //fill map with textures, pointers still NULL yet
			texIndex++;
		}
	}

	int numTextures = textureIdMap.size();

	/* array with DevIL image IDs */
	ILuint* imageIds = NULL;
	imageIds = new ILuint[numTextures];

	/* generate DevIL Image IDs */
	ilGenImages(numTextures, imageIds); /* Generation of numTextures image names */

	/* create and fill array with GL texture ids */
	textureIds = new GLuint[numTextures];
	glGenTextures(numTextures, textureIds); /* Texture name generation */


	/* get iterator */
	std::map<std::string, GLuint*>::iterator itr = textureIdMap.begin();

	for (int i = 0; i < numTextures; i++)
	{

		//save IL image ID
		std::string filename = (*itr).first;  // get filename
		(*itr).second = &textureIds[i];	  // save texture id for filename in map
		itr++;								  // next texture


		ilBindImage(imageIds[i]); /* Binding of DevIL image name */
		std::string fileloc = basepath + filename;	/* Loading of image */
		success = ilLoadImage((const wchar_t*)fileloc.c_str());

		if (success) /* If no error occured: */
		{
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every colour component into
			unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
			if (!success)
			{
				/* Error occured */
				return -1;
			}
			glBindTexture(GL_TEXTURE_2D, textureIds[i]); /* Binding of texture name */
			//redefine standard texture values
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
			interpolation for magnification filter */
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
			interpolation for minifying filter */
			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
				ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
				ilGetData()); /* Texture specification */
		}
		else
		{
			/* Error occured */
		}
	}
	ilDeleteImages(numTextures, imageIds); /* Because we have already copied image data into texture data
	we can release memory used by image. */

	//Cleanup
	delete[] imageIds;
	imageIds = NULL;

	//return success;
	return TRUE;
}

// ----------------------------------------------------------------------------
int InitGL()					 // All Setup For OpenGL goes here
{
	if (!LoadGLTextures(scene))
	{
		return FALSE;
	}

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);		 // Enables Smooth Shading
	//glClearColor(0.2, 0.7, 1, 0);
	glClearColor(1.0, 1.0, 1.0, 0); //COLORE SFONDO
	glClearDepth(1.0f);				// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // Uses default lighting parameters
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);

	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
	glDisable(GL_BLEND);

	return TRUE;					// Initialization Went OK
}

// ----------------------------------------------------------------------------
//serve per passare alla modalità ortografica invece che prospettica, può essere utile per il menù.
void toOrtho() {
	glMatrixMode(GL_PROJECTION);

	// Save the current projection matrix
	glPushMatrix();
	// Make the current matrix the identity matrix
	glLoadIdentity();
	// Set the projection (to 2D orthographic)
	gluOrtho2D(-10, 10, -10, 10);

	glMatrixMode(GL_MODELVIEW);


}

// ----------------------------------------------------------------------------
void exitOrtho() {

	glMatrixMode(GL_PROJECTION);

	// Restore the original projection matrix
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}


void* font = GLUT_BITMAP_TIMES_ROMAN_24;
void output(int x, int y, std::string str)
{
	int len, i;
	glRasterPos3f(x, y, -20);
	glColor3f(0.0, 0.0, 0.0);
	len = str.length();
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, str[i]);
	}
	glutPostRedisplay();
}

//inizializzazione coordinate note
void init_coord_note() {
	for (int i = 0; i < numnote; i++) {
		if (i == 0) {
			noteY[i] = Ystart;
			int r = rand() % 4;
			if (r == 0) {
				noteX[i] = do_x;
			}
			else if (r == 1) {
				noteX[i] = re_x;
			}
			else if (r == 2) {
				noteX[i] = mi_x;
			}
			else if (r == 3) {
				noteX[i] = fa_x;
			}
		}
		else if (i == 1) {
			int delay = rand() % 10;
			noteY[i] = Ystart + delay + 5;
			int r = rand() % 4;
			if (r == 0) {
				noteX[i] = do_x;
			}
			else if (r == 1) {
				noteX[i] = re_x;
			}
			else if (r == 2) {
				noteX[i] = mi_x;
			}
			else if (r == 3) {
				noteX[i] = fa_x;
			}
		}
		else if (i == 2) {
			int delay = rand() % 5;
			noteY[i] = Ystart + delay + 5;
			int r = rand() % 4;
			if (r == 0) {
				noteX[i] = do_x;
			}
			else if (r == 1) {
				noteX[i] = re_x;
			}
			else if (r == 2) {
				noteX[i] = mi_x;
			}
			else if (r == 3) {
				noteX[i] = fa_x;
			}
		}
		else if (i == 3) {
			int delay = rand() % 15;
			noteY[i] = Ystart + delay + 8;
			int r = rand() % 4;
			if (r == 0) {
				noteX[i] = do_x;
			}
			else if (r == 1) {
				noteX[i] = re_x;
			}
			else if (r == 2) {
				noteX[i] = mi_x;
			}
			else if (r == 3) {
				noteX[i] = fa_x;
			}
		}
		else if (i == 4) {
			int delay = rand() % 20;
			noteY[i] = Ystart + delay + 10;
			int r = rand() % 4;
			if (r == 0) {
				noteX[i] = do_x;
			}
			else if (r == 1) {
				noteX[i] = re_x;
			}
			else if (r == 2) {
				noteX[i] = mi_x;
			}
			else if (r == 3) {
				noteX[i] = fa_x;
			}
		}
	}
	for (int i = 0; i < numnote; i++) {
		for (int j = 0; j < numnote; j++) {
			if (i != j) {
				if (noteX[i] == noteX[j] && (noteY[i] - noteY[j]) < 4 && (noteY[i] - noteY[j]) > -4) {
					noteY[i] += 6;
				}
			}
		}
	}
}

void display_game_over() {

	//display score 
	std::string game_over = "GAME OVER";
	output(0, 0, game_over);
	std::string text_row_3 = " YOUR FINAL SCORE WAS   " + std::to_string(SCORE);
	output(-4, -5, text_row_3);
	
}

void display_life() {

	for (int i = 0; i < life; i++) {

		glPushMatrix();
		glLoadIdentity();
		glScalef(0.015, 0.02, 0.02);
		glTranslatef(50 - i*7, 42, -15);
		glRotatef(spin, 0.0, 1.0, 0.0);
		recursive_render(scene, scene->mRootNode->mChildren[4], 1.0);
		glPopMatrix();
	
	}
}

void spawn_notes() {
	for (int i = 0; i < numnote; i++) {
		if (i == 0) {				//prima nota
			if (noteY[i] < boarderY || catched[i] == 1) {
				if (catched[i] == 0) { life -= 1; }
				noteY[i] = Ystart;
				catched[i] = 0;
				int r = rand() % 4;
				if (r == 0) {
					noteX[i] = do_x;
				}
				else if (r == 1) {
					noteX[i] = re_x;
				}
				else if (r == 2) {
					noteX[i] = mi_x;
				}
				else if (r == 3) {
					noteX[i] = fa_x;
				}
				for (int j = 0; j < numnote; j++) {
					if (i != j) {
						if (noteX[i] == noteX[j] && (noteY[i] - noteY[j]) < 4 && (noteY[i] - noteY[j]) > -4) {
							noteY[i] += 6;
						}
					}
				}
			}
			noteY[i] -= note_speed;
			//idle note  
			glPushMatrix();
			glLoadIdentity();
			glScalef(0.025, 0.04, 0.04);
			glTranslatef(noteX[i], noteY[i], noteZ);
			glRotatef(-spin + 20, 0.0, 1.0, 0.0);
			recursive_render(scene, scene->mRootNode->mChildren[1], 1.0);
			glPopMatrix();
		}
		else if (i == 1) {
			if (noteY[i] < boarderY || catched[i] == 1) {
				if (catched[i] == 0) { life -= 1; }
				int delay = rand() % 5;
				noteY[i] = Ystart + delay + 5; //seconda nota arriva sempre 5+n random tra 0 e 5 dopo
				catched[i] = 0;
				int r = rand() % 4;
				if (r == 0) {
					noteX[i] = do_x;
				}
				else if (r == 1) {
					noteX[i] = re_x;
				}
				else if (r == 2) {
					noteX[i] = mi_x;
				}
				else if (r == 3) {
					noteX[i] = fa_x;
				}
				for (int j = 0; j < numnote; j++) {
					if (i != j) {
						if (noteX[i] == noteX[j] && (noteY[i] - noteY[j]) < 4 && (noteY[i] - noteY[j]) > -4) {
							noteY[i] += 6;
						}
					}
				}
			}
			noteY[i] -= note_speed;
			//idle note  2
			glPushMatrix();
			glLoadIdentity();
			glScalef(0.025, 0.04, 0.04);
			glTranslatef(noteX[i], noteY[i], noteZ);
			glRotatef(spin, 0.0, 1.0, 0.0);
			recursive_render(scene, scene->mRootNode->mChildren[1], 1.0);
			glPopMatrix();
		}
		else if (i == 2) {
			if (noteY[i] < boarderY || catched[i] == 1) {
				if (catched[i] == 0) { life -= 1; }
				int delay = rand() % 10;
				noteY[i] = Ystart + delay + 5; //la terza nota arriva sempre 5+numero random tra 0 e 10 dopo
				catched[i] = 0;
				int r = rand() % 4;
				if (r == 0) {
					noteX[i] = do_x;
				}
				else if (r == 1) {
					noteX[i] = re_x;
				}
				else if (r == 2) {
					noteX[i] = mi_x;
				}
				else if (r == 3) {
					noteX[i] = fa_x;
				}
				for (int j = 0; j < numnote; j++) {
					if (i != j) {
						if (noteX[i] == noteX[j] && (noteY[i] - noteY[j]) < 4 && (noteY[i] - noteY[j]) > -4) {
							noteY[i] += 6;
						}
					}
				}
			}
			noteY[i] -= note_speed;
			//idle note  3
			glPushMatrix();
			glLoadIdentity();
			glScalef(0.025, 0.04, 0.04);
			glTranslatef(noteX[i], noteY[i], noteZ);
			glRotatef(spin + spin / 2, 0.0, 1.0, 0.0);
			recursive_render(scene, scene->mRootNode->mChildren[1], 1.0);
			glPopMatrix();
		}
		else if (i == 3) {
			if (noteY[i] < boarderY || catched[i] == 1) {
				if (catched[i] == 0) { life -= 1; }
				int delay = rand() % 15;
				noteY[i] = Ystart + delay + 8; //la terza nota arriva sempre 5+numero random tra 0 e 10 dopo
				catched[i] = 0;
				int r = rand() % 4;
				if (r == 0) {
					noteX[i] = do_x;
				}
				else if (r == 1) {
					noteX[i] = re_x;
				}
				else if (r == 2) {
					noteX[i] = mi_x;
				}
				else if (r == 3) {
					noteX[i] = fa_x;
				}
				for (int j = 0; j < numnote; j++) {
					if (i != j) {
						if (noteX[i] == noteX[j] && (noteY[i] - noteY[j]) < 4 && (noteY[i] - noteY[j]) > -4) {
							noteY[i] += 6;
						}
					}
				}
			}
			noteY[i] -= note_speed;
			//idle note  4
			glPushMatrix();
			glLoadIdentity();
			glScalef(0.025, 0.04, 0.04);
			glTranslatef(noteX[i], noteY[i], noteZ);
			glRotatef(spin + spin / 2, 0.0, 1.0, 0.0);
			recursive_render(scene, scene->mRootNode->mChildren[1], 1.0);
			glPopMatrix();
		}
		else if (i == 4) {
			if (noteY[i] < boarderY || catched[i] == 1) {
				if (catched[i] == 0) { life -= 1; }
				int delay = rand() % 20;
				noteY[i] = Ystart + delay + 10; //la terza nota arriva sempre 5+numero random tra 0 e 10 dopo
				catched[i] = 0;
				int r = rand() % 4;
				if (r == 0) {
					noteX[i] = do_x;
				}
				else if (r == 1) {
					noteX[i] = re_x;
				}
				else if (r == 2) {
					noteX[i] = mi_x;
				}
				else if (r == 3) {
					noteX[i] = fa_x;
				}
				for (int j = 0; j < numnote; j++) {
					if (i != j) {
						if (noteX[i] == noteX[j] && (noteY[i] - noteY[j]) < 4 && (noteY[i] - noteY[j]) > -4) {
							noteY[i] += 6;
						}
					}
				}
			}
			noteY[i] -= note_speed;
			//idle note  5
			glPushMatrix();
			glLoadIdentity();
			glScalef(0.025, 0.04, 0.04);
			glTranslatef(noteX[i], noteY[i], noteZ);
			glRotatef(spin + spin / 2, 0.0, 1.0, 0.0);
			recursive_render(scene, scene->mRootNode->mChildren[1], 1.0);
			glPopMatrix();
		}


	}
}

void spawn_bonus() {

	if (count_bonus > 4) {

		timer -= initial_note_speed;

		if (timer < 0 || flag_hit == 1) {

			if (timer > 0 && flag_hit == 1) { SCORE += 10; }
			flag_hit = 0;

			//calcolo nuove coordinate
			int r = rand() % 4;

			if (r == 0) {
				bonus_obj_x = rand() % 10;
				bonus_obj_y = rand() % 10;
			}
			else if (r == 1) {
				bonus_obj_x = -rand() % 10;
				bonus_obj_y = rand() % 10;
			}
			else if (r == 2) {
				bonus_obj_x = rand() % 10;
				bonus_obj_y = -rand() % 10;
			}
			else if (r == 3) {
				bonus_obj_x = -rand() % 10;
				bonus_obj_y = -rand() % 10;
			}

			count_bonus = 0;
			timer = 70;
			glutPostRedisplay();
		}
		else {
			//spawn oggetto bonus
			glPushMatrix();
			glLoadIdentity();
			glScalef(0.025, 0.04, 0.04);
			glTranslatef(bonus_obj_x, bonus_obj_y, -20);
			glRotatef(spin, 0.0, 1.0, 0.0);
			recursive_render(scene, scene->mRootNode->mChildren[2], 1.0);
			glPopMatrix();

		}
	}
}

void display_game() {
	//display score 
	std::string text_row_1 = "SCORE:" + std::to_string(SCORE);
	output(8, 5.5, text_row_1);

	display_life();

	//piano
	glPushMatrix();
	glLoadIdentity();
	glScalef(0.07, 0.07, 0.15);
	glTranslatef(-1.0, 0.0, 0.0);
	glRotatef(-70, 1.0, 0.0, 0.0);
	recursive_render(scene, scene->mRootNode->mChildren[0], 1.0);
	glPopMatrix();

	//note
	glPushMatrix();
	glLoadIdentity();
	glScalef(0.025, 0.04, 0.04);
	glTranslatef(x_coord, y_coord, z_coord);
	glRotatef(spin, 0.0, 1.0, 0.0);
	recursive_render(scene, scene->mRootNode->mChildren[1], 1.0);
	glPopMatrix();


	//gramophone
	/*glPushMatrix();
	glLoadIdentity();
	glScalef(0.04, 0.04, 0.04);
	glTranslatef(-10.0, 10.0, -20.0);
	glRotatef(-70, 1.0, 0.0, 0.0);
	glRotatef(-spin * 0.5, 0.0, 1.0, 0.0);
	recursive_render(scene, scene->mRootNode->mChildren[2], 1.0);
	glPopMatrix();*/



	spawn_bonus();

	//note che scendono
	spawn_notes();


}

void display_menu() {

	glPushMatrix();
	glLoadIdentity();
	glScalef(0.05, 0.085, 1.0);
	glTranslatef(-1.0, 0.0, 0.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	recursive_render(scene, scene->mRootNode->mChildren[3], 1.0);
	glPopMatrix();


}

void display_settings() {

	glPushMatrix();
	glLoadIdentity();
	glScalef(0.05, 0.085, 1.0);
	glTranslatef(-1.0, 0.0, 0.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	recursive_render(scene, scene->mRootNode->mChildren[5], 1.0);
	glPopMatrix();


}
// ----------------------------------------------------------------------------
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (game_page == 0) {
		//verifico di possedere ancora della vita
		if (life == 0) { 
			game_page = 2;
			glutPostRedisplay();
		}
		else {
			display_game();
		}
		
	}
	else if (game_page == 1) {
		display_menu();
	}

	else if (game_page == 2) {
		display_game_over();
	}
	else if (game_page == 3) {
		display_settings();
	}

	glutSwapBuffers();
}

// ----------------------------------------------------------------------------
void idle(int) {
	spin += 2;
	glutPostRedisplay();
	glutTimerFunc(1000 / 120, idle, 0);							//120 FPS
}



// ---------------------------------------------------------------------------
// Check the mouse position and manages the color change of the Menu keys
void pointer(int x, int y)
{

}

void start_game() {
	game_page = 0;
	init_coord_note();
	glutPostRedisplay();
}


// ----------------------------------------------------------------------------
// Check where mouse points when clicked
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (game_page == 1) {				//MENU PAGE
			if (x > 577 && x < 939 && y > 433 && y < 650) {
				start_game();
			}
			else if (x > 1080 && x < 1350 && y > 508 && y < 630) {
				game_page = 3; //open settings
				glutPostRedisplay();
			}
		}
		else if (game_page == 3) {				//SETTING PAGE oko
			if (x > 219 && x < 366 && y > 643 && y < 846) {
				game_page = 1; //back to menu
				glutPostRedisplay();
			}
			//easy
			else if (x > 217 && x < 442 && y > 495 && y < 640) {
				//set the easy mode (3 notes)
				numnote = 3;
				game_page = 1; //back to menu
				glutPostRedisplay();
			}
			
			//medium
			else if (x > 617 && x < 911 && y > 479 && y < 660) {
				//set the medium mode (4 notes)
				numnote = 4;
				game_page = 1; //back to menu
				glutPostRedisplay();
			}
			else if (x > 1080 && x < 1350 && y > 508 && y < 630) {
				//set the hard mode (5 notes)
				numnote = 5;
				game_page = 1; //open settings
				glutPostRedisplay();
			}
			
		}
		std::string text_row_1 = "x: " + std::to_string(x) + ", invece y: " + std::to_string(y);
		printf("%s\n", text_row_1.c_str());
	}
}

// ----------------------------------------------------------------------------
// Manages arrow keys

void special_keyboard(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		x_coord -= 0.3;
		//controllo sulla posizione coincidente 
		if (((x_coord - bonus_obj_x) > -2 && (x_coord - bonus_obj_x) < 2) && ((y_coord - bonus_obj_y) > -2 && (y_coord - bonus_obj_y) < 2)) {
			flag_hit = 1;
		}
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_RIGHT) {
		x_coord += 0.3;

		if (((x_coord - bonus_obj_x) > -2 && (x_coord - bonus_obj_x) < 2) && ((y_coord - bonus_obj_y) > -2 && (y_coord - bonus_obj_y) < 2)) {
			flag_hit = 1;
		}
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_UP) {
		y_coord += 0.3;

		if (((x_coord - bonus_obj_x) > -2 && (x_coord - bonus_obj_x) < 2) && ((y_coord - bonus_obj_y) > -2 && (y_coord - bonus_obj_y) < 2)) {
			flag_hit = 1;
		}
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_DOWN) {
		y_coord -= 0.3;

		if (((x_coord - bonus_obj_x) > -2 && (x_coord - bonus_obj_x) < 2) && ((y_coord - bonus_obj_y) > -2 && (y_coord - bonus_obj_y) < 2)) {
			flag_hit = 1;
		}
		glutPostRedisplay();
	}

}

// ----------------------------------------------------------------------------
// Manages the keyboard
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) {
		exit(1);
	}

	if (game_page == 1) {
		switch (key) {
			//nella schermata principale se premo "enter" inizio a giocare
		case 13:
			break;
		}
	}
	else if (game_page == 0) {
		switch (key) {
		case 27:
			exit(1);
			break;

		case 'a':
			//do
			for (int i = 0; i < numnote; i++) {
				if ((noteY[i] < point_zoneY + 1) && (noteY[i] > point_zoneY - 1)) {
					if (noteX[i] == do_x) {
						SCORE += 1;
						note_speed += 0.001; //se ho preso la nota, aumento leggermente la velocità
						count_bonus += 1; //incremento contatore per far spawnare oggetto bonus
						catched[i] = 1;
					}
				}
			}
			break;


		case 's':
			//re
			for (int i = 0; i < numnote; i++) {
				if ((noteY[i] < point_zoneY + 1) && (noteY[i] > point_zoneY - 1)) {
					if (noteX[i] == re_x) {
						SCORE += 1;
						note_speed += 0.001; //se ho preso la nota, aumento leggermente la velocità
						count_bonus += 1; //incremento contatore per far spawnare oggetto bonus
						catched[i] = 1;

					}
				}
			}
			break;


		case 'd':
			//mi
			for (int i = 0; i < numnote; i++) {
				if ((noteY[i] < point_zoneY + 1) && (noteY[i] > point_zoneY - 1)) {
					if (noteX[i] == mi_x) {
						SCORE += 1;
						note_speed += 0.001; //se ho preso la nota, aumento leggermente la velocità
						count_bonus += 1; //incremento contatore per far spawnare oggetto bonus
						catched[i] = 1;
					}
				}
			}
			break;

		case 'f':
			//fa
			for (int i = 0; i < numnote; i++) {
				if ((noteY[i] < point_zoneY + 1) && (noteY[i] > point_zoneY - 1)) {
					if (noteX[i] == fa_x) {
						SCORE += 1;
						note_speed += 0.001; //se ho preso la nota, aumento leggermente la velocità
						count_bonus += 1; //incremento contatore per far spawnare oggetto bonus
						catched[i] = 1;
					}
				}
			}
			break;


		case 'g':
			//sol
			break;



		case 'h':

		case 'j':
			//si
			break;

			//metto in pausa e torno nella schermata principale
		case 'p':
			game_page = 1;
			glutPostRedisplay();
			break;
		}
	}
	else if (game_page == 2) {
		switch (key) {
		case 13:
			SCORE = 0;
			count_bonus = 0;
			life = 5;
			note_speed = initial_note_speed;
			init_coord_note();
			game_page = 0;
			glutPostRedisplay();
			break;
		}
	}
}

// ----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	struct aiLogStream stream;

	glutInitWindowSize(XRES, YRES);
	glutInitWindowPosition(700, 150);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInit(&argc, argv);


	glutCreateWindow("Piano Hero");
	glutFullScreen();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(pointer);
	glutReshapeFunc(reshape);


	srand(time(NULL));
	idle(0);

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "assimp_log.txt");
	aiAttachLogStream(&stream);

	loadasset(path.c_str());

	if (!InitGL())
	{
		fprintf(stderr, "Initialization failed");
		return FALSE; 
	}

	glutGet(GLUT_ELAPSED_TIME);

	glutMainLoop();

	aiReleaseImport(scene);

	aiDetachAllLogStreams();

	return 0;
}
