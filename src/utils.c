#include "utils.h"
#include "log.h"

#include <string.h>
#include <errno.h>

b32
slurp_file(const char *filename, char **dest, size_t *bytes_read)
{
	if (!filename) {
		LOG_ERROR("filename is NULL");
		return false;
	}
	
	if (!dest) {
		LOG_ERROR("dest is NULL");
		return false;
	}
	
	FILE *f = fopen(filename, "rb");
	if (!f) {
		LOG_ERROR("Could not open file %s: %s", filename, strerror(errno));
		return false;
	}
	
	fseek(f, 0, SEEK_END);
	size_t file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *string = malloc(file_size + 1);
	if (!string) {
		LOG_ERROR("Could not allocate %zu bytes: %s", file_size + 1, strerror(errno));
		return false;
	}
	size_t read = fread(string, 1, file_size, f);
	if (read != file_size) {
		LOG_ERROR("Did not read all bytes of file %s. Expected %zu, read %zu",
				  filename, file_size, read);
		return false;
	}
	fclose(f);

	string[file_size] = '\0';
	
	*dest = string;
    *bytes_read = file_size;

    return true;
}
