#ifndef DATA_ERRORS_H
#define DATA_ERRORS_H

// Error codes for comprehensive error reporting
typedef enum
{
    DATA_SUCCESS = 1,
    DATA_ERROR_FILE_NOT_FOUND = -1,
    DATA_ERROR_PERMISSION_DENIED = -2,
    DATA_ERROR_INVALID_INPUT = -3,
    DATA_ERROR_MEMORY_ALLOCATION = -4,
    DATA_ERROR_BUFFER_OVERFLOW = -5,
    DATA_ERROR_MALFORMED_DATA = -6,
    DATA_ERROR_RECORD_NOT_FOUND = -7,
    DATA_ERROR_DISK_FULL = -8
} data_error_t;

// Returns a pointer to the last error message (static storage)
const char *get_last_error(void);

// Internal: set error message (exposed for reuse across modules)
void set_error_message(const char *format, ...);

#endif // DATA_ERRORS_H
