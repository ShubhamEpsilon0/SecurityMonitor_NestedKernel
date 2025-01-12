#include <stddef.h>
#include <stdarg.h>

#include "snprintf.h"

#include "kernel/types.h"

// Helper function to convert an integer to a string
void int_to_str(int value, char *str, size_t *index, size_t max_len)
{
    char temp[12]; // Buffer to hold integer digits (max digits for 32-bit int + sign)
    int is_negative = 0;
    int i = 0;

    if (value < 0)
    {
        is_negative = 1;
        value = -value;
    }

    // Generate digits in reverse order
    do
    {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    if (is_negative)
    {
        temp[i++] = '-';
    }

    // Write characters to the output in reverse order
    while (i > 0 && *index < max_len - 1)
    {
        str[(*index)++] = temp[--i];
    }
}

void ptr_to_hex(void *ptr, char *str, size_t *index, size_t max_len)
{
    uint64 value = (uint64)ptr;
    char temp[16]; // Buffer for hexadecimal digits
    int i = 0;

    // Generate digits in reverse order
    do
    {
        int digit = value % 16;
        temp[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= 16;
    } while (value > 0);

    // Add "0x" prefix
    if (*index < max_len - 1)
    {
        str[(*index)++] = '0';
    }
    if (*index < max_len - 1)
    {
        str[(*index)++] = 'x';
    }

    // Write characters to the output in reverse order
    while (i > 0 && *index < max_len - 1)
    {
        str[(*index)++] = temp[--i];
    }
}

// Main snprintf implementation
int snprintf(char *str, size_t size, const char *format, ...)
{
    va_list args;
    size_t index = 0; // Current position in the output buffer
    const char *p = format;

    if (size == 0)
    {
        return 0; // No space to write any characters
    }

    va_start(args, format);

    while (*p != '\0')
    {
        if (*p == '%' && *(p + 1) != '\0')
        {
            p++; // Skip the '%' character
            if (*p == 'd')
            { // Integer
                int value = va_arg(args, int);
                int_to_str(value, str, &index, size);
            }
            else if (*p == 's')
            { // String
                const char *value = va_arg(args, const char *);
                while (*value != '\0' && index < size - 1)
                {
                    str[index++] = *value++;
                }
            }
            else if (*p == 'c')
            { // Character
                char value = (char)va_arg(args, int);
                if (index < size - 1)
                {
                    str[index++] = value;
                }
            }
            else if (*p == 'p')
            { // Pointer
                void *value = va_arg(args, void *);
                ptr_to_hex(value, str, &index, size);
            }
            else
            { // Unsupported specifier, treat as literal
                if (index < size - 1)
                {
                    str[index++] = '%';
                }
                if (index < size - 1)
                {
                    str[index++] = *p;
                }
            }
        }
        else
        { // Regular character
            if (index < size - 1)
            {
                str[index++] = *p;
            }
        }
        p++;
    }

    va_end(args);

    // Null-terminate the string
    str[index] = '\0';

    return (int)index;
}