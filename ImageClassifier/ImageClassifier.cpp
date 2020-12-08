#include <iostream>
#include <fstream>
#include <windows.h>
#include <commctrl.h>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <mbstring.h>
#include <tchar.h> 
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "NeuralNetwork.h"

#pragma comment(lib, "ComCtl32.Lib")
using namespace std;
#define MAIN_CLASS_NAME L"MainWindow"
#define MAIN_WINDOW_NAME L"Digits Classifier"

int WindowX = 400;
int WindowY = 100;
int ImageMapRectX = 0;
int ImageMapRectY = 10;
int RectCountX = 28;
int RectCountY = 28;
int RectOffsetX = 1;
int RectOffsetY = 1;
int RectWidth = 15;
int RectHeight = 15;
int recognizedNumber = -1;
bool isLeftButtonClicked = false;
bool isDrawingMode = false;
NeuralNetwork* nn;
vector<vector<double>> result = vector<vector<double>>(10, { {0.0} });

int DigitMap[784] = {};
int DigitMapTwoDimensional[28][28];



string intToStr(int a) {
	stringstream ss;
	ss << a;
	return ss.str();
}

bool ifMouseInImageMap() {
	POINT p;
	if (GetCursorPos(&p))
	{
		if ((p.x >= WindowX + ImageMapRectX) &&
			((p.x <= WindowX + ImageMapRectX + (RectWidth + RectOffsetX) * RectCountX - RectOffsetX + 7)) &&
			(p.y >= WindowY + ImageMapRectY + 31) &&
			((p.y <= WindowY + ImageMapRectY + (RectHeight + RectOffsetY) * RectCountY - RectOffsetY + 28)))
		{
			return true;
		}
		else {
			return false;
		}
	}
}

POINT getRectangleCordsOnHover() {
	POINT p;
	p.x = 0;
	p.y = 0;
	POINT currpos;
	if (GetCursorPos(&currpos))
	{
		int mainRectWidth = (RectWidth + RectOffsetX) * RectCountX - RectOffsetX;
		int mainRectHeight = (RectHeight + RectOffsetY) * RectCountY - RectOffsetY;
		p.x = (currpos.x - WindowX - 7) / (RectWidth + RectOffsetX);
		p.y = (currpos.y - WindowY - 40) / (RectHeight + RectOffsetY);
	}
	return p;
}

int x_1;
int x_2;
int y_1;
int y_2;

NeuralNetwork* loadNN() {
	NeuralNetwork *nn = NULL; 
	{
		std::ifstream ifs("C:\\Users\\German\\source\\repos\\ImageClassifier\\Debug\\nn");
		boost::archive::text_iarchive ia(ifs);
		ia >> (nn);
	}
	return nn;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (uMsg) {
		case WM_CREATE:
			memset(DigitMap, 0, 784);
			for (int i = 0; i < 28; i++) {
				memset(DigitMapTwoDimensional[i], 0, 27);
			}
			nn = loadNN();
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (!isDrawingMode) {
				Rectangle(hdc, ImageMapRectX,
					ImageMapRectY,
					ImageMapRectX + (RectWidth + RectOffsetX) * RectCountX - RectOffsetX,
					ImageMapRectY + (RectHeight + RectOffsetY) * RectCountY - RectOffsetY);
				for (int i = 0; i < RectCountX; i++) {
					for (int j = 0; j < RectCountY; j++) {
						Rectangle(hdc, j * (RectWidth + RectOffsetX) + ImageMapRectX,
							i * (RectHeight + RectOffsetY) + ImageMapRectY,
							j * (RectWidth + RectOffsetX) + RectWidth + ImageMapRectX,
							i * (RectHeight + RectOffsetY) + RectHeight + ImageMapRectY);
					}
				}

				CreateWindow(
					L"button", 
					L"Clear field", 
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					5, 
					RectCountY * (RectHeight + RectOffsetY) + ImageMapRectY + 10, 
					80, 
					20, 
					hWnd, 
					(HMENU)10000, 
					NULL, 
					NULL);

				CreateWindow(
					L"button",
					L"Classify", 
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					100, 
					RectCountY * (RectHeight + RectOffsetY) + ImageMapRectY + 10, 
					80, 
					20, 
					hWnd, 
					(HMENU)10001,
					NULL, 
					NULL);

				string str = "Это цифра: ";
				string number = intToStr(recognizedNumber);
				str = str + number;
				TextOutA(hdc, 200, RectCountY * (RectHeight + RectOffsetY) + ImageMapRectY + 10, str.c_str(), strlen(str.c_str()));
				
				Rectangle(hdc, 490, 90, 700, 307);
				SelectObject(ps.hdc, HBRUSH(CreateSolidBrush(RGB(255, 255, 255))));
				for (int i = 0; i <= 9; i++) {
					TextOutA(hdc, 500, 100 + i * 20, (intToStr(i) + ":").c_str(), strlen((intToStr(i) + ":").c_str()));
					SelectObject(ps.hdc, HBRUSH(CreateSolidBrush(RGB(255, 0, 55))));
					Rectangle(hdc, 515, 100 + i * 20 + 2, 515 + result[i][0] * 130, 100 + i * 20 + 15);
				}

				for (int i = 0; i < RectCountY; i++) {
					for (int j = 0; j < RectCountX; j++) {
						if (DigitMapTwoDimensional[i][j] == 1) {
							int x1 = j * (RectWidth + RectOffsetX);
							int x2 = x1 + RectWidth;
							int y1 = i * (RectHeight + RectOffsetY) + 10;
							int y2 = y1 + RectHeight;
							SelectObject(ps.hdc, GetStockObject(GRAY_BRUSH));
							Rectangle(hdc, x1, y1, x2, y2);
						}
					}
				}
			}
			else {
				SelectObject(ps.hdc, GetStockObject(GRAY_BRUSH));
				Rectangle(hdc, x_1, y_1, x_2, y_2);
			}
			break;
		case WM_MOUSEMOVE:
			if (ifMouseInImageMap() && isLeftButtonClicked) {
				POINT pos = getRectangleCordsOnHover();
				DigitMapTwoDimensional[pos.y][pos.x] = 1;
				OutputDebugStringA((intToStr(pos.y) + " " + intToStr(pos.x) + "\n").c_str());
				x_1 = pos.x * (RectWidth + RectOffsetX);
				x_2 = x_1 + RectWidth;
				y_1 = pos.y * (RectHeight + RectOffsetY) + 10;
				y_2 = y_1 + RectHeight;
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP: 
			isLeftButtonClicked = !isLeftButtonClicked;
			isDrawingMode = !isDrawingMode;
			break;
		case WM_COMMAND:
			if (wParam == 10000) {
				int k = 0;
				for (int i = 0; i < 28; i++) {
					for (int j = 0; j < 28; j++) {
						DigitMapTwoDimensional[i][j] = 0;
						DigitMap[k] = 0;
						k++;
					}
				}

				for (int i = 0; i < result.size(); i++) {
					result[i][0] = 0.0;
				}
				isDrawingMode = false;
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}

			if (wParam == 10001) {
				vector<vector<double>> dMap = vector<vector<double>>(784, { {0.0} });
				int pointer = 0;
				for (int i = 0; i < 28; i++) {
					for (int j = 0; j < 28; j++) {
						dMap[pointer][0] = DigitMapTwoDimensional[i][j];
						DigitMap[pointer] = DigitMapTwoDimensional[i][j];
						pointer++;
					}
				}
				
				result = nn->tryRecognizeNumber(dMap);
				double num = result[0][0];
				int index = 0;
				for (int i = 0; i < result.size(); i++) {
					if (result[i][0] > num) {
						num = result[i][0];
						index = i;
					}
				}
				int b = 5;
				recognizedNumber = index;
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(NULL);
			return NULL;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	HWND hMainWindow;
	MSG msg;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = MAIN_CLASS_NAME;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;

	RegisterClassEx(&wc);
	hMainWindow = CreateWindowEx(
		NULL, 
		MAIN_CLASS_NAME, 
		MAIN_WINDOW_NAME, 
		WS_OVERLAPPED | WS_SYSMENU, 
		WindowX, 
		WindowY,
		655, 
		600, 
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}