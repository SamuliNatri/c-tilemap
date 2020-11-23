#include <windows.h>
#include <stdint.h>

#define COLUMNS 10
#define ROWS 10

typedef uint32_t u32;

int running = 1;
int client_width = 640;
int client_height = 640;

int player_x;
int player_y;

int moving_right;
int moving_left;
int moving_up;
int moving_down;

int tile_size = 64;

void* memory;
BITMAPINFO bitmap_info;

int level[COLUMNS * ROWS] = {
    1,1,0,0,0,0,0,0,0,0,
    0,1,1,0,0,0,0,0,0,0,
    0,0,1,1,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,1,1,1,1,1,
    0,0,0,0,0,2,0,0,0,0,
    0,0,0,0,0,1,0,0,0,0,
    0,0,0,0,0,1,0,0,0,0,
    0,0,0,0,0,1,0,0,0,0,
    0,0,0,0,0,1,0,0,0,0
};

void draw_rectangle_tile(int rec_x,
                         int rec_y,
                         int rec_width,
                         int rec_height,
                         u32 color);

void clear_screen(u32 color)
{
    u32 *pixel = (u32 *)memory; 
    
    for(int pixel_number = 0;
        pixel_number < client_width * client_height;
        ++pixel_number)
    {
        *pixel++ = color;
    }
}

void draw_level(int *level)
{
    for(int y = 0;
        y < ROWS;
        ++y)
    {
        for(int x = 0;
            x < COLUMNS;
            ++x)
        {
            
            int tile_type = level[(ROWS - 1 - y) * COLUMNS + x];
            
            if(tile_type != 0)
            {
                u32 color = 0x222222;
                
                if(tile_type == 2) color = 0x444444;
                
                draw_rectangle_tile(x, y, 
                                    tile_size, tile_size, 
                                    color);
            }
        }
    }
}

void draw_rectangle_tile(int rec_x,
                         int rec_y,
                         int rec_width,
                         int rec_height,
                         u32 color)
{
    u32 *pixel = (u32 *)memory;
    pixel += (rec_y * client_width * tile_size) + (rec_x * tile_size); 
    
    for(int y = 0;
        y < rec_height;
        ++y)
    {
        for(int x = 0;
            x < rec_width;
            ++x)
        {
            *pixel++ = color;
        }
        
        pixel += client_width - rec_width;
    }
}

void draw_rectangle(int rec_x,
                    int rec_y,
                    int rec_width,
                    int rec_height,
                    u32 color)
{
    u32 *pixel = (u32 *)memory;
    pixel += rec_y * client_width + rec_x;
    
    for(int y = 0;
        y < rec_height;
        ++y)
    {
        for(int x = 0;
            x < rec_width;
            ++x)
        {
            *pixel++ = color;
        }
        
        pixel += client_width - rec_width;
    }
}

int player_can_move(x, y)
{
    int result = 1;
    
    if(x < 0 | y < 0 | x >= COLUMNS | y >= ROWS) result = 0;
    
    int tile_type = level[(ROWS - 1 - y) * COLUMNS + x];
    
    if(tile_type == 1) result = 0;
    
    return result;
}

void move_player()
{
    int new_x = player_x;
    int new_y = player_y;
    
    if(moving_right) ++new_x;
    if(moving_left) --new_x;
    if(moving_up) ++new_y;
    if(moving_down) --new_y;
    
    if(player_can_move(new_x, new_y))
    {
        player_x = new_x;
        player_y = new_y;
    }
    
    moving_right = 0;
    moving_left = 0;
    moving_up = 0;
    moving_down = 0;
}

LRESULT CALLBACK 
WindowProc(HWND window, 
           UINT message, 
           WPARAM w_param, 
           LPARAM l_param)
{
    LRESULT result;
    switch(message)
    {
        case WM_CLOSE:
        {
            running = 0;
        } break;
        
        case WM_KEYDOWN:
        {
            switch(w_param)
            {
                case VK_RIGHT:
                {
                    moving_right = 1;
                } break;
                
                case VK_LEFT:
                {
                    moving_left = 1;
                } break;
                
                case VK_UP:
                {
                    moving_up = 1;
                } break;
                
                case VK_DOWN:
                {
                    moving_down = 1;
                } break;
            }
        } break;
        
        default:
        {
            result = DefWindowProc(window,
                                   message, 
                                   w_param, 
                                   l_param);
        } break;
    }
    
    return result;
}

int WINAPI 
wWinMain(HINSTANCE instance, 
         HINSTANCE prev_instance, 
         PWSTR cmd_line, 
         int cmd_show)
{
    WNDCLASS window_class = {0};
    
    wchar_t class_name[] = L"GameWindowClass";
    
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = class_name;
    window_class.hCursor = LoadCursor(0,
                                      IDC_CROSS
                                      );
    
    
    
    RECT window_rect;
    window_rect.left = 0;
    window_rect.top = 0;
    window_rect.right = client_width;
    window_rect.bottom = client_height;
    
    AdjustWindowRectEx(&window_rect,
                       WS_OVERLAPPEDWINDOW,
                       0,0
                       );
    
    int window_width = window_rect.right - window_rect.left;
    int window_height = window_rect.bottom - window_rect.top;
    
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    
    int window_x = (screen_width / 2) - (window_width / 2);
    int window_y = (screen_height / 2) - (window_height / 2);
    
    RegisterClass(&window_class);
    
    HWND window = CreateWindowEx(0,
                                 class_name,
                                 L"Game",
                                 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                 window_x,
                                 window_y,
                                 window_width,
                                 window_height,
                                 0,
                                 0,
                                 instance,
                                 0);
    
    memory = VirtualAlloc(0,
                          client_width * client_height * 4,
                          MEM_RESERVE|MEM_COMMIT,
                          PAGE_READWRITE
                          );
    
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = client_width;
    bitmap_info.bmiHeader.biHeight = client_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    
    HDC hdc = GetDC(window);
    
    while(running)
    {
        MSG message;
        while(PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        
        clear_screen(0x111111);
        
        draw_level(level);
        
        move_player();
        /*
                        draw_rectangle(100, 100, 50, 50, 0x222222);
                        draw_rectangle(150, 150, 25, 25, 0x333333);
                        draw_rectangle(175, 175, 15, 15, 0x444444);
                        */
        
        draw_rectangle_tile(player_x, 
                            player_y, 
                            tile_size, 
                            tile_size, 
                            0xff00f7);
        
        StretchDIBits(hdc,
                      0,
                      0,
                      client_width,
                      client_height,
                      0,
                      0,
                      client_width,
                      client_height,
                      memory,
                      &bitmap_info,
                      DIB_RGB_COLORS,
                      SRCCOPY
                      );
    }
    
    return 0;
}
