#include <windows.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
#define SIDES 12

// Global variables for animation
float rotationAngleZ = 0;

// Structure for a 3D point
typedef struct {
    float x, y, z;
} Point3D;

// Function to rotate a point around the Z-axis
void rotateZ(Point3D* p, float angle) {
    float cosA = cos(angle);
    float sinA = sin(angle);
    float newX = cosA * p->x - sinA * p->y;
    float newY = sinA * p->x + cosA * p->y;
    p->x = newX;
    p->y = newY;
}

// Function to project a 3D point to 2D screen coordinates
void projectTo2D(Point3D* p, int* x, int* y) {
    *x = (int)(WIDTH / 2 + p->x * 150);
    *y = (int)(HEIGHT / 2 - p->z * 150);
}

// Function to draw a polygon face with GDI
void drawPolygon(HDC hdc, Point3D vertices[], int vertexCount, COLORREF color) {
    POINT points[SIDES];
    for (int i = 0; i < vertexCount; i++) {
        int x, y;
        projectTo2D(&vertices[i], &x, &y);
        points[i].x = x;
        points[i].y = y;
    }
    HBRUSH hBrush = CreateSolidBrush(color);
    SelectObject(hdc, hBrush);
    Polygon(hdc, points, vertexCount);
    DeleteObject(hBrush);
}

// Window procedure (handles window messages)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Define vertices for the Chaos Emerald shape
            Point3D topVertices[SIDES];
            Point3D bottomVertices[SIDES];
            Point3D tip = {0, 0, -1.0f}; // Tip of the pyramid (bottom)

            // Calculate top and bottom vertices
            for (int i = 0; i < SIDES; i++) {
                float angle = 2 * 3.14159f * i / SIDES;
                float x = cos(angle);
                float y = sin(angle);

                // Top vertices (crown)
                topVertices[i].x = x * 0.7f;
                topVertices[i].y = y * 0.7f;
                topVertices[i].z = 0.1f;

                // Bottom vertices (girdle)
                bottomVertices[i].x = x;
                bottomVertices[i].y = y;
                bottomVertices[i].z = -0.3f;
            }

            // Rotate vertices for animation
            for (int i = 0; i < SIDES; i++) {
                rotateZ(&topVertices[i], rotationAngleZ);
                rotateZ(&bottomVertices[i], rotationAngleZ);
            }
            rotateZ(&tip, rotationAngleZ);

            // Draw top faces (crown)
            for (int i = 0; i < SIDES; i++) {
                int next = (i + 1) % SIDES;
                Point3D face[3] = {topVertices[i], topVertices[next], bottomVertices[i]};
                drawPolygon(hdc, face, 3, RGB(0, 255, 0));
            }

            // Draw bottom faces (pavilion)
            for (int i = 0; i < SIDES; i++) {
                int next = (i + 1) % SIDES;
                Point3D face[3] = {bottomVertices[i], bottomVertices[next], tip};
                drawPolygon(hdc, face, 3, RGB(0, 128, 0));
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_TIMER: {
            // Update the rotation angle for animation
            rotationAngleZ += 0.02f;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {0};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ChaosEmerald";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    // Register the window class
    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, "Chaos Emerald",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
                               NULL, NULL, hInstance, NULL);

    // Set a timer for rotation updates
    SetTimer(hwnd, 1, 30, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}