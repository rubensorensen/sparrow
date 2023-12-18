#include "utils.h"
#include "log.h"

#include <string.h>
#include <errno.h>

b32
slurp_file(const char *filename, char **dest, size_t *bytes_read)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        LOG_ERROR("Could not open file %s: %s\n", filename, strerror(errno));
        return false;
    }

    fseek(f, 0L, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0L, SEEK_SET);

    *dest = (char *)malloc(file_size * sizeof(char));
    if (!*dest) {
        LOG_ERROR("Could not allocate %zu bytes: %s\n", file_size, strerror(errno));
        return false;
    }

    if (fread(*dest, 1, file_size, f) != file_size) {
        LOG_ERROR("Could not read entire file for some reason\n");
        return false;
    }

    fclose(f);

    *bytes_read = file_size;

    return true;
}
