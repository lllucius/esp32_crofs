// Copyright 2017-2018 Leland Lucius
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//
// Demonstrates how to mount and use an embedded CroFS filesystem
//

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "crofs.h"

extern "C"
{
    extern const uint8_t _binary_crofs_start[];
    extern const uint8_t _binary_crofs_size;
}

void walk(const char *parent, const char *name, int level)
{
    DIR *dir = opendir(parent);
    if (dir == NULL)
    {
        printf("Couldn't open directory %s: %s\n", parent, strerror(errno));
        abort();
    }

    if (level == 0)
    {
        printf("/\n");
    }

    while (true)
    {
        errno = 0;
        struct dirent *de = readdir(dir);
        if (de == NULL)
        {
            if (errno != 0)
            {
                printf("Couldn't read directory %s: %s\n", parent, strerror(errno));
                abort();
            }
            break;
        }

        for (int i = 0; i < level; i++)
        {
            printf("|   ");
        }
        printf("|-- %s%s", de->d_name, de->d_type == DT_DIR ? "/\n" : "");

        int len = strlen(parent) + 1 +  strlen(de->d_name)+ 1;
        char *path = (char *) malloc(len);
        if (path == NULL)
        {
            printf("Insufficient memory for path buffer\n");
            abort();
        }
        snprintf(path, len, "%s/%s", parent, de->d_name);

        if (de->d_type == DT_DIR)
        {
            walk(path, de->d_name, level + 1);
        }
        else if (de->d_type == DT_REG)
        {
            FILE *f = fopen(path, "rb");
            if (f == NULL)
            {
                printf("Unable to open %s: %s", path, strerror(errno));
                abort();
            }

            size_t bytes = 0;
            do
            {
                char buf[256];
                bytes += fread(buf, 1, sizeof(buf), f);
                if (ferror(f))
                {
                    printf("Error reading %s: %s", path, strerror(errno));
                    abort();
                }
            } while (!feof(f));

            struct stat st;
            if (fstat(fileno(f), &st) != 0)
            {
                printf("Unable to fstat() %s: %s\n", path, strerror(errno));
                abort();
            }

            printf(" (usize=%ld, csize=%ld, bytes read=%zd)\n", st.st_size, st.st_blocks, bytes);

            if (fclose(f) != 0)
            {
                printf("Unable to close %s: %s", path, strerror(errno));
                abort();
            }

        }

        free(path);
    }

    if (closedir(dir) == -1)
    {
        printf("Failed to close directory %s: %s\n", parent, strerror(errno));
        abort();
    }

    return;
}

extern "C" void app_main(void *)
{
    const crofs_config_t cfg =
    {
        .fs_addr = CroFS::get_embedded_start(),
        .fs_size = CroFS::get_embedded_size(),
        .base_path = CONFIG_CROFS_EXAMPLE_MOUNT_POINT,
        .open_files = CONFIG_CROFS_EXAMPLE_OPEN_FILES
    };

    CroFS crofs;

    if (crofs.init(&cfg) != 0)
    {
        printf("CroFS init() failed: %s\n", strerror(errno));
        abort();
    }

    walk(CONFIG_CROFS_EXAMPLE_MOUNT_POINT "/", "", 0);

    vTaskDelay(portMAX_DELAY);
}

