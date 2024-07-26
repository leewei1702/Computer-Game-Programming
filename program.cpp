//	Refer to Direct3D 9 documentation for the meaning of the members.
//https://learn.microsoft.com/en-us/windows/win32/direct3d9/dx9-graphics
//https://learn.microsoft.com/en-us/windows/win32/api/_direct3d9/

#include <d3d9.h>
#include <d3dx9.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
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
		if (currentFrame >= maxFrame) {
			currentFrame = 0;
		}
	}
	void prevFrame() {
		currentFrame--;
		if (currentFrame < 0) {
			currentFrame = maxFrame - 1;
		}
	}
	int getRectLeft() {
		return currentFrame % spriteCol * spriteWidth;
	}
	int getRectRight() {
		return currentFrame % spriteCol * spriteWidth + spriteWidth;
	}
	int getRectTop() {
		return currentFrame / spriteRow * spriteHeight;
	}
	int getRectBottom() {
		return currentFrame / spriteRow * spriteHeight + spriteHeight;
	}
};

//pointer to a texture file
LPDIRECT3DTEXTURE9 bg1 = NULL;
LPDIRECT3DTEXTURE9 bg2 = NULL;
LPDIRECT3DTEXTURE9 bg3 = NULL;
LPDIRECT3DTEXTURE9 currentbg = NULL;
LPDIRECT3DTEXTURE9 pointer = NULL;
LPDIRECT3DTEXTURE9 numTexture = NULL;
//pointer to a sprite interface 
LPD3DXSPRITE sprite = NULL;
RECT spriteRect;
RECT numRect;

//Sprite Transformation
D3DXVECTOR2 spriteCentre;
D3DXVECTOR2 trans;
D3DXMATRIX mat;
D3DXVECTOR2 scaling;

//Screen Resolution
int screenWidth = 1280;
int screenHeight = 720;

//Sprite Sheet
SpriteSheet numSprite(128,128,4,4,0,16);

//Default value for rgb color
int red = 0;
int green = 0;
int blue = 0;

// x and y position of the mouse
int x;
int y;

//The value to inc/dec rgb color
int redInc = 1;
int greenInc = 1;
int blueInc = 1;

//RGB color for pointer sprite
int redPointer = 255;
int greenPointer = 255;
int bluePointer = 255;

int seq;

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
	case WM_KEYDOWN:

		switch (wParam)
		{
			//Press ESC key to exit the program
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
			//Press X key to exit the program
		case 'x':
		case 'X':
			PostQuitMessage(0);
			break;
			//Press R to increment/decrement red color
		case 'R':
		case 'r':
			red += redInc;
			if (red <= 0 || red >= 255) {
				redInc *= -1;
			}
			if (red > 255 || red < 0) {
				red += redInc + redInc;
			}
			cout << "RGB(" << red << ", " << green << ", " << blue << ")" << endl;
			break;
			//Press G to increment/decrement green color
		case 'G':
		case 'g':
			green += greenInc;
			if (green <= 0 || green >= 255) {
				greenInc *= -1;
			}
			if (green > 255 || green < 0) {
				green += greenInc + greenInc;
			}
			cout << "RGB(" << red << ", " << green << ", " << blue << ")" << endl;
			break;
			//Press B to increment/decrement blue color
		case 'B':
		case 'b':
			blue += blueInc;
			if (blue <= 0 || blue >= 255) {
				blueInc *= -1;
			}
			if (blue > 255 || blue < 0) {
				blue += blueInc + blueInc;
			}
			cout << "RGB(" << red << ", " << green << ", " << blue << ")" << endl;
			break;
			// Swapping background
		case '1':
			currentbg = bg1;
			break;
		case '2':
			currentbg = bg2;
			break;
		case '3':
			currentbg = bg3;
			break;
			// <-/-> change the color of cursor
		case VK_LEFT:
			seq++;
			if (seq > 4) {
				seq = 0;
			}
			changeCursorColor();
			break;
		case VK_RIGHT:
			seq--;
			if (seq < 0) {
				seq = 4;
			}
			changeCursorColor();
			break;
		case VK_UP:
			numSprite.nextFrame();
			break;
		case VK_DOWN:
			numSprite.prevFrame();
			break;
		}
		break;
	case WM_MOUSEMOVE:
	{
		x = (short)LOWORD(lParam);
		y = (short)HIWORD(lParam);
	}
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

	//Background
	spriteCentre = D3DXVECTOR2(256, 256);
	trans = D3DXVECTOR2(0, 0);
	mat;
	scaling = D3DXVECTOR2(3.2, 2.4);

	D3DXMatrixTransformation2D(&mat, NULL, 0.0, &scaling, &spriteCentre, NULL, &trans);

	sprite->SetTransform(&mat);

	sprite->Draw(currentbg, &spriteRect, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Numbers
	numRect.left = numSprite.getRectLeft();
	numRect.right = numSprite.getRectRight();
	numRect.top = numSprite.getRectTop();
	numRect.bottom = numSprite.getRectBottom();

	spriteCentre = D3DXVECTOR2(64, 64);
	trans = D3DXVECTOR2(30, 30);
	mat;
	scaling = D3DXVECTOR2(1, 1);

	D3DXMatrixTransformation2D(&mat, NULL, 0.0, &scaling, &spriteCentre, NULL, &trans);

	sprite->SetTransform(&mat);

	sprite->Draw(numTexture, &numRect, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Pointer
	spriteCentre = D3DXVECTOR2(0, 0);
	trans = D3DXVECTOR2(x, y);
	mat;
	scaling = D3DXVECTOR2(1.0, 1.0);

	D3DXMatrixTransformation2D(&mat, NULL, 0.0, &scaling, &spriteCentre, NULL, &trans);

	sprite->SetTransform(&mat);

	sprite->Draw(pointer, NULL, NULL, NULL, D3DCOLOR_XRGB(redPointer, greenPointer, bluePointer));

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

	hr = D3DXCreateTextureFromFile(directStruct.d3dDevice, "Assets/bg1.png", &bg1);
	hr = D3DXCreateTextureFromFile(directStruct.d3dDevice, "Assets/bg2.png", &bg2);
	hr = D3DXCreateTextureFromFile(directStruct.d3dDevice, "Assets/bg3.png", &bg3);
	currentbg = bg1;

	hr = D3DXCreateTextureFromFile(directStruct.d3dDevice, "Assets/pointer.png", &pointer);

	hr = D3DXCreateTextureFromFileEx(directStruct.d3dDevice, "Assets/numbers.bmp", D3DX_DEFAULT, D3DX_DEFAULT,
										D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
										D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 128, 0), 
										NULL, NULL, &numTexture);

	if (FAILED(hr)) {
		cout << "Create Texture from File Failed!!!";
	}

	spriteRect.left = 0;
	spriteRect.right = 400;
	spriteRect.top = 0;
	spriteRect.bottom = 300;
}

void cleanupSprite() {
	sprite->Release();
	sprite = NULL;

	currentbg->Release();
	currentbg = NULL;

	pointer->Release();
	pointer = NULL;

	numTexture->Release();
	numTexture = NULL;
}

int main()  //int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	createWindow();

	createDirectX();

	createSprite();

	while (windowIsRunning())
	{
		render();

	}

	cleanupSprite();

	cleanupDirectX();

	cleanupWindow();

	return 0;
}
