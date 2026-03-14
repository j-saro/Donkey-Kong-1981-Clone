#include "core/cmdline.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static void print_usage(const char *prog_name) {
    printf("Usage: %s [OPTIONS]\n", prog_name);
    printf("Donkey Kong - A classic arcade game clone.\n\n");
    printf("  -x, --width=WIDTH      Set window width (X-axis) (default: 600)\n");
    printf("  -y, --height=HEIGHT    Set window height (Y-axis) (default: 600)\n");
    printf("  -f, --fullscreen       Start in fullscreen mode\n");
    printf("  -l, --level=LEVEL      Start directly at a specific level number (1-4)\n");
    printf("  -p, --data-path=DIR    Specify folder path to game data files\n");
}

int parse_cmdline(int argc, char **argv, arguments_t *arguments) {
    int opt;
    
    // Define long options to match your old argp structure
    static struct option long_options[] = {
        {"width",      required_argument, 0, 'x'},
        {"height",     required_argument, 0, 'y'},
        {"fullscreen", no_argument,       0, 'f'},
        {"level",      required_argument, 0, 'l'},
        {"data-path",  required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "x:y:fl:p:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'x': // x-axis/Width
                arguments->width = atoi(optarg);
                if (arguments->width <= 0) { fprintf(stderr, "Error: Width must be positive.\n"); return 1; }
                break;
            case 'y': // y-axis/Height
                arguments->height = atoi(optarg);
                if (arguments->height <= 0) { fprintf(stderr, "Error: Height must be positive.\n"); return 1; }
                break;
            case 'f': // Fullscreen
                arguments->fullscreen = TRUE;
                break;
            case 'l': // Level Start
                arguments->start_level = atoi(optarg);
                if (arguments->start_level < 0 || arguments->start_level > MAX_LEVELS) { fprintf(stderr, "Error: Invalid level.\n"); return 1; }
                break;
            case 'p': // Data path
                arguments->level_folder_path = optarg;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Logic for fullscreen conflict (replaces ARGP_KEY_END)
    if (arguments->fullscreen && (arguments->width != BASE_WIDTH || arguments->height != BASE_HEIGHT)) {
        g_message("Fullscreen specified, ignoring custom width/height");
    }

    return 0;
}
