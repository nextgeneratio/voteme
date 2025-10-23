#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "csv_io.h"

// Local helpers
int validate_file_access(const char *filename, const char *mode)
{
    if (!filename)
    {
        set_error_message("Error: filename cannot be NULL");
        return 0;
    }

    struct stat file_stat;

    // Check if file exists for read operations
    if (strchr(mode, 'r'))
    {
        if (stat(filename, &file_stat) != 0)
        {
            set_error_message("Error: File '%s' does not exist or cannot be accessed", filename);
            return 0;
        }

        if (access(filename, R_OK) != 0)
        {
            set_error_message("Error: No read permission for file '%s'", filename);
            return 0;
        }
    }

    // Check write permissions for write/append
    if (strchr(mode, 'w') || strchr(mode, 'a'))
    {
        // safe strdup
        char *dir = NULL;
        {
            size_t len = strlen(filename) + 1;
            dir = malloc(len);
            if (!dir)
            {
                set_error_message("Error: Memory allocation failed for path duplication");
                return 0;
            }
            memcpy(dir, filename, len);
        }
        if (!dir)
        {
            set_error_message("Error: Memory allocation failed for path duplication");
            return 0;
        }
        char *last_slash = strrchr(dir, '/');
        if (last_slash)
        {
            *last_slash = '\0';
            if (access(dir, W_OK) != 0)
            {
                set_error_message("Error: No write permission for directory of file '%s'", filename);
                free(dir);
                return 0;
            }
        }
        free(dir);
    }

    return 1;
}

static int validate_string_input(const char *str, const char *param_name, size_t max_length)
{
    if (!str)
    {
        set_error_message("Error: %s cannot be NULL", param_name);
        return 0;
    }

    if (strlen(str) == 0)
    {
        set_error_message("Error: %s cannot be empty", param_name);
        return 0;
    }

    if (strlen(str) >= max_length)
    {
        set_error_message("Error: %s exceeds maximum length of %zu characters", param_name, max_length);
        return 0;
    }

    if (strchr(str, '\n') || strchr(str, '\r'))
    {
        set_error_message("Error: %s contains invalid newline characters", param_name);
        return 0;
    }

    return 1;
}

int read_csv_line(FILE *fp, char *fields[], int max_fields, char delimiter)
{
    if (!fp || !fields || max_fields <= 0)
    {
        set_error_message("Error: Invalid parameters for read_csv_line");
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    if (!fgets(line, sizeof(line), fp))
    {
        if (feof(fp))
        {
            return 0; // EOF is not an error
        }
        set_error_message("Error: Failed to read line from file");
        return 0;
    }

    if (strlen(line) >= MAX_LINE_LENGTH - 1 && line[MAX_LINE_LENGTH - 2] != '\n')
    {
        set_error_message("Error: Line exceeds maximum length of %d characters", MAX_LINE_LENGTH);
        return 0;
    }

    int i = 0;
    char delim_str[2] = {delimiter, '\0'};
    char *token = strtok(line, delim_str);
    while (token && i < max_fields)
    {
        while (*token == ' ' || *token == '\t')
            token++;
        char *end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r'))
        {
            *end-- = '\0';
        }

        // duplicate token safely
        char *copy = NULL;
        {
            size_t len = strlen(token) + 1;
            copy = malloc(len);
            if (!copy)
            {
                set_error_message("Error: Memory allocation failed while parsing CSV fields");
                for (int j = 0; j < i; j++)
                {
                    free(fields[j]);
                }
                return 0;
            }
            memcpy(copy, token, len);
        }
        if (!copy)
        {
            set_error_message("Error: Memory allocation failed while parsing CSV fields");
            for (int j = 0; j < i; j++)
            {
                free(fields[j]);
            }
            return 0;
        }
        fields[i++] = copy;
        token = strtok(NULL, delim_str);
    }

    return i;
}

int append_line(const char *filename, const char *line)
{
    if (!validate_string_input(filename, "filename", MAX_LINE_LENGTH) ||
        !validate_string_input(line, "line", MAX_LINE_LENGTH))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    if (!validate_file_access(filename, "a"))
    {
        return DATA_ERROR_PERMISSION_DENIED;
    }

    FILE *fp = fopen(filename, "a");
    if (!fp)
    {
        if (errno == ENOSPC)
        {
            set_error_message("Error: No space left on device for file '%s'", filename);
            return DATA_ERROR_DISK_FULL;
        }
        set_error_message("Error: Cannot open file '%s' for appending: %s", filename, strerror(errno));
        return DATA_ERROR_PERMISSION_DENIED;
    }

    if (fprintf(fp, "%s\n", line) < 0)
    {
        set_error_message("Error: Failed to write to file '%s': %s", filename, strerror(errno));
        fclose(fp);
        return DATA_ERROR_DISK_FULL;
    }

    if (fclose(fp) != 0)
    {
        set_error_message("Error: Failed to close file '%s': %s", filename, strerror(errno));
        return DATA_ERROR_DISK_FULL;
    }

    return DATA_SUCCESS;
}

int overwrite_file(const char *filename, const char *content)
{
    if (!validate_string_input(filename, "filename", MAX_LINE_LENGTH) || !content)
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    if (strlen(content) > MAX_FILE_SIZE)
    {
        set_error_message("Error: Content size exceeds maximum file size limit");
        return DATA_ERROR_BUFFER_OVERFLOW;
    }

    char backup_name[MAX_LINE_LENGTH + 10];
    snprintf(backup_name, sizeof(backup_name), "%s.backup", filename);

    FILE *original = fopen(filename, "r");
    if (original)
    {
        FILE *backup = fopen(backup_name, "w");
        if (backup)
        {
            char buffer[1024];
            size_t bytes;
            while ((bytes = fread(buffer, 1, sizeof(buffer), original)) > 0)
            {
                fwrite(buffer, 1, bytes, backup);
            }
            fclose(backup);
        }
        fclose(original);
    }

    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        set_error_message("Error: Cannot open file '%s' for writing: %s", filename, strerror(errno));
        return DATA_ERROR_PERMISSION_DENIED;
    }

    if (fprintf(fp, "%s", content) < 0)
    {
        set_error_message("Error: Failed to write content to file '%s': %s", filename, strerror(errno));
        fclose(fp);
        rename(backup_name, filename);
        return DATA_ERROR_DISK_FULL;
    }

    if (fclose(fp) != 0)
    {
        set_error_message("Error: Failed to close file '%s': %s", filename, strerror(errno));
        rename(backup_name, filename);
        return DATA_ERROR_DISK_FULL;
    }

    remove(backup_name);
    return DATA_SUCCESS;
}
