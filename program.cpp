//	Refer to Direct3D 9 documentation for the meaning of the members.
//https://learn.microsoft.com/en-us/windows/win32/direct3d9/dx9-graphics
//https://learn.microsoft.com/en-us/windows/win32/api/_direct3d9/

#include <d3d9.h>
#include <d3dx9.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <dinput.h>
#include <string>
#define PI 3.142

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
	int totalSpriteWidth;
	int totalSpriteHeight;
	int spriteWidth;
	int spriteHeight;
	int spriteRow;
	int spriteCol;
	int currentFrame;
	int maxFrame;
	RECT spriteRect;

public:
	SpriteSheet(int totalSpriteWidth, int totalSpriteHeight, int spriteRow, int spriteCol, int currentFrame, int maxFrame)
	{
		this->totalSpriteWidth = totalSpriteWidth;
		this->totalSpriteHeight = totalSpriteHeight;
		this->spriteRow = spriteRow;
		this->spriteCol = spriteCol;
		this->currentFrame = currentFrame;
		this->maxFrame = maxFrame;
		this->spriteWidth = totalSpriteWidth / spriteCol;
		this->spriteHeight = totalSpriteHeight / spriteRow;
		spriteRect.left = 0;
		spriteRect.right = 0;
		spriteRect.top = 0;
		spriteRect.bottom = 0;

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
	void nextFrame() {
		currentFrame++;
	}
	void prevFrame() {
		currentFrame--;
		if (currentFrame < 0) {
			currentFrame = maxFrame - 1;
		}
	}
	RECT& crop() {
		spriteRect.left = currentFrame % spriteCol * spriteWidth;
		spriteRect.right = currentFrame % spriteCol * spriteWidth + spriteWidth;
		spriteRect.top = currentFrame % maxFrame / spriteRow * spriteHeight;
		spriteRect.bottom = currentFrame % maxFrame / spriteRow * spriteHeight + spriteHeight;
		return spriteRect;
	}
};

//Sprite Transformation Class
class SpriteTransform {
private:
	D3DXVECTOR2 spriteCentre;
	D3DXVECTOR2 trans;
	D3DXMATRIX mat;
	D3DXVECTOR2 scaling;
public:
	SpriteTransform(D3DXVECTOR2 spriteCentre, D3DXVECTOR2 trans, D3DXVECTOR2 scaling) {
		this->spriteCentre = spriteCentre;
		this->trans = trans;
		this->scaling = scaling;
	}
	D3DXVECTOR2& getSpriteCentre() {
		return spriteCentre;
	}
	D3DXVECTOR2& getTrans() {
		return trans;
	}
	D3DXMATRIX& getMat() {
		return mat;
	}
	D3DXVECTOR2& getScaling() {
		return scaling;
	}
	void setSpriteCentre(D3DXVECTOR2 spriteCentre) {
		this->spriteCentre = spriteCentre;
	}
	void setTrans(D3DXVECTOR2 trans) {
		this->trans = trans;
	}
	void setMat(D3DXMATRIX mat) {
		this->mat = mat;
	}
	void setScaling(D3DXVECTOR2 scaling) {
		this->scaling = scaling;
	}
	void transform() {
		D3DXMatrixTransformation2D(&mat, NULL, 0.0, &scaling, &spriteCentre, NULL, &trans);
	}
};

//Texture Class
class Texture {
private:
	LPDIRECT3DTEXTURE9 texture;
	LPCTSTR fileLocation = "";
	int redKey = 0, greenKey = 0, blueKey = 0;
public:
	Texture(LPDIRECT3DTEXTURE9 texture, LPCTSTR fileLocation, int redKey, int greenKey, int blueKey) {
		this->texture = texture;
		this->fileLocation = fileLocation;
		this->redKey = redKey;
		this->greenKey = greenKey;
		this->blueKey = blueKey;
	}
	Texture(LPDIRECT3DTEXTURE9 texture, LPCTSTR fileLocation) {
		this->texture = texture;
		this->fileLocation = fileLocation;
	}
	Texture(LPDIRECT3DTEXTURE9 texture) {
		this->texture = texture;
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

//pointer to a texture file - (texture, file location, red filter key, green filter key, blue filter key)
Texture bg1(NULL, "Assets/bg1.png");
Texture bg2(NULL, "Assets/bg2.png");
Texture bg3(NULL, "Assets/bg3.png");
Texture currentbg(NULL);
Texture pointer(NULL, "Assets/pointer.png");
Texture numTexture(NULL, "Assets/numbers.bmp", 0, 128, 0);
Texture explosion(NULL, "Assets/explosion.png");

//pointer to a sprite interface 
LPD3DXSPRITE sprite = NULL;
RECT spriteRect;

//pointer to font interface
LPD3DXFONT font = NULL;
RECT textRect;

//pointer to line interface
LPD3DXLINE line = NULL;

//Screen Resolution
int screenWidth = 1280;
int screenHeight = 720;

//Sprite Sheet Object - (totalWidth, totalHeight, row, col, currentFrame, maxFrame)
SpriteSheet numSprite(128, 128, 4, 4, 0, 16);
SpriteSheet explosionSprite(2048, 2048, 8, 8, 0, 64);

// x and y position of the mouse
int x;
int y;

//Default value for rgb color
int red = 0;
int green = 0;
int blue = 0;

//The value to inc/dec rgb color
int redInc = 1;
int greenInc = 1;
int blueInc = 1;

//RGB color for pointer sprite
int redPointer = 255;
int greenPointer = 255;
int bluePointer = 255;

int seq;

//Starting point for the line
double secDegree = 270;
double minDegree = 270;
double hrDegree = 270;
//Radius for the circle
double secRadius = 110;
double minRadius = 80;
double hrRadius = 50;
//Circle center value
double circleXCenter = 500;
double circleYCenter = 500;
//Clock measurement gap between the secLine
double measureGap = 30;
//Clock measurement length
double measureLength = 20;
//Clock validation
bool hrBegin = false;

//Input 
LPDIRECTINPUT8 dInput;
LPDIRECTINPUTDEVICE8  dInputKeyboardDevice;
LPDIRECTINPUTDEVICE8 dInputMouseDevice;
DIMOUSESTATE mouseState;

LONG currentXpos = 500;
LONG currentYpos = 500;

//	Key input buffer
BYTE  diKeys[256];

void changeCursorColor() {
	switch (seq) {
	case 1:
		redPointer = 255;
		greenPointer = 0;
		bluePointer = 0;
		break;
	case 2:
		redPointer = 0;
		greenPointer = 255;
		bluePointer = 0;
		break;
	case 3:
		redPointer = 0;
		greenPointer = 0;
		bluePointer = 255;
		break;
	case 4:
		redPointer = 0;
		greenPointer = 0;
		bluePointer = 0;
		break;
	default:
		redPointer = 255;
		greenPointer = 255;
		bluePointer = 255;
		break;
	}
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
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
	ShowCursor(false);
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

	//Sprite Transform Object - (Center, Translation, Scaling)
	SpriteTransform bgTrans(D3DXVECTOR2(256, 256), D3DXVECTOR2(0, 0), D3DXVECTOR2(3.2, 2.4));
	SpriteTransform numTrans(D3DXVECTOR2(64, 64), D3DXVECTOR2(30, 30), D3DXVECTOR2(1, 1));
	SpriteTransform pointerTrans(D3DXVECTOR2(0, 0), D3DXVECTOR2(currentXpos, currentYpos), D3DXVECTOR2(1, 1));
	SpriteTransform explosionTrans(D3DXVECTOR2(1024, 1024), D3DXVECTOR2(200, 200), D3DXVECTOR2(1, 1));
	SpriteTransform textTrans(D3DXVECTOR2(0, 0), D3DXVECTOR2(800, 100), D3DXVECTOR2(1, 1));

	//Background
	spriteRect.left = 0;
	spriteRect.right = 400;
	spriteRect.top = 0;
	spriteRect.bottom = 300;

	//Text
	textRect.left = 100;
	textRect.top = 100;
	textRect.right = 300;
	textRect.bottom = 125;

	bgTrans.transform();

	sprite->SetTransform(&bgTrans.getMat());

	sprite->Draw(currentbg.getTexture(), &spriteRect, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Numbers
	numTrans.transform();

	sprite->SetTransform(&numTrans.getMat());

	sprite->Draw(numTexture.getTexture(), &numSprite.crop(), NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Pointer
	pointerTrans.transform();

	sprite->SetTransform(&pointerTrans.getMat());

	sprite->Draw(pointer.getTexture(), NULL, NULL, NULL, D3DCOLOR_XRGB(redPointer, greenPointer, bluePointer));

	//Explosion
	explosionTrans.transform();

	sprite->SetTransform(&explosionTrans.getMat());

	sprite->Draw(explosion.getTexture(), &explosionSprite.crop(), NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Draw Font
	textTrans.transform();

	sprite->SetTransform(&textTrans.getMat());
	font->DrawText(sprite, "Hello World!", -1, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

	sprite->End();
}

void lineRender() {
	//Rendering clock
	if (secDegree >= 360) {
		secDegree = 0;
	}
	if (minDegree >= 360) {
		minDegree = 0;
		hrBegin = true;
	}
	if (hrDegree >= 360) {
		hrDegree = 0;
	}
	secDegree++;
	double secRad = PI / 180 * secDegree;
	double minRad = PI / 180 * minDegree;
	double hrRad = PI / 180 * hrDegree;
	double measurement = PI / 180 * (360 / 12);

	double secX = cos(secRad) * secRadius + circleXCenter;
	double secY = sin(secRad) * secRadius + circleYCenter;
	double minX = cos(minRad) * minRadius + circleXCenter;
	double minY = sin(minRad) * minRadius + circleYCenter;
	double hrX = cos(hrRad) * hrRadius + circleXCenter;
	double hrY = sin(hrRad) * hrRadius + circleYCenter;
	for (int i = 0; i < 12; i++) {
		double measureXCenter = cos(measurement * i) * (secRadius + measureGap) + circleXCenter;
		double measureYCenter = sin(measurement * i) * (secRadius + measureGap) + circleYCenter;

		double measureX = cos(measurement * i) * measureLength + measureXCenter;
		double measureY = sin(measurement * i) * measureLength + measureYCenter;
		D3DXVECTOR2 measureLineVertices[] = { D3DXVECTOR2(measureXCenter, measureYCenter), D3DXVECTOR2(measureX, measureY) };
		line->Begin();
		line->Draw(measureLineVertices, 2, D3DCOLOR_XRGB(0, 0, 0));
		line->End();
	}
	//Draw Line
	D3DXVECTOR2 secLineVertices[] = { D3DXVECTOR2(circleXCenter,circleYCenter), D3DXVECTOR2(secX,secY) };
	D3DXVECTOR2 minLineVertices[] = { D3DXVECTOR2(circleXCenter,circleYCenter), D3DXVECTOR2(minX,minY) };
	D3DXVECTOR2 hrLineVertices[] = { D3DXVECTOR2(circleXCenter,circleYCenter), D3DXVECTOR2(hrX,hrY) };


	line->Begin();
	line->Draw(secLineVertices, 2, D3DCOLOR_XRGB(255, 0, 0));
	line->Draw(minLineVertices, 2, D3DCOLOR_XRGB(0, 255, 0));
	line->Draw(hrLineVertices, 2, D3DCOLOR_XRGB(0, 0, 255));


	line->End();
	if (secDegree == 270) {
		minDegree += 10;
	}
	if (minDegree == 270 && hrBegin) {
		hrDegree += 10;
		hrBegin = false;
	}
}

void render() {

	directStruct.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(red, green, blue), 1.0f, 0);

	directStruct.d3dDevice->BeginScene();

	spriteRender();

	lineRender();

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

	hr = D3DXCreateLine(directStruct.d3dDevice, &line);

	if (FAILED(hr)) {
		cout << "Create Line Failed!!!";
	}

	hr = bg1.createTextureFromFile();
	hr = bg2.createTextureFromFile();
	hr = bg3.createTextureFromFile();
	currentbg.setTexture(bg1.getTexture());

	hr = pointer.createTextureFromFile();
	hr = explosion.createTextureFromFile();

	hr = numTexture.createTextureFromFileEx();

	if (FAILED(hr)) {
		cout << "Create Texture from File Failed!!!";
	}


}


void cleanupSprite() {
	sprite->Release();
	sprite = NULL;

	currentbg.getTexture()->Release();
	currentbg = NULL;

	pointer.getTexture()->Release();
	pointer = NULL;

	numTexture.getTexture()->Release();
	numTexture = NULL;

	explosion.getTexture()->Release();
	explosion = NULL;

	font->Release();
	font = NULL;

	line->Release();
	line = NULL;
}

void createDirectInput() {
	//	Create the Direct Input object.
	HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), 0x0800, IID_IDirectInput8, (void**)&dInput, NULL);

	//	Create the keyboard device.
	hr = dInput->CreateDevice(GUID_SysKeyboard, &dInputKeyboardDevice, NULL);

	//	Set the input data format.
	dInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);

	//	Set the cooperative level.
	//	To Do:
	//	Try with different combination.
	dInputKeyboardDevice->SetCooperativeLevel(wndStruct.g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	// Create the mouse device
	hr = dInput->CreateDevice(GUID_SysMouse, &dInputMouseDevice, NULL);

	// Set the input data format
	dInputMouseDevice->SetDataFormat(&c_dfDIMouse);

	dInputMouseDevice->SetCooperativeLevel(wndStruct.g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
}

void getInput() {
	dInputKeyboardDevice->Acquire();
	dInputMouseDevice->Acquire();
	dInputKeyboardDevice->GetDeviceState(256, diKeys);

	dInputMouseDevice->GetDeviceState(sizeof(mouseState), (LPVOID)&mouseState);
}

void update() {
	if (diKeys[DIK_ESCAPE] & 0x80) {
		PostQuitMessage(0);
	}
	// Sprite animation
	if (diKeys[DIK_UP] & 0x80)
	{
		cout << "UP" << endl;
		numSprite.nextFrame();
		explosionSprite.nextFrame();
	}
	if (diKeys[DIK_DOWN] & 0x80) {
		cout << "DOWN" << endl;
		numSprite.prevFrame();
		explosionSprite.prevFrame();
	}
	// Change background
	if (diKeys[DIK_1] & 0x80) {
		currentbg = bg1;
	}
	if (diKeys[DIK_2] & 0x80) {
		currentbg = bg2;
	}
	if (diKeys[DIK_3] & 0x80) {
		currentbg = bg3;
	}
	// Change cursor color
	if (diKeys[DIK_LEFT] & 0x80) {
		seq++;
		if (seq > 4) {
			seq = 0;
		}
		changeCursorColor();
	}
	if (diKeys[DIK_RIGHT] & 0x80) {
		seq--;
		if (seq < 0) {
			seq = 4;
		}
		changeCursorColor();
	}
	//Press R to increment/decrement red color
	if (diKeys[DIK_R] & 0x80) {
		red += redInc;
		if (red <= 0 || red >= 255) {
			redInc *= -1;
		}
		if (red > 255 || red < 0) {
			red += redInc + redInc;
		}
		cout << "RGB(" << red << ", " << green << ", " << blue << ")" << endl;
	}
	//Press G to increment/decrement green color
	if (diKeys[DIK_G] & 0x80) {
		green += greenInc;
		if (green <= 0 || green >= 255) {
			greenInc *= -1;
		}
		if (green > 255 || green < 0) {
			green += greenInc + greenInc;
		}
		cout << "RGB(" << red << ", " << green << ", " << blue << ")" << endl;
	}
	//Press B to increment/decrement blue color
	if (diKeys[DIK_B] & 0x80) {
		blue += blueInc;
		if (blue <= 0 || blue >= 255) {
			blueInc *= -1;
		}
		if (blue > 255 || blue < 0) {
			blue += blueInc + blueInc;
		}
		cout << "RGB(" << red << ", " << green << ", " << blue << ")" << endl;
	}
	//Left click
	if (mouseState.rgbButtons[0] & 0x80) {
		// do something
	}
	//Right click
	if (mouseState.rgbButtons[1] & 0x80) {
		//do something
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

int main()  //int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	createWindow();

	createDirectX();

	createSprite();

	createDirectInput();

	while (windowIsRunning())
	{
		getInput();
		//Physics();
		update();
		render();
		//Sound();

	}

	cleanupSprite();

	cleanupDirectX();

	cleanupWindow();

	cleanupInput();

	return 0;
}
