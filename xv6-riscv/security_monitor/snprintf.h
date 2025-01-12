#include <stddef.h>
void int_to_str(int value, char *str, size_t *index, size_t max_len);
void ptr_to_hex(void *ptr, char *str, size_t *index, size_t max_len);
int snprintf(char *str, size_t size, const char *format, ...);
