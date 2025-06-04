#include "core/cmdline.h"

const char *argp_program_version = "v1.0";
static char doc[] = "Donkey Kong - A classic arcade game clone.";
static char args_doc[] = "";

static struct argp_option options[] = {
    {"width",      'w', "WIDTH",  0, "Set game window width (default: 600)", 0},
    {"height",     'h', "HEIGHT", 0, "Set game window height (default: 600)", 0},
    {"fullscreen", 'f', 0,        0, "Start in fullscreen mode", 0},
    {"level",      'l', "LEVEL",  0, "Start directly at a specific level number (1-4)", 0},
    {"data-path",  'p', "DIR",    0, "Specify folder path to game data files\n(default: current directory)", 0},
    {0},
};

error_t parse_options(int key, char *arg, struct argp_state *state) {
    arguments_t *arguments = state->input;

    switch (key) {
        case 'w': // Width
            arguments->width = atoi(arg);
            if (arguments->width <= 0) {
                argp_error(state, "Width must be a positive integer.");
            }
            break;
        
        case 'h': // Height
            arguments->height = atoi(arg);
            if (arguments->height <= 0) {
                argp_error(state, "Height must be a positive integer.");
            }
            break;
        
        case 'f': // Fullscreen
            arguments->fullscreen = TRUE;
            break;
        
        case 'l': // Level Start
            arguments->start_level = atoi(arg);
            if (arguments->start_level < 0 || arguments->start_level > MAX_LEVELS) {
                argp_error(state, "Invalid level: Choose 0 (menu) or 1-%d.", MAX_LEVELS);
            }
            break;
        
        case 'p': // Data path
            arguments->level_folder_path = arg;
            break;
        
        case ARGP_KEY_END:
            if (arguments->fullscreen && (arguments->width != BASE_WIDTH || arguments->height != BASE_HEIGHT)) {
                g_message("Fullscreen specified, ignoring custom width/height");
            }
            break;
        
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int parse_cmdline(int argc, char **argv, arguments_t *arguments) {
	struct argp parser = {options, parse_options, args_doc, doc};
	int ret = argp_parse(&parser, argc, argv, 0, 0, arguments);
}