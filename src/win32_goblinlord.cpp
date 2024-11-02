#pragma comment (lib, "mfplat")
#pragma comment (lib, "mfreadwrite")

#include <stdint.h>
#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include "goblinlord.h"
#include "platform.h"
#include "win32_goblinlord.h"
#include "goblinlord_math.h"
#include "goblinlord_intrinsics.h"

global_variable b32 running = false;
global_variable WINDOWPLACEMENT prev_window_position = {sizeof(prev_window_position)};

global_variable u32 SCREEN_WIDTH = 1020;
global_variable u32 SCREEN_HEIGHT = 540;
global_variable u32 GAME_WIDTH = 960;
global_variable u32 GAME_HEIGHT = 540;
global_variable Win32_Buffer global_backbuffer;
global_variable i64 perf_count_freq;

#include "win32_wasapi.h"
#include "sound.cpp"
#include "goblinlord.cpp"

global_variable WasapiAudio audio;

internal void InitArena(Memory_Arena *arena, memory_index size, u8 *base)
{
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

internal Win32_Window_Dimensions Win32GetWindowDimensions(HWND window)
{
    Win32_Window_Dimensions result;
    RECT client_rect;
    GetClientRect(window, &client_rect);
    result.width = client_rect.right - client_rect.left;
    result.height = client_rect.bottom - client_rect.top;

    return (result);
}

internal void Win32ResizeDIBSection(Win32_Buffer *buffer, int width, int height)
{
    if(buffer->memory)
    {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    buffer->width = width;
    buffer->height = height;

    int bpp = 4;
    buffer->bpp = bpp;

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = buffer->width;
    buffer->info.bmiHeader.biHeight = -buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;

    int bitmap_memory_size = (buffer->width*buffer->height)*bpp;
    buffer->memory = VirtualAlloc(0, bitmap_memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    buffer->pitch = width*bpp;
}

internal void Win32DisplayBufferInWindow(Win32_Buffer *Buffer,
                           HDC DeviceContext, int WindowWidth, int WindowHeight)
{
    if((WindowWidth >= Buffer->width*3) &&
       (WindowHeight >= Buffer->height*3))
    {
        StretchDIBits(DeviceContext,
                      0, 0, 3*Buffer->width, 3*Buffer->height,
                      0, 0, Buffer->width, Buffer->height,
                      Buffer->memory,
                      &Buffer->info,
                      DIB_RGB_COLORS, SRCCOPY);
    }
    else
    {
        int OffsetX = 10;
        int OffsetY = 10;

        PatBlt(DeviceContext, 0, 0, WindowWidth, OffsetY, BLACKNESS);
        PatBlt(DeviceContext, 0, OffsetY + Buffer->height, WindowWidth, WindowHeight, BLACKNESS);
        PatBlt(DeviceContext, 0, 0, OffsetX, WindowHeight, BLACKNESS);
        PatBlt(DeviceContext, OffsetX + Buffer->width, 0, WindowWidth, WindowHeight, BLACKNESS);

        StretchDIBits(DeviceContext,
                      OffsetX, OffsetY, Buffer->width, Buffer->height,
                      0, 0, Buffer->width, Buffer->height,
                      Buffer->memory,
                      &Buffer->info,
                      DIB_RGB_COLORS, SRCCOPY);
    }
}

internal void process_keyboard_event(Game_Button_State *new_state, b32 is_down)
{
    if(new_state->ended_down != is_down)
    {
        new_state->ended_down = is_down;
        ++new_state->half_transistion_count;
    }
}

internal void process_pending_messages(Game_Controller_Input *input)
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
        case WM_QUIT:
        {
            running = false;
        } break;
        case WM_MOUSEMOVE:
        {
            input->mouse_x = GET_X_LPARAM(message.lParam); 
            input->mouse_y = GET_Y_LPARAM(message.lParam);
        } break;
        case WM_LBUTTONDOWN:
        {
            process_keyboard_event(&input->attack, true);
        } break;
        case WM_RBUTTONDOWN:
        {
            process_keyboard_event(&input->block, true);
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            u32 vk_code = (u32)message.wParam;
            // NOTE: Since we are comparing was_down and is_down directly we need to use
            // == and != to convert them to 0 or 1 values
            b32 was_down = ((message.lParam & KEY_MESSAGE_WAS_DOWN_BIT) != 0);
            b32 is_down = ((message.lParam & KEY_MESSAGE_IS_DOWN_BIT) == 0);

            if(was_down != is_down)
            {
                if(vk_code == 'W')
                {
                    process_keyboard_event(&input->move_fwd, is_down);
                }
                else if(vk_code == 'A')
                {
                    process_keyboard_event(&input->move_left, is_down);
                }
                else if(vk_code == 'S')
                {
                    process_keyboard_event(&input->move_back, is_down);
                }
                else if(vk_code == 'D')
                {
                    process_keyboard_event(&input->move_right, is_down);
                }
                else if(vk_code == 'F')
                {
                    process_keyboard_event(&input->inventory, is_down);
                }
                else if(vk_code == 'J')
                {
                    process_keyboard_event(&input->inventory, is_down);
                }
                else if(vk_code == 'E')
                {
                    process_keyboard_event(&input->use, is_down);
                }
                else if(vk_code == VK_SPACE)
                {
                    process_keyboard_event(&input->jump, is_down);
                }

                if(is_down)
                {
                    b32 alt_key_down = (message.lParam & ALT_KEY_DOWN_BIT);
                    if((vk_code == VK_F4) && alt_key_down)
                    {
                        running = false;
                    }
                }
            }
        } break;
        default:
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        } break;
        }
    }
}

LRESULT CALLBACK win32_main_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch( msg )
    {
       case WM_DESTROY:
        {
            // TODO: Handle with error message to user?
            running = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            assert(!"Keyboard input came in through a non-dispatch message!");
        } break;

        case WM_CLOSE:
        {
            // TODO: Are you sure you want to close popup?
            running = false;
        } break;

        case WM_SETCURSOR: 
        {
            result = DefWindowProc(window, msg, wParam, lParam);
        } break;

        case WM_ACTIVATEAPP: 
        {
            if(wParam == TRUE)
            {
                SetLayeredWindowAttributes(window, RGB(0, 0, 0), 255, LWA_ALPHA);
            }
            else 
            {
                SetLayeredWindowAttributes(window, RGB(0, 0, 0), 64, LWA_ALPHA);
            }
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            Win32_Window_Dimensions d = Win32GetWindowDimensions(window);
            Win32DisplayBufferInWindow(&global_backbuffer, dc, d.width, d.height);
            EndPaint(window, &paint);
        } break;

        default:
        {
            result = DefWindowProc(window, msg, wParam, lParam);
        } break;
    }
    return(result);
}

void messageloop(HWND window)
{
    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        switch(msg.message)
        {
        case WM_QUIT:
        {
            running = false;
            DestroyWindow(window);
        } break;
        default:
        {
            TranslateMessage(&msg);    
            DispatchMessage(&msg);  
        } break;
        }
    }
}

inline LARGE_INTEGER win32_get_wall_clock(void)
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return(counter);
}

inline r32 win32_get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
    r32 seconds_elapsed = ((r32)(end.QuadPart - start.QuadPart) /
                             (r32)perf_count_freq);
    return(seconds_elapsed);
}

int WINAPI WinMain(HINSTANCE instance,
    HINSTANCE prev_instance, 
    LPSTR cmd_line,
    int show_cmd)
{
    LARGE_INTEGER perf_freq_result;
    QueryPerformanceFrequency(&perf_freq_result);
    perf_count_freq = perf_freq_result.QuadPart;
    UINT desired_scheduler_milliseconds = 1;
    b32 scheduler_granularity_set = (timeBeginPeriod(desired_scheduler_milliseconds) == TIMERR_NOERROR);

    WNDCLASS window_class = {};

    Win32ResizeDIBSection(&global_backbuffer, GAME_WIDTH, GAME_HEIGHT);

    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = win32_main_window_callback;
    window_class.hInstance = instance;
    window_class.hCursor = LoadCursor(instance, IDC_CROSS);
    window_class.lpszClassName = ("goblin_window_class");

    if(RegisterClass(&window_class))
    {
        HWND window = CreateWindowEx(
            0, //WS_EX_TOPMOST|WS_EX_LAYERED,
            window_class.lpszClassName,
            "Goblin Lord",
            WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            0,
            0,
            instance,
            0);

        if(window) 
        {
            HDC refresh_dc = GetDC(window);
            int monitor_refresh_hz = 60;
            int win32_refresh_rate = GetDeviceCaps(refresh_dc, VREFRESH);
            ReleaseDC(window, refresh_dc);
            if(win32_refresh_rate > 1) 
            {
                monitor_refresh_hz = win32_refresh_rate;
            }
            r32 game_update_hz = (monitor_refresh_hz / 2.0f);
            r32 target_seconds_per_frame = 1.0f / (r32)monitor_refresh_hz;

#if 0
            Game_Memory game_memory = {};
            game_memory.permanent_storage_size = megabytes(64);
            game_memory.transient_storage_size = gigabytes((uint64_t)3);
            game_memory.total_storage_size = game_memory.permanent_storage_size + game_memory.transient_storage_size;
            game_memory.game_memory_block = VirtualAlloc(0, (size_t)game_memory.total_storage_size, 
                                                        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            game_memory.permanent_storage = game_memory.game_memory_block;
            game_memory.transient_storage = ((u8 *)game_memory.permanent_storage + 
                                            game_memory.permanent_storage_size);
#endif
    
            // TODO: Load game music and a sound effect and connect the sound effect to some button press
            WA_Start(&audio, 48000, 2, SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT);
            size_t sampleRate = audio.bufferFormat->nSamplesPerSec;
            size_t bytesPerSample = audio.bufferFormat->nBlockAlign;

            // background "music" that will be looping
            Sound background = S_Load(L"C:/Gamedev/render/build/sound/music/midnightforest.mp3", sampleRate);
            background.loop = true;

            // simple sound effect, won't be looping
            Sound effect = S_Load(L"C:/Gamedev/render/build/sound/effects/sword_swing.wav", sampleRate);

            HANDLE input = GetStdHandle(STD_INPUT_HANDLE);

            running = true;

            Game_Controller_Input new_input = {};
            Game_Controller_Input old_input = {};

            LARGE_INTEGER last_counter = win32_get_wall_clock();
            LARGE_INTEGER flip_wall_clock = win32_get_wall_clock();

            u64 last_cycle_count = __rdtsc();

            while(running)
            {
                new_input.dt_for_frame = target_seconds_per_frame;

                new_input = {};

                for(int button_index = 0; 
                    button_index < array_count(new_input.buttons); 
                    ++button_index)
                {
                    new_input.buttons[button_index].ended_down =
                    old_input.buttons[button_index].ended_down;
                }
                process_pending_messages(&new_input);

                v2 dd_player = {};
                if(new_input.move_left.ended_down)
                {
                    dd_player.X = -1.0f;
                }
                if(new_input.move_right.ended_down)
                {
                    dd_player.X = 1.0f;
                }
                if(new_input.move_fwd.ended_down)
                {
                    dd_player.Y = 1.0f;
                }
                if(new_input.move_back.ended_down)
                {
                    dd_player.Y = -1.0f;
                }
                // TODO: Mouse movement and button handling

                bool escPressed = false;
                bool spacePressed = false;
                bool delayPressed = false;

                // TODO: Hook this up to regular input code
                while(WaitForSingleObject(input, 0) == WAIT_OBJECT_0)
                {
                    INPUT_RECORD record;
                    DWORD read;
                    if (ReadConsoleInputW(input, &record, 1, &read)
                        && read == 1
                        && record.EventType == KEY_EVENT
                        && record.Event.KeyEvent.bKeyDown)
                    {
                        switch (record.Event.KeyEvent.wVirtualKeyCode)
                        {
                        case VK_ESCAPE: escPressed = true; break;
                        case VK_SPACE: spacePressed = true; break;
                        case 'D': delayPressed = true; break;
                        }
                    }
                }

                if (escPressed)
                {
                    break;
                }

                if (spacePressed)
                {
                    effect.pos = 0;
                }

                {
                    WA_LockBuffer(&audio);

                    // write at least 100msec of samples into buffer (or whatever space available, whichever is smaller)
                    // this is max amount of time you expect code will take until the next iteration of loop
                    // if code will take more time then you'll hear discontinuity as buffer will be filled with silence
                    size_t writeCount = min(sampleRate/10, audio.sampleCount);

                    // alternatively you can write as much as "audio.sampleCount" to fully fill the buffer (~1 second)
                    // then you can try to increase delay below to 900+ msec, it still should sound fine
                    //writeCount = audio.sampleCount;

                    // advance sound playback positions
                    size_t playCount = audio.playCount;
                    S_Update(&background, playCount);
                    S_Update(&effect, playCount);

                    // initialize output with 0.0f
                    float* output = (float *)audio.sampleBuffer;
                    memset(output, 0, writeCount * bytesPerSample);

                    // mix sounds into output
                    S_Mix(output, writeCount, 0.3f, &background);
                    S_Mix(output, writeCount, 0.8f, &effect);

                    WA_UnlockBuffer(&audio, writeCount);
                }

                Game_Offscreen_Buffer offscreen_buffer = {};
                offscreen_buffer.memory = global_backbuffer.memory;
                offscreen_buffer.width = global_backbuffer.width; 
                offscreen_buffer.height = global_backbuffer.height;
                offscreen_buffer.pitch = global_backbuffer.pitch;
                offscreen_buffer.bpp = global_backbuffer.bpp;

                GameUpdateAndRender(&offscreen_buffer, &new_input, dd_player);

                Win32_Window_Dimensions d = Win32GetWindowDimensions(window);
                HDC dc = GetDC(window);
                Win32DisplayBufferInWindow(&global_backbuffer, dc, d.width, d.height);
                ReleaseDC(window, dc);

                LARGE_INTEGER work_counter = win32_get_wall_clock();
                r32 work_seconds_elapsed = win32_get_seconds_elapsed(last_counter, work_counter);
                r32 seconds_elapsed_for_frame = work_seconds_elapsed;

                if(seconds_elapsed_for_frame < target_seconds_per_frame)
                {
                    if(scheduler_granularity_set)
                    {
                        DWORD sleep_milliseconds = (DWORD)(1000.0f * (target_seconds_per_frame - 
                                                                    seconds_elapsed_for_frame));
                        if(sleep_milliseconds > 0)
                        {
                            Sleep(sleep_milliseconds);
                        }
                    }

                    r32 test_seconds_elapsed_for_fame = win32_get_seconds_elapsed(last_counter, win32_get_wall_clock());
                    if(test_seconds_elapsed_for_fame < target_seconds_per_frame)
                    {
                        // TODO: Log the missed sleep here
                    }

                    while(seconds_elapsed_for_frame < target_seconds_per_frame)
                    {
                        seconds_elapsed_for_frame = win32_get_seconds_elapsed(last_counter, 
                                                                            win32_get_wall_clock()); 
                    }

                }
                else
                {
                    // TODO: Missed frame rate
                    // TODO: Logging
                }

                LARGE_INTEGER end_counter = win32_get_wall_clock();
                r32 milliseconds_per_frame = 1000.0f*win32_get_seconds_elapsed(last_counter, end_counter);
                last_counter = end_counter;

                flip_wall_clock = win32_get_wall_clock();
                
                Game_Controller_Input temp = new_input;
                new_input = old_input;
                old_input = temp;

                u64 end_cycle_count = __rdtsc();
                u64 cycles_elapsed = end_cycle_count - last_cycle_count;
                last_cycle_count = end_cycle_count;

                r64 fps = 0.0f;
                r64 megacycles_per_frame = ((r64)cycles_elapsed / (1000.0f * 1000.0f));

                char text_buffer[256];
                sprintf(text_buffer, "%.02f milliseconds per frame | %.02f frames per second | %.02f megacycles per frame", 
                        milliseconds_per_frame, fps, megacycles_per_frame);
                OutputDebugString(text_buffer);
                OutputDebugString("\n\n");
            } // End of while(running)
        }
        else 
        {
            // TODO: Log error if window creation failed
        }
    }
    else 
    {
         // TODO: Log error if window class creation failed
    }

    WA_Stop(&audio);
    
    return(0);
}