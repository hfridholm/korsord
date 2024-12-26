/*
 * file.h - file read and write functions
 *
 * Written by Hampus Fridholm
 *
 * Last updated: 2024-12-26
 *
 *
 * In main compilation unit; define FILE_IMPLEMENT
 *
 *
 * FUNCTIONS:
 *
 * size_t file_size_get(const char* filepath)
 * 
 * size_t file_read(void* pointer, size_t size, const char* filepath)
 * 
 * size_t file_write(const void* pointer, size_t size, const char* filepath)
 * 
 * 
 * size_t dir_file_size_get(const char* dirpath, const char* name)
 * 
 * size_t dir_file_write(const void* pointer, size_t size, const char* dirpath, const char* name)
 * 
 * size_t dir_file_read(void* pointer, size_t size, const char* dirpath, const char* name)
 */

/*
 * From here on, until FILE_IMPLEMENT,
 * it is like a normal header file with declarations
 */

#ifndef FILE_H
#define FILE_H

extern size_t file_size_get(const char* filepath);

extern size_t file_read(void* pointer, size_t size, const char* filepath);

extern size_t file_write(const void* pointer, size_t size, const char* filepath);


extern size_t dir_file_size_get(const char* dirpath, const char* name);

extern size_t dir_file_write(const void* pointer, size_t size, const char* dirpath, const char* name);

extern size_t dir_file_read(void* pointer, size_t size, const char* dirpath, const char* name);

#endif // FILE_H

/*
 * This header library file uses _IMPLEMENT guards
 *
 * If FILE_IMPLEMENT is defined, the definitions will be included
 */

#ifdef FILE_IMPLEMENT

#include <stdio.h>
#include <string.h>

/*
 * Get the size of a file
 *
 * PARAMS
 * - const char* filepath | Path to file
 *
 * RETURN (size_t size)
 * - 0  | Error
 * - >0 | Number of bytes in file
 */
size_t file_size_get(const char* filepath)
{
  FILE* stream = fopen(filepath, "rb");

  if(!stream) return 0;

  fseek(stream, 0, SEEK_END); 

  size_t size = ftell(stream);

  fseek(stream, 0, SEEK_SET); 

  fclose(stream);

  return size;
}

/*
 * Write a number of bytes from memory at pointer to file
 *
 * PARAMS
 * - const void* pointer  | Address to write data to
 * - size_t      size     | Number of bytes to write
 * - const char* filepath | Path to file
 *
 * RETURN (same as fwrite, size_t write_size)
 * - 0  | Error
 * - >0 | The number of written bytes
 */
size_t file_write(const void* pointer, size_t size, const char* filepath)
{
  if(!pointer) return 0;

  FILE* stream = fopen(filepath, "wb");

  if(!stream) return 0;

  size_t write_size = fwrite(pointer, 1, size, stream);

  fclose(stream);

  return write_size;
}

/*
 * Read a number of bytes from file to memory at pointer
 *
 * PARAMS
 * - void*       pointer  | Address to store read data
 * - size_t      size     | Number of bytes to read
 * - const char* filepath | Path to file
 *
 * RETURN (same as fread, size_t read_size)
 * - 0  | Error
 * - >0 | Number of read bytes
 */
size_t file_read(void* pointer, size_t size, const char* filepath)
{
  if(!pointer) return 0;

  FILE* stream = fopen(filepath, "rb");

  if(stream == NULL) return 0;

  int status = fread(pointer, 1, size, stream);

  fclose(stream);

  return status;
}

/*
 * Read from file inside directory
 *
 * PARAMS
 * - const void* pointer | Address to read data from
 * - size_t      size    | Number of bytes to read
 * - const char* dirpath | Path to directory
 * - const char* name    | Name of file
 *
 * RETURN (same as fread, size_t read_size)
 * - 0  | Error
 * - >0 | Number of read bytes
 */
size_t dir_file_read(void* pointer, size_t size, const char* dirpath, const char* name)
{
  size_t path_size = strlen(dirpath) + 1 + strlen(name);

  char filepath[path_size + 1];

  sprintf(filepath, "%s/%s", dirpath, name);

  return file_read(pointer, size, filepath);
}

/*
 * Write to file inside directory
 *
 * PARAMS
 * - const void* pointer | Address to write data to
 * - size_t      size    | Number of bytes to write
 * - const char* dirpath | Path to directory
 * - const char* name    | Name of file
 *
 * RETURN (same as fwrite, size_t write_size)
 * - 0  | Error
 * - >0 | The number of written bytes
 */
size_t dir_file_write(const void* pointer, size_t size, const char* dirpath, const char* name)
{
  size_t path_size = strlen(dirpath) + 1 + strlen(name);

  char filepath[path_size + 1];

  sprintf(filepath, "%s/%s", dirpath, name);

  return file_write(pointer, size, filepath);
}

/*
 * Get size of file inside directory
 *
 * PARAMS
 * - const char* dirpath | Path to directory
 * - const char* name    | Name of file
 *
 * RETURN (size_t size)
 * - 0  | Error
 * - >0 | Number of bytes in file
 */
size_t dir_file_size_get(const char* dirpath, const char* name)
{
  size_t path_size = strlen(dirpath) + 1 + strlen(name);

  char filepath[path_size + 1];

  sprintf(filepath, "%s/%s", dirpath, name);

  return file_size_get(filepath);
}

#endif // FILE_IMPLEMENT
