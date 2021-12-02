// Included header files.
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

// Define directives for constants.
#define MY_SDL_FLAGS SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO
#define DIGITS_LENGTH 24
#define TEXTS_LENGTH 4
#define WIDTH  290
#define HEIGHT 180
#define TITLE "Super Clock - SDL"
#define ICON "images/superclock.png"
#define FPS 60

#define LEN(x) (sizeof(x)/sizeof(x[0]))

// Global constants variables.
const float FRAME_DELAY = 1000.0f / FPS;

// Global variables.
bool running = true;
bool show_fps = false;
bool show_time = false;
int style = 1;

// forward declaration of functions.
bool sdl_setup(SDL_Window** win, SDL_Renderer** rend);
bool rects_populate_res(SDL_Rect rects[], size_t rectsc, int style, SDL_Window *win);
bool texts_populate(SDL_Texture *texts[], size_t textsc, SDL_Renderer *rend);
struct tm *get_time(struct tm *tt_local);
void time_in_title(struct tm timeinfo, SDL_Window *win);
void time_to_binary(int *digits, struct tm timeinfo);
void fps_print();
void fps_delay();
Uint32 timer_show_time(Uint32 interval, void* param);
void memory_release_exit(SDL_Window** win, SDL_Renderer** rend, SDL_Texture *texts[], size_t textsc, int exit_status);

// Main function that launches the program.
int main(void)
{
    struct tm *tt_local;
    SDL_Event event;
    SDL_TimerID timer;

    SDL_Window *win = NULL;
    SDL_Renderer *rend = NULL;
    int digits[DIGITS_LENGTH];
    SDL_Rect rects[DIGITS_LENGTH];
    SDL_Texture *texts[TEXTS_LENGTH] = {0};

    // Initialize SDL, create window and renderer.
    if (!sdl_setup(&win, &rend))
        memory_release_exit(&win, &rend, texts, LEN(texts), EXIT_FAILURE);

    // Create all the Rects and set the Resolution.
    if (!rects_populate_res(rects, LEN(rects), style, win))
        memory_release_exit(&win, &rend, texts, LEN(texts), EXIT_FAILURE);

    // Create the 4 textures.
    if (!texts_populate(texts, LEN(texts), rend))
        memory_release_exit(&win, &rend, texts, LEN(texts), EXIT_FAILURE);

    while (running) {
        // Check key events, key pressed or released.
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    // handling of close button
                    running = false;
                    break;
                case SDL_USEREVENT:
                    if (event.user.code == 1) {
                        show_time = false;
                        SDL_SetWindowTitle(win, TITLE);
                    }
                case SDL_KEYDOWN:
                    // keyboard API for key pressed
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_SPACE:
                            if (show_time)
                                SDL_RemoveTimer(timer);
                            else
                                show_time = true;
                            timer = SDL_AddTimer(5000, timer_show_time, NULL);
                            break;
                        case SDL_SCANCODE_S:
                            if (style < 6)
                                style++;
                            else
                                style = 1;
                            rects_populate_res(rects, LEN(rects), style, win);
                            break;
                        case SDL_SCANCODE_F:
                            if (show_fps)
                                show_fps = false;
                            else
                                show_fps = true;
                            break;
                        default:
                            break;
                    }
            }
        }

        // Get the current time each frame.
        tt_local = get_time(tt_local);

        // Set the window title as the time.
        if (show_time)
            time_in_title(*tt_local, win);

        // Convert Decemil time to Binary time.
        time_to_binary(digits, *tt_local);

        // Clear the existing renderer.
        SDL_RenderClear(rend);

        // Draw the images to the renderer.
        for (int i = 0; i < 24; i++) {
            int offset = (style < 4 ) ? 0: 2;
            SDL_RenderCopy(rend, texts[digits[i] + offset], NULL, &rects[i]);
        }

        // Swap the back buffer to the front.
        SDL_RenderPresent(rend);

        // Print FPS to standard output.
        if (show_fps)
            fps_print();

        // Calculate delay needed for the FPS.
        fps_delay();
    }
    // Release memory and null pointers before exiting.
    memory_release_exit(&win, &rend, texts, LEN(texts), EXIT_SUCCESS);
}

// Initialize SDL, create window and renderer.
bool sdl_setup(SDL_Window** win, SDL_Renderer** rend) {
    // Initialize SDL.
    if (SDL_Init(MY_SDL_FLAGS)) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return false;
    }

    // Initialize TTF
    if(TTF_Init()) {
        fprintf(stderr, "Error initializing TTF: %s\n", TTF_GetError());
        return false;
    }

    // Created the SDL Window.
    *win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if (!*win) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        return false;
    }

    // Create the SDL Renderer.
    *rend = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED);
    if (!*rend) {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        return false;
    }

    // load icon image as surface, set as window icon, then release surface.
    SDL_Surface *icon = IMG_Load(ICON);
    if (icon) {
        SDL_SetWindowIcon(*win, icon);
        SDL_FreeSurface(icon);
    } else {
        fprintf(stderr, "Error creating icon surface: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

// Create the layout of the Rects and set the Resolution.
bool rects_populate_res(SDL_Rect rects[], size_t rectsc, int style, SDL_Window *win) {
    switch (style) {
        case 2:     // styles 2 and 5 have the same demintions.
        case 5: {
            int x[] = {5, 60, 115};
            int y[] = {5, 50, 95, 140, 185, 230, 275, 320};
            int i = 0;
            for (int ix = 0; ix < LEN(x); ix++ ) {
                for (int iy = 0; iy < LEN(y); iy++ ) {
                    if (i >= rectsc) {
                        fprintf(stderr, "Populating rects array went out of bounds!\n");
                        return false;
                    }
                    rects[i].x = x[ix];
                    rects[i].y = y[iy];
                    rects[i].w = 35;
                    rects[i].h = 35;
                    i++;
                }
            }
            SDL_SetWindowSize(win, 155, 360);
            break;
        }
        case 3:     // styles 3 and 5 have the same demintions.
        case 6: {
            int y[] = {5, 60, 115};
            int x[] = {5, 50, 95, 140, 185, 230, 275, 320};
            int i = 0;
            for (int iy = 0; iy < LEN(y); iy++ ) {
                for (int ix = 0; ix < LEN(x); ix++ ) {
                    if (i >= rectsc) {
                        fprintf(stderr, "Populating rects array went out of bounds!\n");
                        return false;
                    }
                    rects[i].x = x[ix];
                    rects[i].y = y[iy];
                    rects[i].w = 35;
                    rects[i].h = 35;
                    i++;
                }
            }
            SDL_SetWindowSize(win, 360, 155);
            break;
        }
        case 1:     // styles 1 and 4 have the same demintions. This is the default.
        case 4:
        default: {
            int x[] = {5, 50, 105, 150, 205, 250};
            int y[] = {5, 50, 95, 140};
            int i = 0;
            for (int ix = 0; ix < LEN(x); ix++ ) {
                for (int iy = 0; iy < LEN(y); iy++ ) {
                    if (i >= rectsc) {
                        fprintf(stderr, "Populating rects array went out of bounds!\n");
                        return false;
                    }
                    rects[i].x = x[ix];
                    rects[i].y = y[iy];
                    rects[i].w = 35;
                    rects[i].h = 35;
                    i++;
                }
            }
            SDL_SetWindowSize(win, 290, 180);
        }
    }
    return true;
}

// Create the 4 textures.
bool texts_populate(SDL_Texture *texts[], size_t textsc, SDL_Renderer *rend) {
    TTF_Font *font = TTF_OpenFont("freesansbold.ttf", 35);
    if (!font) {
        fprintf(stderr, "Error creating font: %s\n", TTF_GetError());
        return false;
    }
    SDL_Color colors[2] = {{20, 20, 20}, {223, 223, 223}};
    SDL_Rect off_rect = {8, 3, 35, 35};
    for (int i = 0; i < textsc; i++) {
        SDL_Surface *orig_surf = SDL_CreateRGBSurface(0,35,35,32,0,0,0,0);
        if (!orig_surf) {
            fprintf(stderr, "Error creating a surface: %s\n", SDL_GetError());
            return false;
        }
        SDL_Color color;
        if (i == 1)
            color = colors[1];
        else
            color = colors[0];
        SDL_FillRect(orig_surf, NULL, SDL_MapRGB(orig_surf->format,
                                                 color.r, color.g, color.b));
        if (i > 1) {
            SDL_Surface *text_surf;
            if (i == 2)
                text_surf = TTF_RenderText_Blended(font, "0", colors[1]);
            else
                text_surf = TTF_RenderText_Blended(font, "1", colors[1]);
            if (!text_surf) {
                fprintf(stderr, "Error creating a surface: %s\n", SDL_GetError());
                return false;
            }
            SDL_BlitSurface(text_surf, NULL, orig_surf, &off_rect);
            SDL_FreeSurface(text_surf);
        }
        texts[i] = SDL_CreateTextureFromSurface(rend, orig_surf);
        if (!texts[i]) {
            fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
            return false;
        }
        SDL_FreeSurface(orig_surf);
    }
    TTF_CloseFont(font);
    return true;
}

// Return the current time.
struct tm *get_time(struct tm *timeinfo) {
    time_t tt;
    time(&tt);
    return localtime(&tt);
}

// Display the time in the Title.
void time_in_title(struct tm timeinfo, SDL_Window *win) {
    static char time_strg[9];
    sprintf(time_strg, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    SDL_SetWindowTitle(win, time_strg);
}

// Convert Decemil time to Binary time.
void time_to_binary(int *digits, struct tm timeinfo) {
    for (int i = 23; i > 15; i--) {
        digits[i] = timeinfo.tm_sec % 2;
        timeinfo.tm_sec /= 2;
    }
    for (int i = 15; i > 7; i--) {
        digits[i] = timeinfo.tm_min % 2;
        timeinfo.tm_min /= 2;
    }
    for (int i = 7; i >= 0; i--) {
        digits[i] = timeinfo.tm_hour % 2;
        timeinfo.tm_hour /= 2;
    }
}

// Print FPS to standard output.
void fps_print() {
    static Uint32 next_time = 1000;
    static int frame_count = 1;
    Uint32 current_time = SDL_GetTicks();
    if (current_time >= next_time) {
        if ((current_time - 1000) > next_time) {
            if (current_time > (UINT32_MAX - 1000))
                next_time = 1000;
            else
                next_time = current_time + 1000;
        } else {
            printf("%d\n", frame_count);
            if (next_time > (UINT32_MAX - 1000))
                next_time = 1000;
            else
                next_time += 1000;
        }
        frame_count = 1;
    } else {
        if (next_time > (UINT32_MAX - 1000))
            next_time = 1000;
        else
            frame_count++;
    }
}

// Calculate delay needed for the FPS.
void fps_delay() {
    static float carry_delay = 0;
    static Uint32 last_time = 0;

    Uint32 current_time = SDL_GetTicks();
    // Checking for roll over
    if (current_time >= last_time) {
        Uint32 elapsed_time = current_time - last_time;
        if ( ( FRAME_DELAY + carry_delay ) > elapsed_time ) {
            unsigned int current_delay = FRAME_DELAY - elapsed_time + carry_delay;
            SDL_Delay(current_delay);
            current_time = SDL_GetTicks();
            // Checking for roll over again.
            if (current_time >= last_time) {
                elapsed_time = current_time - last_time;
                carry_delay = FRAME_DELAY - elapsed_time + carry_delay;
            } else {
                carry_delay = 0;
            }
        } else {
            carry_delay = FRAME_DELAY - elapsed_time + carry_delay;
        }
    } else {
        carry_delay = 0;
    }
    if (carry_delay > FRAME_DELAY)
        carry_delay = FRAME_DELAY;
    if (carry_delay < -FRAME_DELAY)
        carry_delay = -FRAME_DELAY;
    last_time = current_time;
}

// Create a user callback event.
Uint32 timer_show_time(Uint32 interval, void* param) {
    // Create a user event to call the game loop.
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = 1;
    event.user.data1 = 0;
    event.user.data2 = 0;
    SDL_PushEvent(&event);
    return 0;
}

// Release memory and null pointers before exiting.
void memory_release_exit(SDL_Window** win, SDL_Renderer** rend, SDL_Texture *texts[], size_t textsc, int exit_status) {
    for (int i = 0; i < textsc; i++) {
        SDL_DestroyTexture(texts[i]);
        texts[i] = NULL;
    }
    SDL_DestroyRenderer(*rend);
    *rend = NULL;
    SDL_DestroyWindow(*win);
    *win = NULL;
    SDL_Quit();
    exit(exit_status);
}
