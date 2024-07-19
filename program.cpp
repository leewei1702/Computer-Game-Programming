//	Refer to Direct3D 9 documentation for the meaning of the members.
//https://learn.microsoft.com/en-us/windows/win32/direct3d9/dx9-graphics
//https://learn.microsoft.com/en-us/windows/win32/api/_direct3d9/

#include <d3d9.h>
#include <d3dx9.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
using namespace std;

WNDCLASS wndClass;
HWND g_hWnd = NULL;
MSG msg;

D3DPRESENT_PARAMETERS d3dPP;

IDirect3D9* direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);

IDirect3DDevice9* d3dDevice;

//pointer to a texture file
LPDIRECT3DTEXTURE9 bg1 = NULL;
LPDIRECT3DTEXTURE9 bg2 = NULL;
LPDIRECT3DTEXTURE9 bg3 = NULL;
LPDIRECT3DTEXTURE9 currentbg = NULL;
LPDIRECT3DTEXTURE9 pointer = NULL;
//pointer to a sprite interface 
LPD3DXSPRITE sprite = NULL;
RECT spriteRect;

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
		case '1':
			currentbg = bg1;
			break;
		case '2':
			currentbg = bg2;
			break;
		case '3':
			currentbg = bg3;
			break;
			//Press F to swap to fullscreen/windowed mode
			//case 'f':
			//case 'F':
			//	if (d3dPP.Windowed == true) {
			//		cout << "fullscreen on\n";
			//		d3dPP.Windowed = false;
			//	}
			//	else {
			//		cout << "windowed on\n";
			//		d3dPP.Windowed = true;
			//	}
			//	HRESULT hr = d3dDevice->Reset(&d3dPP);
			//	if (FAILED(hr)) {
			//		cout << "Resetting Device Failed !!!";
			//	}
			//	break;
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

	ZeroMemory(&wndClass, sizeof(wndClass));

	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpfnWndProc = WindowProcedure;
	wndClass.lpszClassName = "Game Window";
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wndClass);

	g_hWnd = CreateWindowEx(0, wndClass.lpszClassName, "Valorant 3.0", WS_OVERLAPPEDWINDOW, 0, 100, 1280, 720, NULL, NULL, GetModuleHandle(NULL), NULL);
	ShowCursor(false);
	ShowWindow(g_hWnd, 1);

}

bool windowIsRunning() {

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

void cleanupWindow() {
	UnregisterClass(wndClass.lpszClassName, GetModuleHandle(NULL));
}

void createDirectX() {

	ZeroMemory(&d3dPP, sizeof(d3dPP));

	d3dPP.Windowed = true;
	d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dPP.BackBufferCount = 1;
	d3dPP.BackBufferWidth = 1280; //default = 400
	d3dPP.BackBufferHeight = 720; //default = 300
	d3dPP.hDeviceWindow = g_hWnd;

	HRESULT hr = direct3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dPP, &d3dDevice);

	if (FAILED(hr))
		cout << "Creating Directx Failed !!!";
}

void render() {

	d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(red, green, blue), 1.0f, 0);

	d3dDevice->BeginScene();

	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	D3DXVECTOR2 spriteCentre;
	D3DXVECTOR2 trans;
	D3DXMATRIX mat;

	spriteCentre = D3DXVECTOR2(256, 256);
	trans = D3DXVECTOR2(0, 0);
	mat;

	D3DXMatrixTransformation2D(&mat, NULL, 0.0, NULL, &spriteCentre, NULL, &trans);

	sprite->SetTransform(&mat);

	sprite->Draw(currentbg, &spriteRect, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

    spriteCentre = D3DXVECTOR2(12, 12);
	trans = D3DXVECTOR2(x, y);
	mat;
	
	D3DXMatrixTransformation2D(&mat, NULL, 0.0, NULL, &spriteCentre, NULL, &trans);

	sprite->SetTransform(&mat);

	sprite->Draw(pointer, NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	sprite->End();

	d3dDevice->EndScene();

	d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void cleanupDirectX() {
	d3dDevice->Release();
	d3dDevice = NULL;
}

void createSprite() {
	HRESULT hr = D3DXCreateSprite(d3dDevice, &sprite);

	if (FAILED(hr)) {
		cout << "Create Sprite Failed!!!";
	}

	//	Create texture. Study the documentation.
	//load the graphic file to the GPU memory
	hr = D3DXCreateTextureFromFile(d3dDevice, "Assets/bg1.png", &bg1);
	hr = D3DXCreateTextureFromFile(d3dDevice, "Assets/bg2.png", &bg2);
	hr = D3DXCreateTextureFromFile(d3dDevice, "Assets/bg3.png", &bg3);
	currentbg = bg1;

	hr = D3DXCreateTextureFromFile(d3dDevice, "Assets/pointer.png", &pointer);
	//hr = D3DXCreateTextureFromFileEx(d3dDevice, "Assets/bg1.png", D3DX_DEFAULT, D3DX_DEFAULT,
	//									D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
	//									D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_XRGB(255, 255, 255), 
	//									NULL, NULL, &texture);

	if (FAILED(hr)) {
		cout << "Create Texture from File Failed!!!";
	}

	//	Specify the "	" rectangle.
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
