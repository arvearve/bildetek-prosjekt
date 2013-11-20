#include<ctime>
#include <vector>

//-------- Variables -------------------------------
extern int g_iWindowWidth;
extern int g_iWindowHeight;
extern int g_iGLUTWindowHandle;
extern int g_iErrorCode ;

extern unsigned char g_eCurrentScene;
extern  float rot_angle;
typedef struct
{
	double R;
	double G;
	double B;
} Color;

typedef struct
{
	float x;
	float y;
} Coordinate;


typedef struct
{
	Color color;
	std::vector<Coordinate> items;
} Group;

//-------- Functions --------------------------------
// Cleanup : This is the exist function.
void Cleanup( int exitCode, bool bExit = true );

// SetupGL :  Set up the OpenGL state machine and create a light source
void SetupGL();

// LoadResources : Create and upload resources to graphics card
void LoadResources();

void RenderGroup(std::vector<Coordinate> coordinates, Color color, int modelNum);
void RenderGroupCount(int count, int offset);
// DisplayGL :  The function responsible for drawing everything in the
// OpenGL context associated to a window.
void DisplayGL();

// KeyboardGL :  This functions is called whenever the keyboard is used
// inside the opengl window
void KeyboardGL( unsigned char c, int x, int y );

// MouseGL :  This functions is called whenever the mouse is used
// inside the opengl window
void mosue_moved(int x, int y );

void resetPointer();
// Handle the window size changes and define the world coordinate
// system and projection type
void ReshapeGL( int w, int h );

//Function called when screen is idle.
void Idle();

// Functions called to draw different scenes
void RenderScene();