//	Refer to Direct3D 9 documentation for the meaning of the members.
//https://learn.microsoft.com/en-us/windows/win32/direct3d9/dx9-graphics
//https://learn.microsoft.com/en-us/windows/win32/api/_direct3d9/

#include <d3d9.h>
#include <d3dx9.h>
#include <Windows.h>
#include <iostream>
#include <dinput.h>
#include <string>
#include <D3dx9math.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "FrameTimer.h"
#include "AudioManager.h"
#define WIN32_LEAN_AND_MEAN

using namespace std;
enum powerUp { hpPowerUp, bulletPowerUp, timePowerUp };
enum UIController { MainMenu, GameMenu, GameOverMenu, SpaceshipSelectionMenu, CrosshairSelectionMenu };
enum phaseList {FirstPhase, SecondPhase, ThirdPhase};
enum asteroidList {smallAsteroid, mediumAsteroid, largeAsteroid};
enum gameOver {Retry, Exit};

//Window Structure
struct {
	WNDCLASS wndClass;
	WNDCLASS splashWndClass;
	HWND g_hWnd = NULL;
	HWND g_hWnd2 = NULL;
	MSG msg;
} wndStruct;

//Direct3D Structure
struct {
	D3DPRESENT_PARAMETERS d3dPP;
	IDirect3D9* direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	IDirect3DDevice9* d3dDevice;

	D3DPRESENT_PARAMETERS splashD3dPP;
	IDirect3D9* splashDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	IDirect3DDevice9* splashD3dDevice;
} directStruct;

//SpriteSheet Class
class SpriteSheet
{
private:
	float totalSpriteWidth = NULL;
	float totalSpriteHeight = NULL;
	float spriteWidth = NULL;
	float spriteHeight = NULL;
	int spriteRow = NULL;
	int spriteCol = NULL;
	int currentFrame = NULL;
	int maxFrame = NULL;
	RECT spriteRect;

public:
	SpriteSheet(float totalSpriteWidth, float totalSpriteHeight, int spriteRow, int spriteCol, int currentFrame, int maxFrame)
	{
		this->totalSpriteWidth = totalSpriteWidth;
		this->totalSpriteHeight = totalSpriteHeight;
		this->spriteRow = spriteRow;
		this->spriteCol = spriteCol;
		this->currentFrame = currentFrame - 1;
		this->maxFrame = maxFrame;
		this->spriteWidth = totalSpriteWidth / spriteCol;
		this->spriteHeight = totalSpriteHeight / spriteRow;
		spriteRect.top = 0;
		spriteRect.right = 0;
		spriteRect.bottom = 0;
		spriteRect.left = 0;
	}

	SpriteSheet(float totalSpriteWidth, float totalSpriteHeight) {
		this->totalSpriteWidth = totalSpriteWidth;
		this->totalSpriteHeight = totalSpriteHeight;
		spriteRect.top = 0;
		spriteRect.right = 0;
		spriteRect.bottom = 0;
		spriteRect.left = 0;
	}

	float getTotalSpriteWidth() {
		return totalSpriteWidth;
	}
	float getTotalSpriteHeight() {
		return totalSpriteHeight;
	}
	int getSpriteRow() {
		return spriteRow;
	}
	int getSpriteCol() {
		return spriteCol;
	}
	int getCurrentFrame() {
		return currentFrame;
	}
	int getMaxFrame() {
		return maxFrame;
	}
	float getSpriteWidth() {
		return spriteWidth;
	}
	float getSpriteHeight() {
		return spriteHeight;
	}
	RECT& getRect() {
		return spriteRect;
	}
	void setCurrentFrame(int currentFrame) {
		this->currentFrame = currentFrame - 1;
	}
	void staticFrame() {
		currentFrame = 0;
	}
	void leftFrame() {
		currentFrame = 2;
	}
	void rightFrame() {
		currentFrame = 4;
	}
	void nextThrustFrame() {
		currentFrame++;
		if (currentFrame > maxFrame - 1) {
			currentFrame = 0;
		}
	}
	void prevFrame() {
		currentFrame--;
		if (currentFrame < 0) {
			currentFrame = maxFrame - 1;
		}
	}
	RECT& crop() {
		if (spriteRow == NULL || spriteCol == NULL) {
			spriteRect.left = 0;
			spriteRect.right = totalSpriteWidth;
			spriteRect.top = 0;
			spriteRect.bottom = totalSpriteHeight;
			return spriteRect;
		}
		spriteRect.left = (currentFrame % spriteCol) * spriteWidth;
		spriteRect.right = spriteRect.left + spriteWidth;
		spriteRect.top = (currentFrame / spriteCol) * spriteHeight;
		spriteRect.bottom = spriteRect.top + spriteHeight;
		return spriteRect;
	}
};

//Sprite Transformation Class
class SpriteTransform {
private:
	D3DXMATRIX mat;
	D3DXVECTOR2 scalingCenter;
	float scalingRotation;
	D3DXVECTOR2 scaling;
	D3DXVECTOR2 rotationCenter;
	float rotation;
	D3DXVECTOR2 trans;
	int powerUpChosen;
	int asteroidHp;
	int asteroidChosen;
	D3DXVECTOR2 asteroidVelocity = D3DXVECTOR2 (0,0);
public:
	SpriteTransform(D3DXVECTOR2 scalingCenter, float scalingRotation, D3DXVECTOR2 scaling, D3DXVECTOR2 rotationCenter, float rotation, D3DXVECTOR2 trans) {
		this->scalingCenter = scalingCenter;
		this->scalingRotation = scalingRotation;
		this->scaling = scaling;
		this->rotationCenter = rotationCenter;
		this->rotation = rotation;
		this->trans = trans;
	}
	SpriteTransform(D3DXVECTOR2 scalingCenter, float scalingRotation, D3DXVECTOR2 scaling, D3DXVECTOR2 rotationCenter, float rotation, D3DXVECTOR2 trans, int powerUpChosen) {
		this->scalingCenter = scalingCenter;
		this->scalingRotation = scalingRotation;
		this->scaling = scaling;
		this->rotationCenter = rotationCenter;
		this->rotation = rotation;
		this->trans = trans;
		this->powerUpChosen = powerUpChosen;
	}
	SpriteTransform(D3DXVECTOR2 scalingCenter, float scalingRotation, D3DXVECTOR2 scaling, D3DXVECTOR2 rotationCenter, float rotation, D3DXVECTOR2 trans, int asteroidHp, int asteroidChosen) {
		this->scalingCenter = scalingCenter;
		this->scalingRotation = scalingRotation;
		this->scaling = scaling;
		this->rotationCenter = rotationCenter;
		this->rotation = rotation;
		this->trans = trans;
		this->asteroidHp = asteroidHp;
		this->asteroidChosen = asteroidChosen;
	}
	SpriteTransform() {

	}
	
	int getAsteroidHp() {
		return asteroidHp;
	}
	int getAsteroidChosen() {
		return asteroidChosen;
	}
	D3DXMATRIX& getMat() {
		return mat;
	}
	D3DXVECTOR2& getScalingCenter() {
		return scalingCenter;
	}
	float getScalingRotation() {
		return scalingRotation;
	}
	D3DXVECTOR2& getScaling() {
		return scaling;
	}
	D3DXVECTOR2& getRotationCenter() {
		return rotationCenter;
	}
	float getRotation() {
		return rotation;
	}
	D3DXVECTOR2& getTrans() {
		return trans;
	}
	int getPowerUpChosen() {
		return powerUpChosen;
	}
	D3DXVECTOR2& getAsteroidVelocity() {
		return asteroidVelocity;
	}
	void setAsteroidHp(int asteroidHp) {
		this->asteroidHp = asteroidHp;
	}
	void setAsteroidChosen(int asteroidChosen) {
		this->asteroidChosen = asteroidChosen;
	}
	void setMat(D3DXMATRIX mat) {
		this->mat = mat;
	}
	void setScalingCenter(D3DXVECTOR2 scalingCenter) {
		this->scalingCenter = scalingCenter;
	}
	void setScalingRotation(float scalingRotation) {
		this->scalingRotation = scalingRotation;
	}
	void setScaling(D3DXVECTOR2 scaling) {
		this->scaling = scaling;
	}
	void setRotationCenter(D3DXVECTOR2 rotationCenter) {
		this->rotationCenter = rotationCenter;
	}
	void setRotation(float rotation) {
		this->rotation = rotation;
	}
	void setTrans(D3DXVECTOR2 trans) {
		this->trans = trans;
	}
	void setAsteroidVelocity(D3DXVECTOR2 asteroidVelocity){
		this->asteroidVelocity = asteroidVelocity;
	}
	void transform() {
		D3DXMatrixTransformation2D(&mat, &scalingCenter, scalingRotation, &scaling, &rotationCenter, rotation, &trans);
	}
	void reduceAsteroidHp() {
		asteroidHp--;
	}
};

//Texture Class
class Texture {
private:
	LPDIRECT3DTEXTURE9 texture = NULL;
	LPCTSTR fileLocation = "";
	int redKey = 0, greenKey = 0, blueKey = 0;
public:
	Texture(LPCTSTR fileLocation, int redKey, int greenKey, int blueKey) {
		this->fileLocation = fileLocation;
		this->redKey = redKey;
		this->greenKey = greenKey;
		this->blueKey = blueKey;
	}
	Texture(LPCTSTR fileLocation) {
		this->fileLocation = fileLocation;
	}
	HRESULT createTextureFromFile() {
		return D3DXCreateTextureFromFile(directStruct.d3dDevice, fileLocation, &texture);
	}
	HRESULT createSplashTextureFromFile() {
		return D3DXCreateTextureFromFile(directStruct.splashD3dDevice, fileLocation, &texture);
	}

	HRESULT createTextureFromFileEx() {
		return D3DXCreateTextureFromFileEx(directStruct.d3dDevice, fileLocation, D3DX_DEFAULT, D3DX_DEFAULT,
			D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
			D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_XRGB(redKey, greenKey, blueKey),
			NULL, NULL, &texture);
	}

	LPDIRECT3DTEXTURE9 getTexture() {
		return texture;
	}
	void setTexture(LPDIRECT3DTEXTURE9 texture) {
		this->texture = texture;
	}
	void releaseTexture() {
		texture->Release();
		texture = NULL;
	}
};

//Textures
Texture pointerTexture(NULL);
Texture crosshairTexture("Assets/crosshair.png");
Texture crosshair2Texture("Assets/crosshair2.png");
Texture crosshair3Texture("Assets/crosshair3.png");
Texture spaceshipTexture("Assets/ship.png");
Texture spaceship2Texture("Assets/ship2.png");
Texture currentSpaceshipTexture(NULL);
Texture thrustTexture("Assets/thrust.png");
Texture turretTexture("Assets/turret.png");
Texture asteroidTexture("Assets/asteroid.png");
Texture bulletTexture("Assets/bullet.png");
Texture hpPowerUpTexture("Assets/powerup1.png");
Texture bulletPowerUpTexture("Assets/powerup2.png");
Texture timePowerUpTexture("Assets/powerup3.png");
Texture powerUpTexture(NULL);
Texture splashTexture("Assets/splash.jpg");
Texture cursorTexture("Assets/cursor.png");
Texture buttonBgTexture("Assets/buttonBg.png");
Texture bgTexture("Assets/bg.png");

//Sprite Sheet
//Sprite Sheet Object - (totalWidth, totalHeight, row, col, currentFrame, maxFrame)
//                    - (totalWidth, totalHeight)
//                    - (spriteRectLeft, spriteRectRight, spriteRectTop, spriteRectBottom)
SpriteSheet pointerSprite(30, 30);
SpriteSheet cursorSprite(30, 30);
SpriteSheet buttonBgSprite(250, 120);
SpriteSheet spaceshipSprite(250, 50, 1, 5, 1, 5);
SpriteSheet thrustSprite(32, 20, 1, 2, 1, 2);
SpriteSheet turretSprite(1024, 128, 1, 8, 1, 8);
SpriteSheet asteroidSprite(60, 60);
SpriteSheet bulletSprite(16, 28);
SpriteSheet hpPowerUpSprite(35, 35);
SpriteSheet bulletPowerUpSprite(35, 35);
SpriteSheet timePowerUpSprite(35, 35);

//Sprite Transformation
SpriteTransform pointerTrans;
SpriteTransform cursorTrans;
SpriteTransform spaceshipTrans;
SpriteTransform thrustTrans;
SpriteTransform turretTrans;
//lazy to dynamically increase the size
SpriteTransform bulletTrans[200];
SpriteTransform asteroidTrans[200];
SpriteTransform powerUpTrans[30];
SpriteTransform buttonBgTrans;
SpriteTransform textTrans;
SpriteTransform timerTextTrans;
SpriteTransform livesTextTrans;
SpriteTransform helpTextTrans;
SpriteTransform scoresTextTrans;
SpriteTransform highScoresTextTrans;
SpriteTransform bgTrans;
SpriteTransform spaceship2Trans;
SpriteTransform spaceshipSelectionTrans;
SpriteTransform spaceship2SelectionTrans;
SpriteTransform spaceshipSelectionTextTrans;
SpriteTransform spaceship2SelectionTextTrans;
SpriteTransform crosshairTrans;
SpriteTransform crosshair2Trans;
SpriteTransform crosshair3Trans;
SpriteTransform crosshairSelectionTrans;
SpriteTransform crosshair2SelectionTrans;
SpriteTransform crosshair3SelectionTrans;
SpriteTransform crosshairSelectionTextTrans;
SpriteTransform crosshair2SelectionTextTrans;
SpriteTransform crosshair3SelectionTextTrans;
SpriteTransform titleTextTrans;
SpriteTransform continueButtonTrans;
SpriteTransform exitButtonTrans;
SpriteTransform continueTextTrans;
SpriteTransform exitTextTrans;
SpriteTransform helpText2Trans;

//Default value for rgb color
int red = 0;
int green = 0;
int blue = 0;

//pointer to a sprite interface 
LPD3DXSPRITE sprite = NULL;

//pointer to font interface
LPD3DXFONT font = NULL;
//Rect for text
RECT textRect;
RECT timerTextRect;
RECT scoresTextRect;
RECT livesTextRect;

//Screen Resolution
int screenWidth = 1280;
int screenHeight = 720;

//Input 
LPDIRECTINPUT8 dInput;
LPDIRECTINPUTDEVICE8  dInputKeyboardDevice;
LPDIRECTINPUTDEVICE8 dInputMouseDevice;
DIMOUSESTATE mouseState;

//Default position for mouse
LONG currentXpos = 500;
LONG currentYpos = 500;
//Variable to show mouse position text
string strCurrentXpos;
string strCurrentYpos;
char posText[9];

//Timer Stuff
int waveSec;
int waveMin;
//Variable to show timer text
string strWaveSec;
string strWaveMin;
char timerText[15];

//Spaceship Live
int lives = 3;
//Variable to show lives text
string strLivesPrefix = "Lives: ";
string strLivesPostfix;
char livesText[15];

//Scores
int scores = 0;
int highScores = 0;
//Variable to show score text
string strScoresPrefix = "Scores: ";
string strScoresPostfix;
char scoresText[15];
string strHighScoresPrefix = "High Scores: ";
string strHighScoresPostfix;
char highScoresText[15];

//	Key input buffer
BYTE  diKeys[256];

// FrameTimer Object
FrameTimer* gameTimer = new FrameTimer();
FrameTimer* bulletTimer = new FrameTimer();
FrameTimer* thrustTimer = new FrameTimer();
FrameTimer* asteroidTimer = new FrameTimer();
FrameTimer* waveTimer = new FrameTimer();
FrameTimer* splashTimer = new FrameTimer();
FrameTimer* transitionTimer = new FrameTimer();
// Audio Object
AudioManager* myAudioManager = new AudioManager();

//PI
float static PI = 3.142;

//Friction
float friction = 0.01; //1 = no friction
float asteroidFriction = 0.3;

//Spaceship position
D3DXVECTOR2 spaceshipPosition(600, 600);
//Spaceship rotation
float spaceshipRotation = 0;
//Spaceship Physics
D3DXVECTOR2 spaceshipVelocity;
D3DXVECTOR2 spaceshipAcceleration;
D3DXVECTOR2 spaceshipEngineForce;
float spaceshipEnginePower = 30;
float spaceshipMass = 100;

// Turret
D3DXVECTOR2 turretPosition;
float turretRotation;
float pointerCenterX;
float pointerCenterY;
float turretCenterX;
float turretCenterY;

// Bullet
boolean toggleShoot = false;
D3DXVECTOR2 bulletStartPosition;
D3DXVECTOR2 bulletPos;
int defaultBulletInterval = 5;
int bulletInterval = defaultBulletInterval;
int bulletEntry = 0;
D3DXVECTOR2 bulletVelocity;
float bulletPower = 10;
float bulletMass = 100;

// Asteroid
int asteroidEntry = 0;
D3DXVECTOR2 asteroidVelocity;
float asteroidMass;
float asteroidPower;
float asteroidRotation;
float asteroidRotationRate = 0.05;
float asteroidStartRotation;
int smallAsteroidHp = 1;
int mediumAsteroidHp = 3;
int largeAsteroidHp = 5;
float smallAsteroidMass = 10;
float mediumAsteroidMass = 20;
float largeAsteroidMass = 50;
float smallAsteroidPower = 10;
float mediumAsteroidPower = 8;
float largeAsteroidPower = 5;
D3DXVECTOR2 asteroidStartPosition(500, -100);
D3DXVECTOR2 asteroidPosition;
D3DXVECTOR2 asteroidScaling;
int chosenAsteroid;
int chosenAsteroidHp;

//Power Up
boolean timeStop;
int timeStopDuration = 10;
int timeStopDurationLeft = timeStopDuration;
boolean bulletPowerUpPicked;
int bulletPowerUpDuration = 10;
int bulletPowerUpDurationLeft = bulletPowerUpDuration;
int bulletPowerUpSpeed = 15;
int powerUpEntry = 0;
D3DXVECTOR2 powerUpPosition;
int powerUpSpawnRate = 5;
int powerUpSpawnRateLeft = powerUpSpawnRate;
int powerUpChosen;

//Splash Screen
int splashScreenWidth = 500;
int splashScreenHeight = 500;
int splashCount;
int maxSplashCount = 5;

//UI Controller
int currentMenu = MainMenu;

//Transition
float currentTransitionPos = 2000;
float beforeTransitionPos = 80;
float afterTransitionPos = 80;
boolean afterTransition = false;

//Game Over Text
string strSec = " seconds ";
string strMin = " minutes ";
string strSurvived = "You survived for ";
int totalTextLength;
char survivedTimerText[50];

//Game Over Button Action
int gameOverAction;

// Physics calculation
float massSum;
D3DXVECTOR2 diffVelocity;
D3DXVECTOR2 diffPos;
float dotProduct;
float magnitude;
float diffPosX;
float diffPosY;
bool collisionDetected;

//Waves
int currentPhase = FirstPhase;
int secondPhaseTimer = 10;  //in seconds
int thirdPhaseTimer = 30;   //in seconds

void render();
void createDirectInput();
void resetStage() {
	cout << "Stage Resetted Successfully" << endl;
	lives = 3;
	waveSec = 0;
	waveMin = 0;
	asteroidEntry = 0;
	bulletEntry = 0;
	powerUpEntry = 0;
	scores = 0;
	toggleShoot = false;
	currentPhase = FirstPhase;
	myAudioManager->channel->setPaused(false);
	myAudioManager->channel9->setPaused(true);
	spaceshipSprite.setCurrentFrame(1);
	spaceshipPosition = D3DXVECTOR2(600, 600);
	spaceshipVelocity = D3DXVECTOR2(0, 0);
	timeStopDurationLeft = 0;
	bulletPowerUpDurationLeft = 0;
	powerUpSpawnRateLeft = powerUpSpawnRate;
}

LRESULT CALLBACK SplashWindowProcedure(HWND hWnd2, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	default:
		return DefWindowProc(hWnd2, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x58: //X key
			if (!toggleShoot) {
				toggleShoot = true;
			}
			else {
				toggleShoot = false;
			}
			break;
		case 0x56:  //V key
			if (!timeStop) {
				timeStopDurationLeft = 10000;
				timeStop = true;
				asteroidVelocity = D3DXVECTOR2(0, 0);
			}
			else {
				timeStop = false;
			}
			break;
		default:
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void removeBulletGap(int removedIndex) {
	for (int i = removedIndex; i < bulletEntry - 1; i++) {
		bulletTrans[i] = bulletTrans[i + 1];
	}
	bulletEntry--;
}

void removeAsteroidGap(int removedIndex) {
	for (int i = removedIndex; i < asteroidEntry - 1; i++) {
		asteroidTrans[i] = asteroidTrans[i + 1];
	}
	asteroidEntry--;
}

void removePowerUpGap(int removedIndex) {
	for (int i = removedIndex; i < powerUpEntry - 1; i++) {
		powerUpTrans[i] = powerUpTrans[i + 1];
	}
	powerUpEntry--;
}

void createSplashWindow() {
	ZeroMemory(&wndStruct.splashWndClass, sizeof(wndStruct.splashWndClass));

	wndStruct.splashWndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndStruct.splashWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndStruct.splashWndClass.hInstance = GetModuleHandle(NULL);
	wndStruct.splashWndClass.lpfnWndProc = SplashWindowProcedure;
	wndStruct.splashWndClass.lpszClassName = "Splash Window";
	wndStruct.splashWndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wndStruct.splashWndClass);

	wndStruct.g_hWnd2 = CreateWindowEx(0, wndStruct.splashWndClass.lpszClassName, "SplashScreen", WS_DISABLED | WS_POPUP, 300, 300, splashScreenWidth, splashScreenHeight, NULL, NULL, GetModuleHandle(NULL), NULL);

	ShowWindow(wndStruct.g_hWnd2, 1);
	ShowCursor(true);
}

void createWindow() {

	ZeroMemory(&wndStruct.wndClass, sizeof(wndStruct.wndClass));

	wndStruct.wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndStruct.wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndStruct.wndClass.hInstance = GetModuleHandle(NULL);
	wndStruct.wndClass.lpfnWndProc = WindowProcedure;
	wndStruct.wndClass.lpszClassName = "Game Window";
	wndStruct.wndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wndStruct.wndClass);

	wndStruct.g_hWnd = CreateWindowEx(0, wndStruct.wndClass.lpszClassName, "Spaceship Xtreme 2.0 (PRESS ESC TO EXIT THE PROGRAM)", WS_OVERLAPPEDWINDOW, 0, 100, screenWidth, screenHeight, NULL, NULL, GetModuleHandle(NULL), NULL);

	ShowWindow(wndStruct.g_hWnd, 1);
	ShowCursor(false);
	ShowCursor(false);
}

bool windowIsRunning() {
	if (PeekMessage(&wndStruct.msg, NULL, 0, 0, PM_REMOVE))
	{
		if (wndStruct.msg.message == WM_QUIT)
			return false;
		TranslateMessage(&wndStruct.msg);
		DispatchMessage(&wndStruct.msg);
	}
	return true;
}

void cleanupWindow() {
	UnregisterClass(wndStruct.wndClass.lpszClassName, GetModuleHandle(NULL));
}

void cleanupSplashWindow() {
	UnregisterClass(wndStruct.splashWndClass.lpszClassName, GetModuleHandle(NULL));
}

void createDirectX() {

	ZeroMemory(&directStruct.d3dPP, sizeof(directStruct.d3dPP));

	directStruct.d3dPP.Windowed = true;
	directStruct.d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	directStruct.d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	directStruct.d3dPP.BackBufferCount = 1;
	directStruct.d3dPP.BackBufferWidth = screenWidth;
	directStruct.d3dPP.BackBufferHeight = screenHeight;
	directStruct.d3dPP.hDeviceWindow = wndStruct.g_hWnd;

	HRESULT hr = directStruct.direct3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wndStruct.g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &directStruct.d3dPP, &directStruct.d3dDevice);

	if (FAILED(hr))
		cout << "Creating Directx Failed !!!";
}

void createSplashDirectX() {

	ZeroMemory(&directStruct.splashD3dPP, sizeof(directStruct.splashD3dPP));

	directStruct.splashD3dPP.Windowed = true;
	directStruct.splashD3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	directStruct.splashD3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	directStruct.splashD3dPP.BackBufferCount = 1;
	directStruct.splashD3dPP.BackBufferWidth = splashScreenWidth;
	directStruct.splashD3dPP.BackBufferHeight = splashScreenHeight;
	directStruct.splashD3dPP.hDeviceWindow = wndStruct.g_hWnd2;

	HRESULT hr = directStruct.splashDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wndStruct.g_hWnd2, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &directStruct.splashD3dPP, &directStruct.splashD3dDevice);

	if (FAILED(hr))
		cout << "Creating Directx Failed !!!";
}

void spriteRender() {
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	//Sprite Transform Object - (scalingCenter, scalingRotation, scaling, rotationCenter, rotation, trans)
	pointerTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(currentXpos, currentYpos));
	spaceshipTrans = SpriteTransform(D3DXVECTOR2(spaceshipSprite.getSpriteWidth() / 2, spaceshipSprite.getSpriteHeight() / 2), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(spaceshipSprite.getSpriteWidth() / 2, spaceshipSprite.getSpriteHeight() / 2), spaceshipRotation, spaceshipPosition);
	thrustTrans = SpriteTransform(D3DXVECTOR2(thrustSprite.getSpriteWidth() / 2, thrustSprite.getSpriteHeight() / 2), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(thrustSprite.getSpriteWidth() / 2, thrustSprite.getSpriteHeight() / 2), spaceshipRotation, spaceshipPosition + D3DXVECTOR2(spaceshipSprite.getSpriteWidth() / 2 - 4, spaceshipSprite.getSpriteHeight()));
	turretTrans = SpriteTransform(D3DXVECTOR2(turretSprite.getSpriteWidth() / 2, turretSprite.getSpriteHeight() / 2), 0, D3DXVECTOR2(0.35, 0.35), D3DXVECTOR2(turretSprite.getSpriteWidth() / 2, turretSprite.getSpriteHeight() / 2), turretRotation, turretPosition);

	textTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(800, 100));
	timerTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(520, 35));
	livesTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1000, 35));
	helpTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(50, 35));
	helpText2Trans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(50, 70));
	scoresTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(50, 105));
	highScoresTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(50, 140));

	//Text
	textRect.left = 0;
	textRect.top = 0;
	textRect.right = 300;
	textRect.bottom = 125;

	//Scores Text
	scoresTextRect.left = 0;
	scoresTextRect.top = 0;
	scoresTextRect.right = 200;
	scoresTextRect.bottom = 125;
	//Timer Text
	timerTextRect.left = 0;
	timerTextRect.top = 0;
	timerTextRect.right = 100;
	timerTextRect.bottom = 125;
	//Lives Text
	livesTextRect.left = 0;
	livesTextRect.top = 0;
	livesTextRect.right = 100;
	livesTextRect.bottom = 125;

	//Draw Sprite

	thrustTrans.transform();
	sprite->SetTransform(&thrustTrans.getMat());
	sprite->Draw(thrustTexture.getTexture(), &thrustSprite.crop(), NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	spaceshipTrans.transform();
	sprite->SetTransform(&spaceshipTrans.getMat());
	sprite->Draw(currentSpaceshipTexture.getTexture(), &spaceshipSprite.crop(), NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	turretTrans.transform();
	sprite->SetTransform(&turretTrans.getMat());
	sprite->Draw(turretTexture.getTexture(), &turretSprite.crop(), NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	for (int i = 0; i < bulletEntry; i++) {
		bulletTrans[i].transform();
		sprite->SetTransform(&bulletTrans[i].getMat());
		sprite->Draw(bulletTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));
	}
	for (int i = 0; i < asteroidEntry; i++) {
		asteroidTrans[i].transform();
		sprite->SetTransform(&asteroidTrans[i].getMat());
		sprite->Draw(asteroidTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));
	}
	for (int i = 0; i < powerUpEntry; i++) {
		powerUpTrans[i].transform();
		sprite->SetTransform(&powerUpTrans[i].getMat());
		if (powerUpTrans[i].getPowerUpChosen() == hpPowerUp) {
			powerUpTexture = hpPowerUpTexture;
		}
		else if (powerUpTrans[i].getPowerUpChosen() == bulletPowerUp) {
			powerUpTexture = bulletPowerUpTexture;
		}
		else if (powerUpTrans[i].getPowerUpChosen() == timePowerUp) {
			powerUpTexture = timePowerUpTexture;
		}
		sprite->Draw(powerUpTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));
	}


	pointerTrans.transform();
	sprite->SetTransform(&pointerTrans.getMat());
	sprite->Draw(pointerTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Draw Mouse Position Font
	//textTrans.transform();

	//sprite->SetTransform(&textTrans.getMat());
	//strCurrentXpos = to_string(currentXpos);
	//strCurrentYpos = to_string(currentYpos);
	//for (int i = 0; i < strCurrentXpos.length(); i++) {
	//	posText[i] = strCurrentXpos[i];
	//}
	//posText[4] = ',';
	//for (int i = 0; i < strCurrentYpos.length(); i++) {
	//	posText[i + 5] = strCurrentYpos[i];
	//}

	//font->DrawText(sprite, posText, -1, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	//for (int i = 0; i < sizeof(posText); i++) {
	//	posText[i] = ' ';
	//}
	//Draw Timer Font
	timerTextTrans.transform();

	sprite->SetTransform(&timerTextTrans.getMat());
	strWaveSec = to_string(waveSec);
	strWaveMin = to_string(waveMin);
	for (int i = 0; i < strWaveMin.length(); i++) {
		timerText[i] = strWaveMin[i];
	}
	timerText[strWaveMin.length()] = ':';
	for (int i = 0; i < strWaveSec.length(); i++) {
		timerText[i + strWaveMin.length() + 1] = strWaveSec[i];
	}
	font->DrawText(sprite, timerText, -1, &timerTextRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	for (int i = 0; i < sizeof(timerText); i++) {
		timerText[i] = ' ';
	}
	//Draw Lives Font
	livesTextTrans.transform();

	sprite->SetTransform(&livesTextTrans.getMat());
	strLivesPostfix = to_string(lives);
	for (int i = 0; i < strLivesPrefix.length(); i++) {
		livesText[i] = strLivesPrefix[i];
	}
	for (int i = 0; i < strLivesPostfix.length(); i++) {
		livesText[i + strLivesPrefix.length()] = strLivesPostfix[i];
	}
	font->DrawText(sprite, livesText, -1, &livesTextRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	for (int i = 0; i < sizeof(livesText); i++) {
		livesText[i] = ' ';
	}
	//Draw Help Font
	helpTextTrans.transform();

	sprite->SetTransform(&helpTextTrans.getMat());
	font->DrawText(sprite, "Left click to shoot", -1, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	//Draw Help Font 2
	helpText2Trans.transform();

	sprite->SetTransform(&helpText2Trans.getMat());
	font->DrawText(sprite, "Press X to toggle the shooting", -1, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	//Draw Scores Font
	scoresTextTrans.transform();

	sprite->SetTransform(&scoresTextTrans.getMat());
	strScoresPostfix = to_string(scores);
	for (int i = 0; i < strScoresPrefix.length(); i++) {
		scoresText[i] = strScoresPrefix[i];
	}
	for (int i = 0; i < strScoresPostfix.length(); i++) {
		scoresText[i + strScoresPrefix.length()] = strScoresPostfix[i];
	}
	font->DrawText(sprite, scoresText, -1, &scoresTextRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	for (int i = 0; i < sizeof(scoresText); i++) {
		scoresText[i] = ' ';
	}
	//Draw High Scores Font
	highScoresTextTrans.transform();

	sprite->SetTransform(&highScoresTextTrans.getMat());
	strHighScoresPostfix = to_string(highScores);
	for (int i = 0; i < strHighScoresPrefix.length(); i++) {
		highScoresText[i] = strHighScoresPrefix[i];
	}
	for (int i = 0; i < strHighScoresPostfix.length(); i++) {
		highScoresText[i + strHighScoresPrefix.length()] = strHighScoresPostfix[i];
	}
	font->DrawText(sprite, highScoresText, -1, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	for (int i = 0; i < sizeof(highScoresText); i++) {
		highScoresText[i] = ' ';
	}
	sprite->End();
}

void render() {

	directStruct.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(red, green, blue), 1.0f, 0);

	directStruct.d3dDevice->BeginScene();

	spriteRender();

	directStruct.d3dDevice->EndScene();

	directStruct.d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void splashRender() {
	directStruct.splashD3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(red, green, blue), 1.0f, 0);

	directStruct.splashD3dDevice->BeginScene();

	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	sprite->Draw(splashTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	sprite->End();

	directStruct.splashD3dDevice->EndScene();

	directStruct.splashD3dDevice->Present(NULL, NULL, NULL, NULL);

}

void cleanupDirectX() {
	directStruct.d3dDevice->Release();
	directStruct.d3dDevice = NULL;
}

void cleanupSplashDirectX() {
	directStruct.splashD3dDevice->Release();
	directStruct.splashD3dDevice = NULL;
}

void createSplashSprite() {
	HRESULT hr = D3DXCreateSprite(directStruct.splashD3dDevice, &sprite);

	if (FAILED(hr)) {
		cout << "Create Sprite Failed!!!";
	}

	hr = splashTexture.createSplashTextureFromFile();

	if (FAILED(hr)) {
		cout << "Create Texture from File Failed!!!";
	}
}

void createSprite() {
	HRESULT hr = D3DXCreateSprite(directStruct.d3dDevice, &sprite);

	if (FAILED(hr)) {
		cout << "Create Sprite Failed!!!";
	}

	hr = D3DXCreateFont(directStruct.d3dDevice, 25, 0, 0, 1, false,
		DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial", &font);

	if (FAILED(hr)) {
		cout << "Create Font Failed!!!";
	}

	hr = thrustTexture.createTextureFromFile();
	hr = spaceshipTexture.createTextureFromFile();
	hr = turretTexture.createTextureFromFile();
	hr = asteroidTexture.createTextureFromFile();
	hr = bulletTexture.createTextureFromFile();
	hr = hpPowerUpTexture.createTextureFromFile();
	hr = bulletPowerUpTexture.createTextureFromFile();
	hr = timePowerUpTexture.createTextureFromFile();
	hr = cursorTexture.createTextureFromFile();
	hr = buttonBgTexture.createTextureFromFile();
	hr = bgTexture.createTextureFromFile();
	hr = spaceship2Texture.createTextureFromFile();
	hr = crosshairTexture.createTextureFromFile();
	hr = crosshair2Texture.createTextureFromFile();
	hr = crosshair3Texture.createTextureFromFile();

	if (FAILED(hr)) {
		cout << "Create Texture from File Failed!!!";
	}
}

void cleanupSprite() {
	sprite->Release();
	sprite = NULL;

	pointerTexture.releaseTexture();

	thrustTexture.releaseTexture();

	spaceshipTexture.releaseTexture();

	turretTexture.releaseTexture();

	asteroidTexture.releaseTexture();

	bulletTexture.releaseTexture();

	hpPowerUpTexture.releaseTexture();

	bulletPowerUpTexture.releaseTexture();

	timePowerUpTexture.releaseTexture();

	powerUpTexture.releaseTexture();

	cursorTexture.releaseTexture();

	buttonBgTexture.releaseTexture();

	bgTexture.releaseTexture();

	spaceship2Texture.releaseTexture();

	currentSpaceshipTexture.releaseTexture();

	crosshairTexture.releaseTexture();

	crosshair2Texture.releaseTexture();

	crosshair3Texture.releaseTexture();

	font->Release();
	font = NULL;
}

void cleanupSplashSprite() {
	splashTexture.releaseTexture();
	splashTexture.setTexture(NULL);
}

void createDirectInput() {

	HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), 0x0800, IID_IDirectInput8, (void**)&dInput, NULL);

	hr = dInput->CreateDevice(GUID_SysKeyboard, &dInputKeyboardDevice, NULL);

	dInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);

	dInputKeyboardDevice->SetCooperativeLevel(wndStruct.g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = dInput->CreateDevice(GUID_SysMouse, &dInputMouseDevice, NULL);

	dInputMouseDevice->SetDataFormat(&c_dfDIMouse);

	dInputMouseDevice->SetCooperativeLevel(wndStruct.g_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
}

void getInput() {
	dInputKeyboardDevice->Acquire();
	dInputMouseDevice->Acquire();
	dInputKeyboardDevice->GetDeviceState(256, diKeys);

	dInputMouseDevice->GetDeviceState(sizeof(mouseState), (LPVOID)&mouseState);
}

void cleanupInput() {
	//	Release keyboard device.
	dInputKeyboardDevice->Unacquire();
	dInputKeyboardDevice->Release();
	dInputKeyboardDevice = NULL;

	//	Release mouse device.
	dInputMouseDevice->Unacquire();
	dInputMouseDevice->Release();
	dInputMouseDevice = NULL;

	//	Release DirectInput.
	dInput->Release();
	dInput = NULL;
}

void collisionDetection() {

	// Collision Between Bullet and Wall
	for (int i = 0; i < bulletEntry; i++) {
		if (bulletTrans[i].getTrans().x > screenWidth - bulletSprite.getTotalSpriteWidth() || bulletTrans[i].getTrans().x < 0 || bulletTrans[i].getTrans().y < 0 || bulletTrans[i].getTrans().y > screenHeight - bulletSprite.getTotalSpriteHeight()) {
			removeBulletGap(i);
			break;
		}
	}
	// Collision Between Asteroid and Wall
	for (int i = 0; i < asteroidEntry; i++) {
		if (asteroidTrans[i].getTrans().x > screenWidth || asteroidTrans[i].getTrans().x < 0 - asteroidSprite.getTotalSpriteWidth() * asteroidTrans[i].getScaling().x || asteroidTrans[i].getTrans().y > screenHeight) {
			removeAsteroidGap(i);
			break;
		}
	}

	// Collision Between Asteroid and Spaceship
	for (int i = 0; i < asteroidEntry; i++) {
		if (spaceshipPosition.x + spaceshipSprite.getSpriteWidth() >= asteroidTrans[i].getTrans().x && spaceshipPosition.x <= asteroidTrans[i].getTrans().x + asteroidSprite.getTotalSpriteWidth() * asteroidTrans[i].getScaling().x && spaceshipPosition.y <= asteroidTrans[i].getTrans().y + asteroidSprite.getTotalSpriteHeight() * asteroidTrans[i].getScaling().y && spaceshipPosition.y + spaceshipSprite.getSpriteHeight() >= asteroidTrans[i].getTrans().y) {
			if (lives > 0) {
				myAudioManager->PlayHit();
				lives--;
			}
			if (lives <= 0) {
				//MessageBox(NULL, TEXT("YOU DIED\n"), TEXT("GIT GUD"), MB_OK | MB_ICONWARNING);
				currentMenu = GameOverMenu;
				myAudioManager->PlayBoom();
				if (scores > highScores) {
					highScores = scores;
				}
			}
			removeAsteroidGap(i);
			break;
		}
	}
	collisionDetected = false;
	// Collision Between Bullet and Asteroid
	for (int i = 0; i < bulletEntry; i++) {
		for (int j = 0; j < asteroidEntry; j++) {
			if (bulletTrans[i].getTrans().x + bulletSprite.getTotalSpriteWidth() >= asteroidTrans[j].getTrans().x && bulletTrans[i].getTrans().x <= asteroidTrans[j].getTrans().x + asteroidSprite.getTotalSpriteWidth() * asteroidTrans[j].getScaling().x && bulletTrans[i].getTrans().y <= asteroidTrans[j].getTrans().y + asteroidSprite.getTotalSpriteHeight() * asteroidTrans[j].getScaling().y && bulletTrans[i].getTrans().y + bulletSprite.getTotalSpriteHeight() >= asteroidTrans[j].getTrans().y) {
				scores++;
				    //PHYSICS 
					// DONT READ, YOU NEVER UNDERSTAND
					// HERE IS THE LINK FOR THE PHYSICS
					// https://en.wikipedia.org/wiki/Elastic_collision
				if (asteroidTrans[j].getAsteroidHp() > 0) {
					if (asteroidTrans[j].getAsteroidChosen() == smallAsteroid) {
						asteroidPower = smallAsteroidPower;
						asteroidMass = smallAsteroidMass;
					}
					else if (asteroidTrans[j].getAsteroidChosen() == mediumAsteroid) {
						asteroidPower = mediumAsteroidPower;
						asteroidMass = mediumAsteroidMass;
					}
					else if (asteroidTrans[j].getAsteroidChosen() == largeAsteroid) {
						asteroidPower = largeAsteroidPower;
						asteroidMass = largeAsteroidMass;
					}
					asteroidVelocity.x = sin(180 * PI / 180) * asteroidPower;
					asteroidVelocity.y = -cos(180 * PI / 180) * asteroidPower;
					bulletVelocity.x = sin(bulletTrans[i].getRotation()) * bulletPower;
					bulletVelocity.y = -cos(bulletTrans[i].getRotation()) * bulletPower;
					massSum = bulletMass + asteroidMass;
					diffVelocity = asteroidVelocity - bulletVelocity;
					diffPos = asteroidTrans[j].getTrans() - bulletTrans[i].getTrans();
					dotProduct = D3DXVec2Dot(&diffVelocity, &diffPos);
					magnitude = D3DXVec2Length(&diffPos);
					diffPosX = asteroidTrans[j].getTrans().x - bulletTrans[i].getTrans().x;
					diffPosY = asteroidTrans[j].getTrans().y - bulletTrans[i].getTrans().y;
					asteroidVelocity.x = asteroidVelocity.x - (2*bulletMass/massSum) * dotProduct/pow(magnitude,2) * diffPosX;
					asteroidVelocity.y = asteroidVelocity.y - (2 * bulletMass / massSum) * dotProduct / pow(magnitude, 2) * diffPosY;
					asteroidPosition = asteroidTrans[j].getTrans() + asteroidVelocity;
					asteroidTrans[j].setTrans(asteroidPosition);
					asteroidTrans[j].setAsteroidVelocity(asteroidVelocity);
					asteroidTrans[j].reduceAsteroidHp();
				}
				if(asteroidTrans[j].getAsteroidHp() <= 0) {
					removeAsteroidGap(j);
				}
				removeBulletGap(i);
				collisionDetected = true;
				break;
			}
		}
		if (collisionDetected) {
			break;
		}
	}
	// Collision Between Spaceship and Powerup
	for (int i = 0; i < powerUpEntry; i++) {
		if (spaceshipPosition.x + spaceshipSprite.getSpriteWidth() >= powerUpTrans[i].getTrans().x && spaceshipPosition.x <= powerUpTrans[i].getTrans().x + hpPowerUpSprite.getTotalSpriteWidth() && spaceshipPosition.y <= powerUpTrans[i].getTrans().y + hpPowerUpSprite.getTotalSpriteHeight() && spaceshipPosition.y + spaceshipSprite.getSpriteHeight() >= powerUpTrans[i].getTrans().y) {
			if (powerUpTrans[i].getPowerUpChosen() == hpPowerUp) {
				cout << "HP PICKED" << endl;
				myAudioManager->PlayPickUp();
				if (lives < 3) {
					lives++;
				}
			}
			if (powerUpTrans[i].getPowerUpChosen() == bulletPowerUp) {
				cout << "BULLET PICKED" << endl;
				myAudioManager->PlayPickUp();
				bulletTimer->init(bulletPowerUpSpeed);
				bulletPowerUpPicked = true;
				bulletPowerUpDurationLeft = bulletPowerUpDuration;
			}
			if (powerUpTrans[i].getPowerUpChosen() == timePowerUp) {
				cout << "TIMESTOP PICKED" << endl;
				myAudioManager->PlayTheWorld();
				timeStop = true;
				asteroidVelocity = D3DXVECTOR2(0, 0);
				timeStopDurationLeft = timeStopDuration;
			}
			removePowerUpGap(i);
			break;
		}
	}
}

void update(int frames) {
	turretPosition = spaceshipPosition - D3DXVECTOR2(spaceshipSprite.getSpriteWidth() / 2 + 10, spaceshipSprite.getSpriteHeight() / 2 + 5);
	if (frames > 1) {
		frames = 1;
	}
	for (int i = 0; i < frames; i++)
	{
		// Bullet Movement
		for (int i = 0; i < bulletEntry; i++) {
			bulletVelocity.x = sin(bulletTrans[i].getRotation()) * bulletPower;
			bulletVelocity.y = -cos(bulletTrans[i].getRotation()) * bulletPower;
			bulletPos = bulletTrans[i].getTrans() + bulletVelocity;
			bulletTrans[i] = SpriteTransform(D3DXVECTOR2(bulletSprite.getTotalSpriteWidth() / 2, bulletSprite.getTotalSpriteHeight() / 2), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(bulletSprite.getTotalSpriteWidth() / 2, bulletSprite.getTotalSpriteHeight() / 2), bulletTrans[i].getRotation(), bulletPos);
		}

		// Asteroid Movement
		if (!timeStop) {
			for (int i = 0; i < asteroidEntry; i++) {
				asteroidRotation = asteroidTrans[i].getRotation() + asteroidRotationRate;
				if (asteroidTrans[i].getAsteroidChosen() == smallAsteroid) {
					asteroidPower = smallAsteroidPower;
				}
				else if (asteroidTrans[i].getAsteroidChosen() == mediumAsteroid) {
					asteroidPower = mediumAsteroidPower;
				}
				else if (asteroidTrans[i].getAsteroidChosen() == largeAsteroid) {
					asteroidPower = largeAsteroidPower;
				}
				asteroidVelocity.x = sin(180 * PI / 180) * asteroidPower;
				asteroidVelocity.y = -cos(180 * PI / 180) * asteroidPower;
				asteroidPosition = asteroidTrans[i].getTrans() + asteroidVelocity;
				asteroidTrans[i].setTrans(asteroidPosition);
				asteroidTrans[i].setRotation(asteroidRotation);
			}
		}
		if (timeStop) {
			for (int i = 0; i < asteroidEntry; i++) {
				asteroidPosition = asteroidTrans[i].getTrans() + asteroidTrans[i].getAsteroidVelocity();
				asteroidTrans[i].setAsteroidVelocity(asteroidTrans[i].getAsteroidVelocity() *= (1-asteroidFriction));
				asteroidTrans[i].setTrans(asteroidPosition);
			}
		}

		// Spaceship Movement
		if ((int)spaceshipVelocity.x == 0 && (int)spaceshipVelocity.y == 0) {
			spaceshipSprite.staticFrame();
		}
		if (diKeys[DIK_A] & 0x80) {
			spaceshipEngineForce.x += sin(270 * PI / 180) * spaceshipEnginePower;
			spaceshipEngineForce.y += -cos(270 * PI / 180) * spaceshipEnginePower;
			spaceshipAcceleration = spaceshipEngineForce / spaceshipMass;
			spaceshipSprite.leftFrame();
		}
		if (diKeys[DIK_D] & 0x80) {
			spaceshipEngineForce.x += sin(90 * PI / 180) * spaceshipEnginePower;
			spaceshipEngineForce.y += -cos(90 * PI / 180) * spaceshipEnginePower;
			spaceshipAcceleration = spaceshipEngineForce / spaceshipMass;
			spaceshipSprite.rightFrame();
		}
		if (diKeys[DIK_W] & 0x80) {
			spaceshipEngineForce.x += sin(0) * spaceshipEnginePower;
			spaceshipEngineForce.y += -cos(0) * spaceshipEnginePower;
			spaceshipAcceleration = spaceshipEngineForce / spaceshipMass;
			spaceshipSprite.staticFrame();
		}
		if (diKeys[DIK_S] & 0x80) {
			spaceshipEngineForce.x += sin(180 * PI / 180) * spaceshipEnginePower;
			spaceshipEngineForce.y += -cos(180 * PI / 180) * spaceshipEnginePower;
			spaceshipAcceleration = spaceshipEngineForce / spaceshipMass;
			spaceshipSprite.staticFrame();
		}
		if ((diKeys[DIK_A] & 0x80) && (diKeys[DIK_W] & 0x80)) {
			spaceshipSprite.leftFrame();
		}
		if ((diKeys[DIK_W] & 0x80) && (diKeys[DIK_D] & 0x80)) {
			spaceshipSprite.rightFrame();
		}
		if ((diKeys[DIK_S] & 0x80) && (diKeys[DIK_A] & 0x80)) {
			spaceshipSprite.leftFrame();
		}
		if ((diKeys[DIK_S] & 0x80) && (diKeys[DIK_D] & 0x80)) {
			spaceshipSprite.rightFrame();
		}

		spaceshipVelocity += spaceshipAcceleration;
		spaceshipVelocity *= (1 - friction);
		spaceshipPosition += spaceshipVelocity;


		//Spaceship right checking
		if (spaceshipPosition.x > screenWidth - spaceshipSprite.getSpriteWidth()) {
			spaceshipPosition.x = screenWidth - spaceshipSprite.getSpriteWidth();
			spaceshipVelocity.x *= -1;
		}
		//Spaceship left checking
		if (spaceshipPosition.x < 0) {
			spaceshipPosition.x = 0;
			spaceshipVelocity.x *= -1;
		}
		//Spaceship top checking
		if (spaceshipPosition.y < 0) {
			spaceshipPosition.y = 0;
			spaceshipVelocity.y *= -1;
		}
		//Spaceship bottom checking
		if (spaceshipPosition.y > screenHeight - spaceshipSprite.getSpriteHeight()) {
			spaceshipPosition.y = screenHeight - spaceshipSprite.getSpriteHeight();
			spaceshipVelocity.y *= -1;
		}

		collisionDetection();

		spaceshipAcceleration = D3DXVECTOR2(0, 0);
		spaceshipEngineForce = D3DXVECTOR2(0, 0);
	}

	pointerCenterX = currentXpos + pointerSprite.getTotalSpriteWidth() / 2;
	pointerCenterY = currentYpos + pointerSprite.getTotalSpriteHeight() / 2;
	turretCenterX = turretPosition.x + turretSprite.getSpriteWidth() / 2;
	turretCenterY = turretPosition.y + turretSprite.getSpriteHeight() / 2;

	if (pointerCenterY > turretCenterY) {
		turretRotation = PI - asin((pointerCenterX - turretCenterX) / sqrt(pow(pointerCenterX - turretCenterX, 2) + pow(turretCenterY - pointerCenterY, 2)));
	}
	else {
		turretRotation = asin((pointerCenterX - turretCenterX) / sqrt(pow(pointerCenterX - turretCenterX, 2) + pow(turretCenterY - pointerCenterY, 2)));
	}

	//Left click
	if (mouseState.rgbButtons[0] & 0x80) {
		//do something
	}
	//Right click
	if (mouseState.rgbButtons[1] & 0x80) {
		//do something
	}
	if (diKeys[DIK_ESCAPE] & 0x80) {
		PostQuitMessage(0);
	}
	//Mouse position
	if (currentYpos >= 0 && currentYpos <= screenHeight - pointerSprite.getTotalSpriteHeight()) {
		currentYpos += mouseState.lY;
	}
	if (currentYpos < 0 || currentYpos > screenHeight - pointerSprite.getTotalSpriteHeight()) {
		currentYpos -= mouseState.lY;
	}
	if (currentXpos >= 0 && currentXpos <= screenWidth - pointerSprite.getTotalSpriteWidth()) {
		currentXpos += mouseState.lX;
	}
	if (currentXpos < 0 || currentXpos > screenWidth - pointerSprite.getTotalSpriteWidth()) {
		currentXpos -= mouseState.lX;
	}
}

void updateBullet(int frames) {
	bulletStartPosition = D3DXVECTOR2(spaceshipPosition.x + spaceshipSprite.getSpriteWidth() / 2 - 5, spaceshipPosition.y + spaceshipSprite.getSpriteHeight() / 2 - 5);
	if (frames > 1) {
		frames = 1;
	}
	for (int i = 0; i < frames; i++) {
		//Left click
		if (mouseState.rgbButtons[0] & 0x80 || toggleShoot == true) {
			bulletTrans[bulletEntry] = SpriteTransform(D3DXVECTOR2(bulletSprite.getTotalSpriteWidth() / 2, bulletSprite.getTotalSpriteHeight() / 2), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(bulletSprite.getTotalSpriteWidth() / 2, bulletSprite.getTotalSpriteHeight() / 2), turretRotation, bulletStartPosition);
			bulletEntry++;
			myAudioManager->PlayShoot(screenWidth, spaceshipPosition.x);
		}
	}

}
void updateThrust(int frames) {
	if (frames > 1) {
		frames = 1;
	}
	for (int i = 0; i < frames; i++) {
		thrustSprite.nextThrustFrame();
	}
}
void updateAsteroid(int frames) {
	if (frames > 1) {
		frames = 1;
	}
	if (!timeStop) {
		for (int i = 0; i < frames; i++) {
			asteroidStartPosition.x = 50 + (rand() % 1100);
			asteroidStartRotation = rand() % 360;
			if(currentPhase== FirstPhase){
				chosenAsteroid = rand() % 1;
			} 
			if (currentPhase == SecondPhase) {
				chosenAsteroid = rand() % 2;
			}
			if (currentPhase == ThirdPhase) {
				chosenAsteroid = rand() % 3;
			}

			if (chosenAsteroid == smallAsteroid) {
				chosenAsteroidHp = smallAsteroidHp;
				asteroidScaling = D3DXVECTOR2(1, 1);
			}
			if (chosenAsteroid == mediumAsteroid) {
				chosenAsteroidHp = mediumAsteroidHp;
				asteroidScaling = D3DXVECTOR2(1.5, 1.5);
			}
			if (chosenAsteroid == largeAsteroid) {
				chosenAsteroidHp = largeAsteroidHp;
				asteroidScaling = D3DXVECTOR2(2, 2);
			}
			asteroidTrans[asteroidEntry] = SpriteTransform(D3DXVECTOR2(35, 35), 0, asteroidScaling, D3DXVECTOR2(35, 35), asteroidStartRotation, asteroidStartPosition, chosenAsteroidHp, chosenAsteroid);
			asteroidEntry++;
		}
	}
}

void updateWave(int frames) {
	if (frames > 1) {
		frames = 1;
	}
	powerUpPosition.x = 50 + (rand() % 1200);
	powerUpPosition.y = 600;
	powerUpChosen = (rand() % 3);
	for (int i = 0; i < frames; i++) {
		waveSec++;
		powerUpSpawnRateLeft--;
		if (waveSec == 60) {
			waveSec = 0;
			waveMin++;
		}
		if (timeStop) {
			timeStopDurationLeft--;
			if (timeStopDurationLeft <= 0) {
				timeStop = false;
				timeStopDurationLeft = timeStopDuration;
			}
		}
		if (bulletPowerUpPicked) {
			bulletPowerUpDurationLeft--;
			if (bulletPowerUpDurationLeft <= 0) {
				bulletPowerUpPicked = false;
				bulletPowerUpDurationLeft = bulletPowerUpDuration;
				bulletTimer->init(defaultBulletInterval);
			}
		}
		if (powerUpSpawnRateLeft <= 0) {
			powerUpSpawnRateLeft = powerUpSpawnRate;
			powerUpTrans[powerUpEntry] = SpriteTransform(D3DXVECTOR2(35, 35), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(35, 35), 0, powerUpPosition, powerUpChosen);
			powerUpEntry++;
			if (powerUpEntry > 3) {
				removePowerUpGap(0);
			}
		}
		if (waveSec == secondPhaseTimer && waveMin < 1) {
			currentPhase = SecondPhase;
		}
		if (waveSec == thirdPhaseTimer && waveMin < 1) {
			currentPhase = ThirdPhase;
			myAudioManager->channel->setPaused(true);
			myAudioManager->PlayDoom();
		}
	}
}

void Sound() {
	myAudioManager->UpdateSound();
}

void updateSplash(int frames) {
	if (frames > 1) {
		frames = 1;
	}
	for (int i = 0; i < frames; i++) {
		splashCount++;
	}
}

void mainMenuUpdate() {

	if (currentYpos >= 0 && currentYpos <= screenHeight - cursorSprite.getTotalSpriteHeight()) {
		currentYpos += mouseState.lY;
	}
	if (currentYpos < 0 || currentYpos > screenHeight - cursorSprite.getTotalSpriteHeight()) {
		currentYpos -= mouseState.lY;
	}
	if (currentXpos >= 0 && currentXpos <= screenWidth - cursorSprite.getTotalSpriteWidth()) {
		currentXpos += mouseState.lX;
	}
	if (currentXpos < 0 || currentXpos > screenWidth - cursorSprite.getTotalSpriteWidth()) {
		currentXpos -= mouseState.lX;
	}
	if (mouseState.rgbButtons[0] & 0x80) {
		if (cursorTrans.getTrans().x  >= buttonBgTrans.getTrans().x && cursorTrans.getTrans().x <= buttonBgTrans.getTrans().x + buttonBgSprite.getTotalSpriteWidth() && cursorTrans.getTrans().y <= buttonBgTrans.getTrans().y + buttonBgSprite.getTotalSpriteHeight() && cursorTrans.getTrans().y  >= buttonBgTrans.getTrans().y) {
			myAudioManager->PlayButtonClick();
			currentMenu = SpaceshipSelectionMenu;
		}

	}

	if (diKeys[DIK_ESCAPE] & 0x80) {
		PostQuitMessage(0);
	}
}

void mainMenuSpriteRender() {
	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	//Sprite Transform Object - (scalingCenter, scalingRotation, scaling, rotationCenter, rotation, trans)
	cursorTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(currentXpos, currentYpos));
	buttonBgTrans = SpriteTransform(D3DXVECTOR2(buttonBgSprite.getTotalSpriteWidth()/2, buttonBgSprite.getTotalSpriteHeight()/2), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(300, 300));

	textTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(380, 340));
	titleTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(250, 200));

	//Text
	textRect.left = 0;
	textRect.top = 0;
	textRect.right = 350;
	textRect.bottom = 125;

	//Draw Sprite

	buttonBgTrans.transform();
	sprite->SetTransform(&buttonBgTrans.getMat());
	sprite->Draw(buttonBgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	titleTextTrans.transform();
	sprite->SetTransform(&titleTextTrans.getMat());
	font->DrawText(sprite, "Welcome to Spaceship Xtreme 2.0", -1, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	textTrans.transform();
	sprite->SetTransform(&textTrans.getMat());
	font->DrawText(sprite, "Start", -1, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	cursorTrans.transform();
	sprite->SetTransform(&cursorTrans.getMat());
	sprite->Draw(cursorTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	sprite->End();
}

void mainMenuRender() {
	directStruct.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(red, green, blue), 1.0f, 0);

	directStruct.d3dDevice->BeginScene();

	mainMenuSpriteRender();

	directStruct.d3dDevice->EndScene();

	directStruct.d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void spaceshipSelectionMenuUpdate() {
	if (currentYpos >= 0 && currentYpos <= screenHeight - cursorSprite.getTotalSpriteHeight()) {
		currentYpos += mouseState.lY;
	}
	if (currentYpos < 0 || currentYpos > screenHeight - cursorSprite.getTotalSpriteHeight()) {
		currentYpos -= mouseState.lY;
	}
	if (currentXpos >= 0 && currentXpos <= screenWidth - cursorSprite.getTotalSpriteWidth()) {
		currentXpos += mouseState.lX;
	}
	if (currentXpos < 0 || currentXpos > screenWidth - cursorSprite.getTotalSpriteWidth()) {
		currentXpos -= mouseState.lX;
	}
	if (currentTransitionPos == 0 && afterTransition == false) {
		if (mouseState.rgbButtons[0] & 0x80) {
			if (cursorTrans.getTrans().x >= spaceshipSelectionTrans.getTrans().x && cursorTrans.getTrans().x <= spaceshipSelectionTrans.getTrans().x + buttonBgSprite.getTotalSpriteWidth() && cursorTrans.getTrans().y <= spaceshipSelectionTrans.getTrans().y + buttonBgSprite.getTotalSpriteHeight() && cursorTrans.getTrans().y >= spaceshipSelectionTrans.getTrans().y) {
				currentSpaceshipTexture = spaceshipTexture;
				myAudioManager->PlayButtonClick();
				afterTransition = true;
			}
		}
		if (mouseState.rgbButtons[0] & 0x80) {
			if (cursorTrans.getTrans().x >= spaceship2SelectionTrans.getTrans().x && cursorTrans.getTrans().x <= spaceship2SelectionTrans.getTrans().x + buttonBgSprite.getTotalSpriteWidth() && cursorTrans.getTrans().y <= spaceship2SelectionTrans.getTrans().y + buttonBgSprite.getTotalSpriteHeight() && cursorTrans.getTrans().y >= spaceship2SelectionTrans.getTrans().y) {
				currentSpaceshipTexture = spaceship2Texture;
				myAudioManager->PlayButtonClick();
				afterTransition = true;
			}
		}
	}
	if (diKeys[DIK_ESCAPE] & 0x80) {
		PostQuitMessage(0);
	}
}

void spaceshipSelectionMenuSpriteRender(int frames) {
	if (frames > 1) {
		frames = 1;
	}
	
	for (int i = 0; i < frames; i++) {
		if (currentTransitionPos > 0) {
			currentTransitionPos -= beforeTransitionPos;
		}
		if (afterTransition) {
			currentTransitionPos -= afterTransitionPos;
		}
		if (currentTransitionPos <= -3000) {
			afterTransition = false;
			currentTransitionPos = 2000;
			currentMenu = CrosshairSelectionMenu;
		}
	}

	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	//Sprite Transform Object - (scalingCenter, scalingRotation, scaling, rotationCenter, rotation, trans)
	cursorTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(currentXpos, currentYpos));
	bgTrans = SpriteTransform(D3DXVECTOR2(350, 256), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(50+ currentTransitionPos, 50));
	spaceshipTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(280+ currentTransitionPos, 180));
	spaceship2Trans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(750+ currentTransitionPos, 180));
	spaceshipSelectionTrans = SpriteTransform(D3DXVECTOR2(100, 57.5), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(220+ currentTransitionPos, 300));
    spaceship2SelectionTrans = SpriteTransform(D3DXVECTOR2(100, 57.5), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(680+ currentTransitionPos, 300));

	textTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(350+ currentTransitionPos, 100));
	spaceshipSelectionTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(270+ currentTransitionPos, 330));
	spaceship2SelectionTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(730+ currentTransitionPos, 330));

	//Text
	textRect.left = 0;
	textRect.top = 0;
	textRect.right = 300;
	textRect.bottom = 125;

	bgTrans.transform();
	sprite->SetTransform(&bgTrans.getMat());
	sprite->Draw(bgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	textTrans.transform();
	sprite->SetTransform(&textTrans.getMat());
	font->DrawText(sprite, "Choose your spaceship", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	spaceshipTrans.transform();
	sprite->SetTransform(&spaceshipTrans.getMat());
	sprite->Draw(spaceshipTexture.getTexture(), &spaceshipSprite.crop(), NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	spaceship2Trans.transform();
	sprite->SetTransform(&spaceship2Trans.getMat());
	sprite->Draw(spaceship2Texture.getTexture(), &spaceshipSprite.crop(), NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	spaceshipSelectionTrans.transform();
	sprite->SetTransform(&spaceshipSelectionTrans.getMat());
	sprite->Draw(buttonBgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	spaceship2SelectionTrans.transform();
	sprite->SetTransform(&spaceship2SelectionTrans.getMat());
	sprite->Draw(buttonBgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	spaceshipSelectionTextTrans.transform();
	sprite->SetTransform(&spaceshipSelectionTextTrans.getMat());
	font->DrawText(sprite, "SELECT", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	spaceship2SelectionTextTrans.transform();
	sprite->SetTransform(&spaceship2SelectionTextTrans.getMat());
	font->DrawText(sprite, "SELECT", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	cursorTrans.transform();
	sprite->SetTransform(&cursorTrans.getMat());
	sprite->Draw(cursorTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	sprite->End();
}

void spaceshipSelectionMenuRender(int frames) {
	directStruct.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(red, green, blue), 1.0f, 0);

	directStruct.d3dDevice->BeginScene();

	spaceshipSelectionMenuSpriteRender(frames);

	directStruct.d3dDevice->EndScene();

	directStruct.d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void crosshairSelectionMenuUpdate() {
	if (currentYpos >= 0 && currentYpos <= screenHeight - cursorSprite.getTotalSpriteHeight()) {
		currentYpos += mouseState.lY;
	}
	if (currentYpos < 0 || currentYpos > screenHeight - cursorSprite.getTotalSpriteHeight()) {
		currentYpos -= mouseState.lY;
	}
	if (currentXpos >= 0 && currentXpos <= screenWidth - cursorSprite.getTotalSpriteWidth()) {
		currentXpos += mouseState.lX;
	}
	if (currentXpos < 0 || currentXpos > screenWidth - cursorSprite.getTotalSpriteWidth()) {
		currentXpos -= mouseState.lX;
	}
	if (currentTransitionPos == 0 && afterTransition == false) {
		if (mouseState.rgbButtons[0] & 0x80) {
			if (cursorTrans.getTrans().x >= crosshairSelectionTrans.getTrans().x && cursorTrans.getTrans().x <= crosshairSelectionTrans.getTrans().x + buttonBgSprite.getTotalSpriteWidth() && cursorTrans.getTrans().y <= crosshairSelectionTrans.getTrans().y + buttonBgSprite.getTotalSpriteHeight() && cursorTrans.getTrans().y >= crosshairSelectionTrans.getTrans().y) {
				pointerTexture = crosshairTexture;
				myAudioManager->PlayButtonClick();
				afterTransition = true;
			}
		}
		if (mouseState.rgbButtons[0] & 0x80) {
			if (cursorTrans.getTrans().x >= crosshair2SelectionTrans.getTrans().x && cursorTrans.getTrans().x <= crosshair2SelectionTrans.getTrans().x + buttonBgSprite.getTotalSpriteWidth() && cursorTrans.getTrans().y <= crosshair2SelectionTrans.getTrans().y + buttonBgSprite.getTotalSpriteHeight() && cursorTrans.getTrans().y >= crosshair2SelectionTrans.getTrans().y) {
				pointerTexture = crosshair2Texture;
				myAudioManager->PlayButtonClick();
				afterTransition = true;
			}
		}
		if (mouseState.rgbButtons[0] & 0x80) {
			if (cursorTrans.getTrans().x >= crosshair3SelectionTrans.getTrans().x && cursorTrans.getTrans().x <= crosshair3SelectionTrans.getTrans().x + buttonBgSprite.getTotalSpriteWidth() && cursorTrans.getTrans().y <= crosshair3SelectionTrans.getTrans().y + buttonBgSprite.getTotalSpriteHeight() && cursorTrans.getTrans().y >= crosshair3SelectionTrans.getTrans().y) {
				pointerTexture = crosshair3Texture;
				myAudioManager->PlayButtonClick();
				afterTransition = true;
			}
		}
	}
	if (diKeys[DIK_ESCAPE] & 0x80) {
		PostQuitMessage(0);
	}
}

void crosshairSelectionMenuSpriteRender(int frames) {
	if (frames > 1) {
		frames = 1;
	}

	for (int i = 0; i < frames; i++) {
		if (currentTransitionPos > 0) {
			currentTransitionPos -= beforeTransitionPos;
		}
		if (afterTransition) {
			currentTransitionPos -= afterTransitionPos;
		}
		if (currentTransitionPos <= -3000) {
			afterTransition = false;
			currentTransitionPos = 2000;
			currentMenu = GameMenu;
		}
	}

	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	//Sprite Transform Object - (scalingCenter, scalingRotation, scaling, rotationCenter, rotation, trans)
	cursorTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(currentXpos, currentYpos));
	bgTrans = SpriteTransform(D3DXVECTOR2(350, 256), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(50 + currentTransitionPos, 50));
	crosshairTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(240 + currentTransitionPos, 180));
	crosshair2Trans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(515 + currentTransitionPos, 180));
	crosshair3Trans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(790 + currentTransitionPos, 180));
	crosshairSelectionTrans = SpriteTransform(D3DXVECTOR2(100, 57.5), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(180 + currentTransitionPos, 300));
	crosshair2SelectionTrans = SpriteTransform(D3DXVECTOR2(100, 57.5), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(455 + currentTransitionPos, 300));
	crosshair3SelectionTrans = SpriteTransform(D3DXVECTOR2(100, 57.5), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(730 + currentTransitionPos, 300));

	textTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(350 + currentTransitionPos, 100));
	crosshairSelectionTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(230 + currentTransitionPos, 330));
	crosshair2SelectionTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(505 + currentTransitionPos, 330));
	crosshair3SelectionTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(780 + currentTransitionPos, 330));

	//Text
	textRect.left = 0;
	textRect.top = 0;
	textRect.right = 300;
	textRect.bottom = 125;

	bgTrans.transform();
	sprite->SetTransform(&bgTrans.getMat());
	sprite->Draw(bgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	textTrans.transform();
	sprite->SetTransform(&textTrans.getMat());
	font->DrawText(sprite, "Choose your crosshair", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	crosshairTrans.transform();
	sprite->SetTransform(&crosshairTrans.getMat());
	sprite->Draw(crosshairTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	crosshair2Trans.transform();
	sprite->SetTransform(&crosshair2Trans.getMat());
	sprite->Draw(crosshair2Texture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	crosshair3Trans.transform();
	sprite->SetTransform(&crosshair3Trans.getMat());
	sprite->Draw(crosshair3Texture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	crosshairSelectionTrans.transform();
	sprite->SetTransform(&crosshairSelectionTrans.getMat());
	sprite->Draw(buttonBgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	crosshair2SelectionTrans.transform();
	sprite->SetTransform(&crosshair2SelectionTrans.getMat());
	sprite->Draw(buttonBgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	crosshair3SelectionTrans.transform();
	sprite->SetTransform(&crosshair3SelectionTrans.getMat());
	sprite->Draw(buttonBgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	crosshairSelectionTextTrans.transform();
	sprite->SetTransform(&crosshairSelectionTextTrans.getMat());
	font->DrawText(sprite, "SELECT", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	crosshair2SelectionTextTrans.transform();
	sprite->SetTransform(&crosshair2SelectionTextTrans.getMat());
	font->DrawText(sprite, "SELECT", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	crosshair3SelectionTextTrans.transform();
	sprite->SetTransform(&crosshair3SelectionTextTrans.getMat());
	font->DrawText(sprite, "SELECT", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	cursorTrans.transform();
	sprite->SetTransform(&cursorTrans.getMat());
	sprite->Draw(cursorTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	sprite->End();
}


void crosshairSelectionMenuRender(int frames) {
	directStruct.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(red, green, blue), 1.0f, 0);

	directStruct.d3dDevice->BeginScene();

	crosshairSelectionMenuSpriteRender(frames);

	directStruct.d3dDevice->EndScene();

	directStruct.d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void gameOverMenuUpdate() {
	if (currentYpos >= 0 && currentYpos <= screenHeight - cursorSprite.getTotalSpriteHeight()) {
		currentYpos += mouseState.lY;
	}
	if (currentYpos < 0 || currentYpos > screenHeight - cursorSprite.getTotalSpriteHeight()) {
		currentYpos -= mouseState.lY;
	}
	if (currentXpos >= 0 && currentXpos <= screenWidth - cursorSprite.getTotalSpriteWidth()) {
		currentXpos += mouseState.lX;
	}
	if (currentXpos < 0 || currentXpos > screenWidth - cursorSprite.getTotalSpriteWidth()) {
		currentXpos -= mouseState.lX;
	}
	if (currentTransitionPos == 0 && afterTransition == false) {
		if (mouseState.rgbButtons[0] & 0x80) {
			if (cursorTrans.getTrans().x >= continueButtonTrans.getTrans().x && cursorTrans.getTrans().x <= continueButtonTrans.getTrans().x + buttonBgSprite.getTotalSpriteWidth() && cursorTrans.getTrans().y <= continueButtonTrans.getTrans().y + buttonBgSprite.getTotalSpriteHeight() && cursorTrans.getTrans().y >= continueButtonTrans.getTrans().y) {
				gameOverAction = Retry;
				myAudioManager->PlayButtonClick();
				afterTransition = true;
			}
		}
		if (mouseState.rgbButtons[0] & 0x80) {
			if (cursorTrans.getTrans().x >= exitButtonTrans.getTrans().x && cursorTrans.getTrans().x <= exitButtonTrans.getTrans().x + buttonBgSprite.getTotalSpriteWidth() && cursorTrans.getTrans().y <= exitButtonTrans.getTrans().y + buttonBgSprite.getTotalSpriteHeight() && cursorTrans.getTrans().y >= exitButtonTrans.getTrans().y) {
				gameOverAction = Exit;
				myAudioManager->PlayButtonClick();
				afterTransition = true;
			}
		}
	}
	if (diKeys[DIK_ESCAPE] & 0x80) {
		PostQuitMessage(0);
	}
}

void gameOverMenuSpriteRender(int frames) {
	if (frames > 1) {
		frames = 1;
	}
	for (int i = 0; i < frames; i++) {
		if (currentTransitionPos > 0) {
			currentTransitionPos -= beforeTransitionPos;
		}
		if (afterTransition) {
			currentTransitionPos -= afterTransitionPos;
		}
		if (currentTransitionPos <= -3000) {
			afterTransition = false;
			currentTransitionPos = 2000;
			if (gameOverAction == Retry) {
				resetStage();
				currentMenu = GameMenu;
			}
			if (gameOverAction == Exit) {
				resetStage();
				currentMenu = MainMenu;
			}
		}
	}
	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	bgTrans = SpriteTransform(D3DXVECTOR2(350, 256), 0, D3DXVECTOR2(0.7, 0.8), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(150 + currentTransitionPos, 150));
	textTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(3, 3), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(400 + currentTransitionPos, 270));
	continueButtonTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(340 + currentTransitionPos, 400));
	exitButtonTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(615 + currentTransitionPos, 400));
	cursorTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(currentXpos, currentYpos));

	continueTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(2, 2), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(390 + currentTransitionPos, 430));
	exitTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1.5, 1.5), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(645 + currentTransitionPos, 440));
	scoresTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(390 + currentTransitionPos, 350));
	timerTextTrans = SpriteTransform(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(530 + currentTransitionPos, 350));

	//Text
	textRect.left = 0;
	textRect.top = 0;
	textRect.right = 200;
	textRect.bottom = 125;

	bgTrans.transform();
	sprite->SetTransform(&bgTrans.getMat());
	sprite->Draw(bgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	continueButtonTrans.transform();
	sprite->SetTransform(&continueButtonTrans.getMat());
	sprite->Draw(buttonBgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	exitButtonTrans.transform();
	sprite->SetTransform(&exitButtonTrans.getMat());
	sprite->Draw(buttonBgTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	continueTextTrans.transform();
	sprite->SetTransform(&continueTextTrans.getMat());
	font->DrawText(sprite, "RETRY", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	exitTextTrans.transform();
	sprite->SetTransform(&exitTextTrans.getMat());
	font->DrawText(sprite, "MAIN MENU", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	textTrans.transform();
	sprite->SetTransform(&textTrans.getMat());
	font->DrawText(sprite, "YOU DIED", -1, &textRect, 0, D3DCOLOR_XRGB(0, 255, 255));

	//Draw Scores Font
	scoresTextTrans.transform();

	sprite->SetTransform(&scoresTextTrans.getMat());
	strScoresPostfix = to_string(scores);
	for (int i = 0; i < strScoresPrefix.length(); i++) {
		scoresText[i] = strScoresPrefix[i];
	}
	for (int i = 0; i < strScoresPostfix.length(); i++) {
		scoresText[i + strScoresPrefix.length()] = strScoresPostfix[i];
	}
	font->DrawText(sprite, scoresText, -1, &textRect, 0, D3DCOLOR_XRGB(255, 0, 255));

	for (int i = 0; i < sizeof(scoresText); i++) {
		scoresText[i] = ' ';
	}

	//Text
	textRect.left = 0;
	textRect.top = 0;
	textRect.right = 500;
	textRect.bottom = 125;
	//Draw Timer Font
	timerTextTrans.transform();

	sprite->SetTransform(&timerTextTrans.getMat());
	strWaveSec = to_string(waveSec);
	strWaveMin = to_string(waveMin);
	for (int i = 0; i < strSurvived.length(); i++) {
		survivedTimerText[i] = strSurvived[i];
	}
	totalTextLength += strSurvived.length();
	for (int i = 0; i < strWaveMin.length(); i++) {
		survivedTimerText[i + totalTextLength] = strWaveMin[i];
	}
	totalTextLength += strWaveMin.length();
	for (int i = 0; i < strMin.length(); i++) {
		survivedTimerText[i + totalTextLength] = strMin[i];
	}
	totalTextLength += strMin.length();
	for (int i = 0; i < strWaveSec.length(); i++) {
		survivedTimerText[i + totalTextLength] = strWaveSec[i];
	}
	totalTextLength += strWaveSec.length();
	for (int i = 0; i < strSec.length(); i++) {
		survivedTimerText[i + totalTextLength] = strSec[i];
	}
	font->DrawText(sprite, survivedTimerText, -1, &textRect, 0, D3DCOLOR_XRGB(255, 0, 255));

	totalTextLength = 0;

	for (int i = 0; i < sizeof(survivedTimerText); i++) {
		survivedTimerText[i] = ' ';
	}

	cursorTrans.transform();
	sprite->SetTransform(&cursorTrans.getMat());
	sprite->Draw(cursorTexture.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	sprite->End();
}

void gameOverMenuRender(int frames) {
	directStruct.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(red, green, blue), 1.0f, 0);

	directStruct.d3dDevice->BeginScene();

	gameOverMenuSpriteRender(frames);

	directStruct.d3dDevice->EndScene();

	directStruct.d3dDevice->Present(NULL, NULL, NULL, NULL);
}

int main()  //int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	srand(time(0));

	gameTimer->init(50);

	asteroidTimer->init(10);

	bulletTimer->init(bulletInterval);

	thrustTimer->init(4);

	waveTimer->init(1);

	splashTimer->init(1);

	transitionTimer->init(50);
	
	createSplashWindow();

	createSplashDirectX();

	createSplashSprite();

	while (splashCount < maxSplashCount && windowIsRunning()) {
		splashRender();
		updateSplash(splashTimer->FramesToUpdate());
	}

	DestroyWindow(wndStruct.g_hWnd2);
	cleanupSplashSprite();
	cleanupSplashDirectX();
	cleanupSplashWindow();
	
	createWindow();

	createDirectX();

	createSprite();

	createDirectInput();

	myAudioManager->InitializeAudio();
	myAudioManager->LoadSounds();
	myAudioManager->PlaySound1();

	while (windowIsRunning())
	{

		if (currentMenu == MainMenu) {
			getInput();
			mainMenuUpdate();
			Sound();
			mainMenuRender();
		}
		if (currentMenu == SpaceshipSelectionMenu) {
			getInput();
			spaceshipSelectionMenuUpdate();
			Sound();
			spaceshipSelectionMenuRender(transitionTimer->FramesToUpdate());
		}
		if (currentMenu == CrosshairSelectionMenu) {
			getInput();
			crosshairSelectionMenuUpdate();
			Sound();
			crosshairSelectionMenuRender(transitionTimer->FramesToUpdate());
		}
		if (currentMenu == GameOverMenu) {
			getInput();
			gameOverMenuUpdate();
			Sound();
			gameOverMenuRender(transitionTimer->FramesToUpdate());
		}
		if (currentMenu == GameMenu) {
			getInput();
			updateBullet(bulletTimer->FramesToUpdate());
			updateThrust(thrustTimer->FramesToUpdate());
			updateAsteroid(asteroidTimer->FramesToUpdate());
			updateWave(waveTimer->FramesToUpdate());
			update(gameTimer->FramesToUpdate());
			Sound();
			render();
		}
	}

	cleanupSprite();

	cleanupDirectX();

	cleanupWindow();

	cleanupInput();

	return 0;
}
