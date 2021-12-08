// Included header files.
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

// Define directives for constants.
#define MY_SDL_FLAGS (SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO)
#define TITLE "Super Clock - SDL"
#define ICON "images/superclock.png"
#define DIGITV_LENGTH 24
#define TEXTV_LENGTH 4
#define FPS 30

// Length of array macro.
#define LEN(x) (sizeof(x)/sizeof(x[0]))

struct superclock {
    SDL_Window *win;
    SDL_Renderer *rend;
    int window_width;
    int window_height;
    struct digitv {
        unsigned short digit;
        SDL_Rect rect;
    } digitv[DIGITV_LENGTH];
    unsigned short digitc;
    SDL_Texture *textv[TEXTV_LENGTH];
    unsigned short textc;
    bool running;
    float frame_delay;
    bool show_fps;
    bool show_time;
    unsigned short style;
    unsigned short offset;
    unsigned short exit_status;
};

// forward declaration of functions.
unsigned short arrays_length(struct superclock *sc);
unsigned short sdl_setup(struct superclock *sc);
unsigned short rectv_populate_res(struct superclock *sc);
unsigned short textv_populate(struct superclock *sc);
struct tm *get_time(struct tm *tt_local);
void time_in_title(const struct tm *tt_local, struct superclock *sc);
void time_to_binary(struct tm tt_local, struct superclock *sc);
void fps_print();
void fps_delay(struct superclock *sc);
Uint32 timer_show_time(Uint32 interval, void* param);
void memory_release_exit(struct superclock *sc);

// Main function that launches the program.
int main(void)
{
    struct tm *tt_local;
    SDL_Event event;
    SDL_TimerID timer;

    struct superclock sc = {
        .win = NULL,
        .rend = NULL,
        .window_width = 290,
        .window_height = 180,
        .running = true,
        .frame_delay = 1000.0f / FPS,
        .show_fps = false,
        .show_time = false,
        .style = 1,
        .offset = 0
    };

    // Make sure arrays were created to the correct length.
    sc.exit_status = arrays_length(&sc);
    if (sc.exit_status)
        memory_release_exit(&sc);

    // Initialize SDL, create window and renderer.
    sc.exit_status = sdl_setup(&sc);
    if (sc.exit_status)
        memory_release_exit(&sc);

    // Create all the Rects and set the Resolution.
    sc.exit_status = rectv_populate_res(&sc);
    if (sc.exit_status)
        memory_release_exit(&sc);

    // Create the 4 textures.
    sc.exit_status = textv_populate(&sc);
    if (sc.exit_status)
        memory_release_exit(&sc);

    while (sc.running) {
        // Check key events, key pressed or released.
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    // handling of close button
                    sc.running = false;
                    break;
                case SDL_USEREVENT:
                    if (event.user.code == 1) {
                        sc.show_time = false;
                        SDL_SetWindowTitle(sc.win, TITLE);
                    }
                    break;
                case SDL_KEYDOWN:
                    // keyboard API for key pressed
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_SPACE:
                            if (sc.show_time)
                                SDL_RemoveTimer(timer);
                            else
                                sc.show_time = true;
                            timer = SDL_AddTimer(5000, timer_show_time, NULL);
                            break;
                        case SDL_SCANCODE_S:
                            if (sc.style < 6)
                                sc.style++;
                            else
                                sc.style = 1;
                            sc.offset = (sc.style < 4 ) ? 0: 2;
                            sc.exit_status = rectv_populate_res(&sc);
                            if (sc.exit_status)
                                memory_release_exit(&sc);
                            break;
                        case SDL_SCANCODE_F:
                            if (sc.show_fps)
                                sc.show_fps = false;
                            else
                                sc.show_fps = true;
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }

        // Get the current time each frame.
        tt_local = get_time(tt_local);

        // Set the window title as the time.
        if (sc.show_time)
            time_in_title(tt_local, &sc);

        // Convert Decemil time to Binary time.
        time_to_binary(*tt_local, &sc);

        // Clear the existing renderer.
        SDL_RenderClear(sc.rend);

        // Draw the images to the renderer.
        for (int i = 0; i < sc.digitc; i++) {
            SDL_RenderCopy(sc.rend, sc.textv[sc.digitv[i].digit + sc.offset], NULL, &sc.digitv[i].rect);
        }

        // Swap the back buffer to the front.
        SDL_RenderPresent(sc.rend);

        // Print FPS to standard output.
        if (sc.show_fps)
            fps_print();

        // Calculate delay needed for the FPS.
        fps_delay(&sc);
    }
    // Release memory and null pointers before exiting.
    memory_release_exit(&sc);
}

// Make sure arrays were created to the correct length.
unsigned short arrays_length(struct superclock *sc) {
    sc->digitc = LEN(sc->digitv);
    sc->textc = LEN(sc->textv);
    if (sc->digitc != DIGITV_LENGTH)
        return 10;
    if (sc->textc != TEXTV_LENGTH)
        return 10;
    return 0;
}


// Initialize SDL, create window and renderer.
unsigned short sdl_setup(struct superclock *sc) {
    // Initialize SDL.
    if (SDL_Init(MY_SDL_FLAGS))
        return 1;

    // Initialize TTF
    if(TTF_Init())
        return 2;

    // Created the SDL Window.
    sc->win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sc->window_width, sc->window_height, 0);
    if (!sc->win)
        return 3;

    // Create the SDL Renderer.
    sc->rend = SDL_CreateRenderer(sc->win, -1, SDL_RENDERER_ACCELERATED);
    if (!sc->rend)
        return 4;

    // load icon image as surface, set as window icon, then release surface.
    SDL_Surface *icon = IMG_Load(ICON);
    if (icon) {
        SDL_SetWindowIcon(sc->win, icon);
        SDL_FreeSurface(icon);
    } else
        return 5;
    return 0;
}

// Create the layout of the Rects and set the Resolution.
unsigned short rectv_populate_res(struct superclock *sc) {
    switch (sc->style) {
        case 2:     // styles 2 and 5 have the same demintions.
        case 5: {
            int x[] = {5, 60, 115};
            int y[] = {5, 50, 95, 140, 185, 230, 275, 320};
            int i = 0;
            for (int ix = 0; ix < LEN(x); ix++ ) {
                for (int iy = 0; iy < LEN(y); iy++ ) {
                    if (i >= sc->digitc)
                        return 6;
                    sc->digitv[i].rect.x = x[ix];
                    sc->digitv[i].rect.y = y[iy];
                    sc->digitv[i].rect.w = 35;
                    sc->digitv[i].rect.h = 35;
                    i++;
                }
            }
            SDL_SetWindowSize(sc->win, 155, 360);
            break;
        }
        case 3:     // styles 3 and 5 have the same demintions.
        case 6: {
            int y[] = {5, 60, 115};
            int x[] = {5, 50, 95, 140, 185, 230, 275, 320};
            int i = 0;
            for (int iy = 0; iy < LEN(y); iy++ ) {
                for (int ix = 0; ix < LEN(x); ix++ ) {
                    if (i >= sc->digitc)
                        return 6;
                    sc->digitv[i].rect.x = x[ix];
                    sc->digitv[i].rect.y = y[iy];
                    sc->digitv[i].rect.w = 35;
                    sc->digitv[i].rect.h = 35;
                    i++;
                }
            }
            SDL_SetWindowSize(sc->win, 360, 155);
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
                    if (i >= sc->digitc)
                        return 6;
                    sc->digitv[i].rect.x = x[ix];
                    sc->digitv[i].rect.y = y[iy];
                    sc->digitv[i].rect.w = 35;
                    sc->digitv[i].rect.h = 35;
                    i++;
                }
            }
            SDL_SetWindowSize(sc->win, 290, 180);
        }
    }
    return 0;
}

// Create the 4 textures.
unsigned short textv_populate(struct superclock *sc) {
    TTF_Font *font = TTF_OpenFont("freesansbold.ttf", 35);
    if (!font)
        return 7;
    SDL_Color colors[2] = {{20, 20, 20}, {223, 223, 223}};
    SDL_Rect off_rect = {8, 3, 35, 35};
    for (int i = 0; i < sc->textc; i++) {
        SDL_Surface *orig_surf = SDL_CreateRGBSurface(0,35,35,32,0,0,0,0);
        if (!orig_surf)
            return 8;
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
            if (!text_surf)
                return 8;
            SDL_BlitSurface(text_surf, NULL, orig_surf, &off_rect);
            SDL_FreeSurface(text_surf);
        }
        sc->textv[i] = SDL_CreateTextureFromSurface(sc->rend, orig_surf);
        if (!sc->textv[i])
            return 9;
        SDL_FreeSurface(orig_surf);
    }
    TTF_CloseFont(font);
    return 0;
}

// Return the current time.
struct tm *get_time(struct tm *tt_local) {
    time_t tt;
    time(&tt);
    return localtime(&tt);
}

// Display the time in the Title.
void time_in_title(const struct tm *tt_local, struct superclock *sc) {
    char time_strg[9];
    snprintf(time_strg, LEN(time_strg), "%02d:%02d:%02d", tt_local->tm_hour, tt_local->tm_min, tt_local->tm_sec);
    SDL_SetWindowTitle(sc->win, time_strg);
}

// Convert Decemil time to Binary time.
void time_to_binary(struct tm tt_local, struct superclock *sc) {
    for (int i = 23; i > 15; i--) {
        sc->digitv[i].digit = tt_local.tm_sec % 2;
        tt_local.tm_sec /= 2;
    }
    for (int i = 15; i > 7; i--) {
        sc->digitv[i].digit = tt_local.tm_min % 2;
        tt_local.tm_min /= 2;
    }
    for (int i = 7; i >= 0; i--) {
        sc->digitv[i].digit = tt_local.tm_hour % 2;
        tt_local.tm_hour /= 2;
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
void fps_delay(struct superclock *sc) {
    static float carry_delay = 0;
    static Uint32 last_time = 0;

    Uint32 current_time = SDL_GetTicks();
    // Checking for roll over
    if (current_time >= last_time) {
        Uint32 elapsed_time = current_time - last_time;
        if ((sc->frame_delay + carry_delay) > elapsed_time) {
            unsigned int current_delay = sc->frame_delay - elapsed_time + carry_delay;
            SDL_Delay(current_delay);
            current_time = SDL_GetTicks();
            // Checking for roll over again.
            if (current_time >= last_time) {
                elapsed_time = current_time - last_time;
                carry_delay = sc->frame_delay - elapsed_time + carry_delay;
            } else {
                carry_delay = 0;
            }
        } else {
            carry_delay = sc->frame_delay - elapsed_time + carry_delay;
        }
    } else {
        carry_delay = 0;
    }
    if (carry_delay > sc->frame_delay)
        carry_delay = sc->frame_delay;
    if (carry_delay < -(sc->frame_delay))
        carry_delay = -(sc->frame_delay);
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
void memory_release_exit(struct superclock *sc) {
    switch (sc->exit_status) {
        case 1:
            fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
            break;
        case 2:
            fprintf(stderr, "Error initializing TTF: %s\n", TTF_GetError());
            break;
        case 3:
            fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
            break;
        case 4:
            fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
            break;
        case 5:
            fprintf(stderr, "Error creating icon surface: %s\n", SDL_GetError());
            break;
        case 6:
            fprintf(stderr, "Populating rectv array went out of bounds!\n");
            break;
        case 7:
            fprintf(stderr, "Error creating font: %s\n", TTF_GetError());
            break;
        case 8:
            fprintf(stderr, "Error creating a surface: %s\n", SDL_GetError());
            break;
        case 9:
            fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        case 10:
            fprintf(stderr, "Error an array was not the expected length:\n");
        default:
            break;
    }

    for (int i = 0; i < sc->textc; i++) {
        SDL_DestroyTexture(sc->textv[i]);
        sc->textv[i] = NULL;
    }
    SDL_DestroyRenderer(sc->rend);
    sc->rend = NULL;
    SDL_DestroyWindow(sc->win);
    sc->win = NULL;
    SDL_Quit();
    exit(sc->exit_status);
}
