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
#include "FrameTimer.h"
#define WIN32_LEAN_AND_MEAN

using namespace std;

//Window Structure
struct {
	WNDCLASS wndClass;
	HWND g_hWnd = NULL;
	MSG msg;
} wndStruct;

//Direct3D Structure
struct {
	D3DPRESENT_PARAMETERS d3dPP;
	IDirect3D9* direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	IDirect3DDevice9* d3dDevice;
} directStruct;

//SpriteSheet Class
class SpriteSheet
{
private:
	int totalSpriteWidth = NULL;
	int totalSpriteHeight = NULL;
	int spriteWidth = NULL;
	int spriteHeight = NULL;
	int spriteRow = NULL;
	int spriteCol = NULL;
	int currentFrame = NULL;
	int maxFrame = NULL;
	RECT spriteRect;
	int currentMovement = NULL;

public:
	SpriteSheet(int totalSpriteWidth, int totalSpriteHeight, int spriteRow, int spriteCol, int currentFrame, int maxFrame)
	{
		this->totalSpriteWidth = totalSpriteWidth;
		this->totalSpriteHeight = totalSpriteHeight;
		this->spriteRow = spriteRow;
		this->spriteCol = spriteCol;
		this->currentFrame = currentFrame-1;
		this->maxFrame = maxFrame;
		this->spriteWidth = totalSpriteWidth / spriteCol;
		this->spriteHeight = totalSpriteHeight / spriteRow;
		spriteRect.left = 0;
		spriteRect.right = 0;
		spriteRect.top = 0;
		spriteRect.bottom = 0;
	}
	SpriteSheet(int spriteRectLeft, int spriteRectRight, int spriteRectTop, int spriteRectBottom) {
		spriteRect.left = spriteRectLeft;
		spriteRect.right = spriteRectRight;
		spriteRect.top = spriteRectTop;
		spriteRect.bottom = spriteRectBottom;
	}
	int getTotalSpriteWidth() {
		return totalSpriteWidth;
	}
	int getTotalSpriteHeight() {
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
	int getSpriteWidth() {
		return spriteWidth;
	}
	int getSpriteHeight() {
		return spriteHeight;
	}
	int getCurrentMovement() {
		return currentMovement;
	}
	RECT& getRect() {
		return spriteRect;
	}
	void setCurrentMovement(int currentMovement) {
		this->currentMovement = currentMovement;
	}
	void setCurrentFrame(int currentFrame) {
		this->currentFrame = currentFrame-1;
	}
	void nextFrame() {
		currentFrame+=2;
	}
	void prevFrame() {
		currentFrame--;
		if (currentFrame < 0) {
			currentFrame = maxFrame - 1;
		}
	}
	RECT& crop() {
		spriteRect.left = currentFrame % spriteCol * spriteWidth;
		spriteRect.right = spriteRect.left + spriteWidth;
		spriteRect.top = currentFrame % maxFrame / spriteRow * spriteHeight;
		spriteRect.bottom = spriteRect.top + spriteHeight;
		return spriteRect;
	}
	RECT& movementCrop() {
		spriteRect.left = currentFrame % spriteCol * spriteWidth;
		spriteRect.right = spriteRect.left + spriteWidth;
		spriteRect.top = currentMovement * spriteHeight;
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

public:
	SpriteTransform(D3DXVECTOR2 scalingCenter, float scalingRotation, D3DXVECTOR2 scaling, D3DXVECTOR2 rotationCenter, float rotation, D3DXVECTOR2 trans) {
		this->scalingCenter = scalingCenter;
		this->scalingRotation = scalingRotation;
		this->scaling = scaling;
		this->rotationCenter = rotationCenter;
		this->rotation = rotation;
		this->trans = trans;
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
	D3DXVECTOR2& getrotationCenter() {
		return rotationCenter;
	}
	float getRotation() {
		return rotation;
	}
	D3DXVECTOR2& getTrans() {
		return trans;
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
	void transform() {
		D3DXMatrixTransformation2D(&mat, &scalingCenter, scalingRotation, &scaling, &rotationCenter, rotation, &trans);
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
};

//Textures
Texture pointer("Assets/pointer.png");
Texture spaceship("Assets/practical9.png");
Texture spaceship2("Assets/practical9.png");

//Spritesheet
//Sprite Sheet Object - (totalWidth, totalHeight, row, col, currentFrame, maxFrame)
//                    - (spriteRectLeft, spriteRectRight, spriteRectTop, spriteRectBottom)
SpriteSheet spaceshipSprite(64, 64, 2, 2, 1, 4);
SpriteSheet spaceshipSprite2(64, 64, 2, 2, 2, 4);


//Default value for rgb color
int red = 0;
int green = 0;
int blue = 0;

//pointer to a sprite interface 
LPD3DXSPRITE sprite = NULL;

//pointer to font interface
LPD3DXFONT font = NULL;
RECT textRect;

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

//	Key input buffer
BYTE  diKeys[256];

// FrameTimer Object
FrameTimer* gameTimer = new FrameTimer();

//PI
float PI = 3.142;

//Friction
float friction = 0.01; //(1,1) = no friction

//Normalize
D3DXVECTOR2 normalized;

//Spaceship position
D3DXVECTOR2 spaceshipPosition(100, 300);
D3DXVECTOR2 spaceshipOldPosition;
//Spaceship rotation
float spaceshipRotation = 0;
//Spaceship Physics
D3DXVECTOR2 spaceshipVelocity;
D3DXVECTOR2 spaceshipAcceleration;
D3DXVECTOR2 spaceshipEngineForce;
float spaceshipEnginePower = 30;
float spaceshipMass = 100;

//Spaceship2 position
D3DXVECTOR2 spaceship2Position(800, 300);
D3DXVECTOR2 spaceship2OldPosition;
//Spaceship2 rotation
float spaceship2Rotation = 0;
//Spaceship2 Physics
D3DXVECTOR2 spaceship2Velocity;
D3DXVECTOR2 spaceship2Acceleration;
D3DXVECTOR2 spaceship2EngineForce;
float spaceship2EnginePower = 40;
float spaceship2Mass = 500;

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
		default:
			break;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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

	wndStruct.g_hWnd = CreateWindowEx(0, wndStruct.wndClass.lpszClassName, "Project A (Press <-/-> to switch the color of the cursor)", WS_OVERLAPPEDWINDOW, 0, 100, screenWidth, screenHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
	ShowCursor(true);
	ShowWindow(wndStruct.g_hWnd, 1);
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

void spriteRender() {
	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	//Sprite Transform Object - (scalingCenter, scalingRotation, scaling, rotationCenter, rotation, trans)
	SpriteTransform pointerTrans(D3DXVECTOR2(0,0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0,0), 0, D3DXVECTOR2(currentXpos, currentYpos));
	SpriteTransform spaceshipTrans(D3DXVECTOR2(spaceshipSprite.getSpriteWidth() / 2, spaceshipSprite.getSpriteHeight() / 2), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(spaceshipSprite.getSpriteWidth() / 2, spaceshipSprite.getSpriteHeight() / 2), spaceshipRotation, spaceshipPosition);
	SpriteTransform spaceship2Trans(D3DXVECTOR2(spaceshipSprite2.getSpriteWidth() / 2, spaceshipSprite2.getSpriteHeight() / 2), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(spaceshipSprite2.getSpriteWidth() / 2, spaceshipSprite2.getSpriteHeight() / 2), spaceship2Rotation, spaceship2Position);
	SpriteTransform textTrans(D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0), 0, D3DXVECTOR2(800,100));

	//Text
	textRect.left = 0;
	textRect.top = 0;
	textRect.right = 300;
	textRect.bottom = 125;

	//Pointer
	pointerTrans.transform();

	sprite->SetTransform(&pointerTrans.getMat());

	sprite->Draw(pointer.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Spaceship
	spaceshipTrans.transform();

	sprite->SetTransform(&spaceshipTrans.getMat());

	sprite->Draw(spaceship.getTexture(), &spaceshipSprite.crop(), NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Spaceship2
	spaceship2Trans.transform();

	sprite->SetTransform(&spaceship2Trans.getMat());

	sprite->Draw(spaceship2.getTexture(), &spaceshipSprite2.crop(), NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Draw Mouse Position Font
	textTrans.transform();

	sprite->SetTransform(&textTrans.getMat());
	strCurrentXpos = to_string(currentXpos);
	strCurrentYpos = to_string(currentYpos);
	for (int i = 0; i < strCurrentXpos.length(); i++) {
		posText[i] = strCurrentXpos[i];
	}
	posText[4] = ',';
	for (int i = 0; i < strCurrentYpos.length(); i++) {
		posText[i + 5] = strCurrentYpos[i];
	}

	font->DrawText(sprite, posText, -1, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	for (int i = 0; i < sizeof(posText); i++) {
		posText[i] = ' ';
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

void cleanupDirectX() {
	directStruct.d3dDevice->Release();
	directStruct.d3dDevice = NULL;
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

	hr = pointer.createTextureFromFile();
	hr = spaceship.createTextureFromFile();
	hr = spaceship2.createTextureFromFile();

	if (FAILED(hr)) {
		cout << "Create Texture from File Failed!!!";
	}
}

void cleanupSprite() {
	sprite->Release();
	sprite = NULL;

	pointer.getTexture()->Release();
	pointer = NULL;

	spaceship.getTexture()->Release();
	spaceship = NULL;

	font->Release();
	font = NULL;
}

void createDirectInput() {

	HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), 0x0800, IID_IDirectInput8, (void**)&dInput, NULL);

	hr = dInput->CreateDevice(GUID_SysKeyboard, &dInputKeyboardDevice, NULL);

	dInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);

	dInputKeyboardDevice->SetCooperativeLevel(wndStruct.g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = dInput->CreateDevice(GUID_SysMouse, &dInputMouseDevice, NULL);

	dInputMouseDevice->SetDataFormat(&c_dfDIMouse);

	dInputMouseDevice->SetCooperativeLevel(wndStruct.g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
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

void update(int frames) {
	for (int i = 0; i < frames; i++)
	{
		// Sprite animation
		if (diKeys[DIK_A] & 0x80) {
			spaceshipRotation -= 0.1;
		}
		if (diKeys[DIK_D] & 0x80) {
			spaceshipRotation += 0.1;
		}
		if (diKeys[DIK_LEFT] & 0x80) {
			spaceship2Rotation -= 0.1;
		}
		if (diKeys[DIK_RIGHT] & 0x80) {
			spaceship2Rotation += 0.1;
		}
		if (diKeys[DIK_UP] & 0x80) {
			spaceship2EngineForce.x = sin(spaceship2Rotation) * spaceship2EnginePower;
			spaceship2EngineForce.y = -cos(spaceship2Rotation) * spaceship2EnginePower;
			spaceship2Acceleration = spaceship2EngineForce / spaceship2Mass;
		}
		if (diKeys[DIK_SPACE] & 0x80) {
			spaceshipEngineForce.x = sin(spaceshipRotation) * spaceshipEnginePower;
			spaceshipEngineForce.y = -cos(spaceshipRotation) * spaceshipEnginePower;
			spaceshipAcceleration = spaceshipEngineForce / spaceshipMass;
		}
		spaceshipOldPosition = spaceshipPosition;
		spaceship2OldPosition = spaceship2Position;

		spaceshipVelocity += spaceshipAcceleration;
		spaceshipVelocity *= (1 - friction);
		spaceshipPosition += spaceshipVelocity;

		spaceship2Velocity += spaceship2Acceleration;
		spaceship2Velocity *= (1 - friction);
		spaceship2Position += spaceship2Velocity;
		//Spaceship right checking
		if (spaceshipPosition.x > screenWidth - spaceshipSprite.getSpriteWidth()) {
			spaceshipPosition.x = screenWidth - spaceshipSprite.getSpriteWidth();
			spaceshipRotation = 2 * PI - spaceshipRotation;
			spaceshipVelocity.x *= -1;
		}
		//Spaceship left checking
		if (spaceshipPosition.x < 0) {
			spaceshipPosition.x = 0;
			spaceshipRotation = 2 * PI - spaceshipRotation;
			spaceshipVelocity.x *= -1;
		}
		//Spaceship top checking
		if (spaceshipPosition.y < 0) {
			spaceshipPosition.y = 0;
			spaceshipRotation = PI - spaceshipRotation;
			spaceshipVelocity.y *= -1;
		}
		//Spaceship bottom checking
		if (spaceshipPosition.y > screenHeight - spaceshipSprite.getSpriteHeight()) {
			spaceshipPosition.y = screenHeight - spaceshipSprite.getSpriteHeight();
			spaceshipRotation = PI - spaceshipRotation;
			spaceshipVelocity.y *= -1;
		}
		//Spaceship2 right checking
		if (spaceship2Position.x > screenWidth - spaceshipSprite2.getSpriteWidth()) {
			spaceship2Position.x = screenWidth - spaceshipSprite2.getSpriteWidth();
			spaceship2Rotation = 2 * PI - spaceship2Rotation;
			spaceship2Velocity.x *= -1;
		}
		//Spaceship2 left checking
		if (spaceship2Position.x < 0) {
			spaceship2Position.x = 0;
			spaceship2Rotation = 2 * PI - spaceship2Rotation;
			spaceship2Velocity.x *= -1;
		}
		//Spaceship2 top checking
		if (spaceship2Position.y < 0) {
			spaceship2Position.y = 0;
			spaceship2Rotation = PI - spaceship2Rotation;
			spaceship2Velocity.y *= -1;
		}
		//Spaceship2 bottom checking
		if (spaceship2Position.y > screenHeight - spaceshipSprite2.getSpriteHeight()) {
			spaceship2Position.y = screenHeight - spaceshipSprite2.getSpriteHeight();
			spaceship2Rotation = PI - spaceship2Rotation;
			spaceship2Velocity.y *= -1;
		}
		//Collision Detection
		if (spaceshipPosition.x + spaceshipSprite.getSpriteWidth() >= spaceship2Position.x && spaceshipPosition.x <= spaceship2Position.x + spaceshipSprite2.getSpriteWidth() && spaceshipPosition.y <= spaceship2Position.y + spaceshipSprite2.getSpriteHeight() && spaceshipPosition.y + spaceshipSprite.getSpriteHeight() >= spaceship2Position.y) {

			//D3DXVec2Normalize(&normalized, &spaceshipVelocity);
			spaceship2Velocity = (2 * spaceshipMass * spaceshipVelocity) / (spaceshipMass + spaceship2Mass) - (spaceshipMass * spaceship2Velocity - spaceship2Mass * spaceship2Velocity) / (spaceshipMass + spaceship2Mass);
			spaceshipVelocity = (spaceshipMass * spaceshipVelocity - spaceship2Mass * spaceshipVelocity) / (spaceshipMass + spaceship2Mass) + (2 * spaceship2Mass * spaceship2Velocity) / (spaceshipMass + spaceship2Mass);

			spaceshipPosition = spaceshipOldPosition;
			spaceship2Position = spaceship2OldPosition;
			spaceshipPosition += spaceshipVelocity;
			spaceship2Position += spaceship2Velocity;
			if (spaceshipPosition.x > screenWidth - spaceshipSprite.getSpriteWidth() || spaceshipPosition.x < 0 || spaceshipPosition.y < 0 || spaceshipPosition.y > screenHeight - spaceshipSprite.getSpriteHeight() || spaceship2Position.x > screenWidth - spaceshipSprite2.getSpriteWidth() || spaceship2Position.x < 0 || spaceship2Position.y < 0 || spaceship2Position.y > screenHeight - spaceshipSprite2.getSpriteHeight()) {
				spaceshipPosition = spaceshipOldPosition;
				spaceship2Position = spaceship2OldPosition;
			}
		}

		spaceshipAcceleration = D3DXVECTOR2(0, 0);
		spaceship2Acceleration = D3DXVECTOR2(0, 0);
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
	if (currentYpos >= 0 || currentYpos <= screenHeight) {
		currentYpos += mouseState.lY;
	}
	if (currentYpos < 0 || currentYpos > screenHeight) {
		currentYpos -= mouseState.lY;
	}
	if (currentXpos >= 0 || currentXpos <= screenWidth) {
		currentXpos += mouseState.lX;
	}
	if (currentXpos < 0 || currentXpos > screenWidth) {
		currentXpos -= mouseState.lX;
	}
}
	

int main()  //int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	gameTimer->init(50);

	createWindow();

	createDirectX();

	createSprite();

	createDirectInput();


	while (windowIsRunning())
	{
		getInput();
		//Physics();
		update(gameTimer->FramesToUpdate());
		render();
		//Sound();

	}

	cleanupSprite();

	cleanupDirectX();

	cleanupWindow();

	cleanupInput();

	return 0;
}
