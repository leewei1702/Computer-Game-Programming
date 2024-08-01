//	Refer to Direct3D 9 documentation for the meaning of the members.
//https://learn.microsoft.com/en-us/windows/win32/direct3d9/dx9-graphics
//https://learn.microsoft.com/en-us/windows/win32/api/_direct3d9/
//try to do clock
//use radian

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
	RECT& crop() {
		spriteRect.left = currentFrame % spriteCol * spriteWidth;
		spriteRect.right = currentFrame % spriteCol * spriteWidth + spriteWidth;
		spriteRect.top = currentFrame / spriteRow * spriteHeight;
		spriteRect.bottom = currentFrame / spriteRow * spriteHeight + spriteHeight;
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

double secDegree = 270;
double minDegree = 270;
double hrDegree = 270;
bool hrBegin = false;

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
			explosionSprite.nextFrame();
			break;
		case VK_DOWN:
			numSprite.prevFrame();
			explosionSprite.prevFrame();
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

	//Sprite Transform Object - (Center, Translation, Scaling)
	SpriteTransform bgTrans(D3DXVECTOR2(256, 256), D3DXVECTOR2(0, 0), D3DXVECTOR2(3.2, 2.4));
	SpriteTransform numTrans(D3DXVECTOR2(64, 64), D3DXVECTOR2(30, 30), D3DXVECTOR2(1, 1));
	SpriteTransform pointerTrans(D3DXVECTOR2(0, 0), D3DXVECTOR2(x, y), D3DXVECTOR2(1, 1));
	SpriteTransform explosionTrans(D3DXVECTOR2(1024, 1024), D3DXVECTOR2(200, 200), D3DXVECTOR2(1, 1));

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
	font->DrawText(sprite, "Hello World!",-1, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

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
	double secRad = 3.142 / 180 * secDegree;
	double minRad = 3.142 / 180 * minDegree;
	double hrRad = 3.142 / 180 * hrDegree;

	double secX = cos(secRad) * sqrt(pow(100, 2) + pow(100, 2)) + 200;
	double secY = sin(secRad) * sqrt(pow(100, 2) + pow(100, 2)) + 200;
	double minX = cos(minRad) * sqrt(pow(50, 2) + pow(50, 2)) + 200;
	double minY = sin(minRad) * sqrt(pow(50, 2) + pow(50, 2)) + 200;
	double hrX = cos(hrRad) * sqrt(pow(30, 2) + pow(30, 2)) + 200;
	double hrY = sin(hrRad) * sqrt(pow(30, 2) + pow(30, 2)) + 200;
	//Draw Line
	D3DXVECTOR2 secLineVertices[] = { D3DXVECTOR2(200,200), D3DXVECTOR2(secX,secY) };
	D3DXVECTOR2 minLineVertices[] = { D3DXVECTOR2(200,200), D3DXVECTOR2(minX,minY) };
	D3DXVECTOR2 hrLineVertices[] = { D3DXVECTOR2(200,200), D3DXVECTOR2(hrX,hrY) };

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
