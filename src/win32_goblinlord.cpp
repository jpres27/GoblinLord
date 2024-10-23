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

global_variable b32 running = false;
global_variable WINDOWPLACEMENT prev_window_position = {sizeof(prev_window_position)};
global_variable u32 WIDTH = 1280;
global_variable u32 HEIGHT = 960;
global_variable Win32_Buffer global_backbuffer;
global_variable i64 perf_count_freq;

#include "win32_wasapi.h"
#include "sound.cpp"

global_variable WasapiAudio audio;

internal void init_arena(Memory_Arena *arena, memory_index size, uint8_t *base)
{
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

// NOTE: Based on Raymond Chen's blog about fullscreen toggling
internal void toggle_fullscreen(HWND window)
{
  DWORD style = GetWindowLong(window, GWL_STYLE);
  if (style & WS_OVERLAPPEDWINDOW) 
  {
    MONITORINFO monitor_info = { sizeof(monitor_info) };
    if (GetWindowPlacement(window, &prev_window_position) &&
        GetMonitorInfo(MonitorFromWindow(window,
                       MONITOR_DEFAULTTOPRIMARY), &monitor_info)) 
    {
      SetWindowLong(window, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
      SetWindowPos(window, HWND_TOP,
                   monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                   monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                   monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                   SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
  }
  else 
  {
    SetWindowLong(window, GWL_STYLE,
                  style | WS_OVERLAPPEDWINDOW);
    SetWindowPlacement(window, &prev_window_position);
    SetWindowPos(window, 0, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }
}

internal Win32_Window_Dimensions get_window_dimensions(HWND window)
{
    Win32_Window_Dimensions result;
    RECT client_rect;
    GetClientRect(window, &client_rect);
    result.width = client_rect.right - client_rect.left;
    result.height = client_rect.bottom - client_rect.top;

    return (result);
}

internal void win32_resize_DIB_section(Win32_Buffer *buffer, int width, int height)
{

    // TODO: Bulletproof this.
    // Maybe don't free first, free after, then free first if that fails.

    int bpp = 4;

    if (buffer->memory)
    {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    buffer->width = width;
    buffer->height = height;

    // NOTE: When the biHeight field is negative, Windows treats this bitmap as top-down
    // so that the first three bytes of the image are the color for the top-left pixel
    // in the bitmap.
    buffer->Info.bmiHeader.biSize = sizeof(buffer->Info.bmiHeader);
    buffer->Info.bmiHeader.biWidth = buffer->width;
    buffer->Info.bmiHeader.biHeight = -buffer->height;
    buffer->Info.bmiHeader.biPlanes = 1;
    buffer->Info.bmiHeader.biBitCount = 32;
    buffer->Info.bmiHeader.biCompression = BI_RGB;

    size_t bitmap_memory_size = (buffer->width * buffer->height) * bpp;
    buffer->memory = VirtualAlloc(0, bitmap_memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    buffer->pitch = width * bpp;
    buffer->bpp = bpp;

    // TODO: Probably clear this to black
}

internal void win32_copy_buffer_to_window(Win32_Buffer *buffer, HDC device_context, int window_width, int window_height, int x, int y, int width, int height)
{
    if((window_width >= buffer->width*2) && (window_height >= buffer->height*2))
    {
        StretchDIBits(device_context,
            0, 0, 2*buffer->width, 2*buffer->height,
            0, 0, buffer->width, buffer->height,
            buffer->memory,
            &buffer->Info,
            DIB_RGB_COLORS,
            SRCCOPY);
    }
    else
    {
        int offset_x = 10;
        int offset_y = 10;
        
        PatBlt(device_context, 0, 0, window_width, offset_y, BLACKNESS);
        PatBlt(device_context, 0, offset_y + buffer->height, window_width, window_height, BLACKNESS);
        PatBlt(device_context, 0, 0, offset_x, window_height, BLACKNESS);
        PatBlt(device_context, offset_x + buffer->width, 0, window_width, window_height, BLACKNESS);

        // NOTE: During prototyping, we will always blit 1:1 pixels for learning to code a renderer
        StretchDIBits(device_context,
                    offset_x, offset_y, buffer->width, buffer->height,
                    0, 0, buffer->width, buffer->height,
                    buffer->memory,
                    &buffer->Info,
                    DIB_RGB_COLORS,
                    SRCCOPY);
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

internal void process_pending_messages(Game_Input *input)
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
            process_keyboard_event(&input->keyboard.attack, true);
        } break;
        case WM_RBUTTONDOWN:
        {
            process_keyboard_event(&input->keyboard.block, true);
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32_t vk_code = (uint32_t)message.wParam;
            // NOTE: Since we are comparing was_down and is_down directly we need to use
            // == and != to convert them to 0 or 1 values
            b32 was_down = ((message.lParam & KEY_MESSAGE_WAS_DOWN_BIT) != 0);
            b32 is_down = ((message.lParam & KEY_MESSAGE_IS_DOWN_BIT) == 0);

            if(was_down != is_down)
            {
                if(vk_code == 'W')
                {
                    process_keyboard_event(&input->keyboard.move_fwd, is_down);
                }
                else if(vk_code == 'A')
                {
                    process_keyboard_event(&input->keyboard.move_left, is_down);
                }
                else if(vk_code == 'S')
                {
                    process_keyboard_event(&input->keyboard.move_back, is_down);
                }
                else if(vk_code == 'D')
                {
                    process_keyboard_event(&input->keyboard.move_right, is_down);
                }
                else if(vk_code == 'F')
                {
                    process_keyboard_event(&input->keyboard.inventory, is_down);
                }
                else if(vk_code == 'J')
                {
                    process_keyboard_event(&input->keyboard.inventory, is_down);
                }
                else if(vk_code == 'E')
                {
                    process_keyboard_event(&input->keyboard.use, is_down);
                }
                else if(vk_code == VK_SPACE)
                {
                    process_keyboard_event(&input->keyboard.jump, is_down);
                }

                if(is_down)
                {
                    b32 alt_key_down = (message.lParam & ALT_KEY_DOWN_BIT);
                    if((vk_code == VK_F4) && alt_key_down)
                    {
                        running = false;
                    }
                    if((vk_code == VK_RETURN) && (alt_key_down))
                    {
                        if(message.hwnd)
                        {
                            toggle_fullscreen(message.hwnd);
                        }
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
    switch( msg )
    {
        case WM_SIZE:
        {
        } break;
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
            case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(window, &paint);
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.right;
            LONG width = paint.rcPaint.bottom - paint.rcPaint.top;
            LONG height = paint.rcPaint.right - paint.rcPaint.left;
            Win32_Window_Dimensions dimensions = get_window_dimensions(window);
            win32_copy_buffer_to_window(&global_backbuffer, device_context,
                                        dimensions.width, dimensions.height, x, y, width, height);
            EndPaint(window, &paint);
        } break;
        return DefWindowProc(window,
            msg,
            wParam,
            lParam);
    }
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
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
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
            game_memory.transient_storage = ((uint8_t *)game_memory.permanent_storage + 
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

            Game_Input game_input[2] = {};
            Game_Input *new_input = &game_input[0];
            Game_Input *old_input = &game_input[1];

            LARGE_INTEGER last_counter = win32_get_wall_clock();
            LARGE_INTEGER flip_wall_clock = win32_get_wall_clock();

            u64 last_cycle_count = __rdtsc();

            while(running)
            {
                new_input->dt_for_frame = target_seconds_per_frame;

                Game_Controller_Input *old_keyboard = &old_input->keyboard;
                Game_Controller_Input *new_keyboard = &new_input->keyboard;
                new_keyboard = {};

                for(int button_index = 0; 
                    button_index < array_count(new_input->keyboard.buttons); 
                    ++button_index)
                {
                    new_keyboard->buttons[button_index].ended_down =
                    old_keyboard->buttons[button_index].ended_down;
                }
                process_pending_messages(new_input);

                vec2 dd_player = {};
                if(new_input->keyboard.move_left.ended_down)
                {
                    dd_player.x = -1.0f;
                }
                if(new_input->keyboard.move_right.ended_down)
                {
                    dd_player.x = 1.0f;
                }
                if(new_input->keyboard.move_fwd.ended_down)
                {
                    dd_player.y = 1.0f;
                }
                if(new_input->keyboard.move_back.ended_down)
                {
                    dd_player.y = -1.0f;
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

                Game_Offscreen_Buffer buffer = {};
                buffer.memory = global_backbuffer.memory;
                buffer.width = global_backbuffer.width;
                buffer.height = global_backbuffer.height;
                buffer.pitch = global_backbuffer.pitch;
                buffer.bpp = global_backbuffer.bpp;

                // Game update and render

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

                Win32_Window_Dimensions dimensions = get_window_dimensions(window);
                HDC device_context = GetDC(window);
                win32_copy_buffer_to_window(&global_backbuffer, device_context,
                                            dimensions.width, dimensions.height, 0, 0, 
                                            dimensions.width, dimensions.height);
                ReleaseDC(window, device_context);

                flip_wall_clock = win32_get_wall_clock();
                
                Game_Input *temp = new_input;
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