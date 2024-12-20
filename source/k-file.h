/*
 * k-file.h - file manipulation functions
 *
 * Written by Hampus Fridholm
 */

#ifndef K_FILE_H
#define K_FILE_H

extern size_t file_write(const void* pointer, size_t size, const char* filepath);

extern size_t file_read(void* pointer, size_t size, const char* filepath);

extern size_t file_size_get(const char* filepath);

#endif // K_FILE_H
