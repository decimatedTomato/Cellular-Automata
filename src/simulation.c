#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

// Portability issues
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080


// My opengl functions file
#include "handle_opengl.c"

// USER DEFINITIONS
#define DEFAULT_WINDOW_WIDTH SCREEN_WIDTH/3
#define DEFAULT_WINDOW_HEIGHT SCREEN_HEIGHT/3
// #define DEFAULT_WINDOW_WIDTH 640
// #define DEFAULT_WINDOW_HEIGHT 480

#define GRID_WIDTH 50U
#define GRID_HEIGHT 50U

#define VERTEX_SHADER_FILE_PATH "res/Shaders/vertex_old.glsl"
#define SHADER_FILE_PATH "res/Shaders/fragment_old.glsl"
#define PROCESSING_SHADER_FILE_PATH "res/Shaders/GoL.glsl"

#define START_PAUSED true

// DISPLAY
typedef enum DisplayMode {
    PLAYING, PAUSED
} DisplayMode;

DisplayMode mode = PLAYING;
int window_width = DEFAULT_WINDOW_WIDTH;
int window_height = DEFAULT_WINDOW_HEIGHT;
bool is_fullscreen = false;

// BUFFERS
uint32_t starting_grid[GRID_HEIGHT * GRID_WIDTH];
GLuint frameBuffer;

// IMAGES
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define TARGET_IMAGE "res/Image/texture.jpg"

void load_image_texture(int slot, unsigned char **img) {
    stbi_set_flip_vertically_on_load(1);
    int width, height, channels;
    *img = stbi_load(TARGET_IMAGE, &width, &height, &channels, 0);
    // printf("width %d height %d channels %d\n", width, height, channels);
    if (img == NULL) {
        printf("Error loading the image\n");
        exit(1);
    }

    GLuint texture_map;
    glBindTexture(GL_TEXTURE_2D, texture_map);
    glGenTextures(1, &texture_map);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glActiveTexture(GL_TEXTURE0 + slot);
    // Made for jpeg (3 channels)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    stbi_image_free(img);
}

void init_grid() {
    for (size_t j = 0; j < GRID_HEIGHT; j++)
        for (size_t i = 0; i < GRID_WIDTH; i++) {
            starting_grid[j * GRID_HEIGHT + i] = (rand() % 3 == 0) ? 0xFF0000FF : 0xFF000000;
        }
}

void init_texture() {
    // For grid
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GRID_WIDTH, GRID_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, starting_grid);
    GLuint starting_texture = 0;

    // For frameBuffer
    glGenFramebuffersEXT(1, &frameBuffer); // This is where GoL output will go, this will be drawn
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer); // Prepare to draw to it
    // Enter starting texture
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, starting_texture, 0);

    // Draw normally

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // Unbind the framebuffer
}

/* Update everything in simulation */
void update() {
    
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GRID_WIDTH, GRID_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, starting_grid);
}

void load() {
    // load from save
    render_frame();
}

void pause() {
    if (mode == PAUSED) mode = PLAYING;
    else if (mode == PLAYING) mode = PAUSED;
}

void step() {
    mode = PAUSED;
    update();
    render_frame();
}

void save() {
    // save
}

int main() {
    long long unsigned int t = time(NULL);
    printf("Random seed %llu\n", t);
    srand(t);

    if(init_GLFW(window_width, window_height, "Simulation") == -1) exit(1);
    init_Debug_Callback();
    init_Quad();

    unsigned int shader_rendering = init_Shader(VERTEX_SHADER_FILE_PATH, SHADER_FILE_PATH);
    set_shader(shader_rendering);
    init_Uniforms();
    unsigned int shader_processing = init_shader(VERTEX_SHADER_FILE_PATH, PROCESSING_SHADER_FILE_PATH);
    set_shader(shader_processing);
    init_Uniforms();

    init_grid();
    init_texture(); // input textures such as noise textures

    /* Loop until the user closes the window */
    bool keep_running = true;
    while (keep_running) {
        take_user_input();
        keep_running = render_frame();
        if (mode != PAUSED) update();
    }

    glDeleteFramebuffers(1, &frameBuffer);
    clean_up();
    return 0;
}