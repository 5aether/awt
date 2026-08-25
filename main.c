#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_DIR ".config/awt"
#define CONFIG_FILE ".config/awt/config.ini"

void print_help() {
    printf("\n  \e[1mawt\e[0m — Ultimate Mpvpaper Engine\n\n");
    printf("  \e[36mUsage:\e[0m\n    awt [command] [args] [--fps=VAL]\n\n");
    printf("  \e[36mCommands:\e[0m\n    set [path] [--fps=VAL]  Set and apply wallpaper with custom FPS\n    start                   Start saved wallpaper from config\n    stop                    Stop wallpaper\n    help                    Show this help\n\n");
}

void expand_and_normalize(const char *input, char *output) {
    char *home = getenv("HOME");
    if (strncmp(input, "~/", 2) == 0) snprintf(output, 1024, "%s/%s", home, input + 2);
    else if (strncmp(input, "$HOME/", 6) == 0) snprintf(output, 1024, "%s/%s", home, input + 6);
    else strcpy(output, input);
}

void get_wallpaper_from_cfg(char *out, size_t size, int *fps_out) {
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", getenv("HOME"), CONFIG_FILE);
    FILE *f = fopen(path, "r");
    char line[1024];
    *fps_out = 60; 
    while (f && fgets(line, sizeof(line), f)) {
        if (strncmp(line, "wallpaper=", 10) == 0) {
            char *val = line + 10;
            val[strcspn(val, "\r\n")] = 0;
            strcpy(out, val);
        } else if (strncmp(line, "fps=", 4) == 0) {
            *fps_out = atoi(line + 4);
        }
    }
    if (f) fclose(f);
}

void save_wallpaper_to_cfg(const char *full_path, int fps) {
    char mkdir_cmd[1024];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s/%s", getenv("HOME"), CONFIG_DIR);
    system(mkdir_cmd);
    char cfg_path[1024];
    snprintf(cfg_path, sizeof(cfg_path), "%s/%s", getenv("HOME"), CONFIG_FILE);
    FILE *f = fopen(cfg_path, "w");
    if (f) {
        fprintf(f, "wallpaper=%s\n", full_path);
        fprintf(f, "fps=%d\n", fps);
        fclose(f);
    }
}

void apply_wallpaper(const char *full_path, int fps, int is_start) {
    system("pkill mpvpaper");
    char cmd[2048];
    
    snprintf(cmd, sizeof(cmd), 
        "mpvpaper '*' \"%s\" -o \"loop --no-audio --hwdec=auto-safe --vo=gpu --gpu-api=vulkan --video-sync=audio --cache=yes --demuxer-max-bytes=500M --hr-seek-framedrop=no --fps=%d\" >/dev/null 2>&1 &", 
        full_path, fps);
        
    system(cmd);
    if (is_start) {
        printf("Wallpaper started\n");
    } else {
        printf("Applied and saved in ~/.config/awt/config.ini\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help();
        return 0;
    }

    if (strcmp(argv[1], "set") == 0 && argc >= 3) {
        char full_path[1024];
        expand_and_normalize(argv[2], full_path);
        
        int fps = 60; 
        if (argc >= 4 && strncmp(argv[3], "--fps=", 6) == 0) {
            fps = atoi(argv[3] + 6);
            if (fps <= 0) fps = 60;
        }

        save_wallpaper_to_cfg(full_path, fps);
        apply_wallpaper(full_path, fps, 0);
        return 0;
    }

    if (strcmp(argv[1], "start") == 0) {
        char full_path[1024];
        int fps = 60;
        get_wallpaper_from_cfg(full_path, sizeof(full_path), &fps);
        if (strlen(full_path) > 0) {
            apply_wallpaper(full_path, fps, 1);
        } else {
            printf("No wallpaper found in config: ~/%s\n", CONFIG_FILE);
        }
        return 0;
    }

    if (strcmp(argv[1], "stop") == 0) {
        system("pkill mpvpaper");
        printf("Wallpaper stopped\n");
        return 0;
    }

    print_help();
    return 0;
}
