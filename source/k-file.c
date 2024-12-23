/*
 * k-file.c - read and write to file
 */

#include <stdio.h>
#include <stddef.h>

/*
 * Get the size of the file at the supplied path
 *
 * The function returns the number of bytes in the file
 *
 * PARAMS
 * - const char* filepath | Path to file
 *
 * RETURN (size_t size)
 * - 0  | Failed to open file, or file is empty
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
 * Read a number of bytes from file to memory at pointer
 *
 * The function returns the number of read bytes
 *
 * PARAMS
 * - void*       pointer  | Pointer to memory buffer
 * - size_t      size     | Number of bytes to read
 * - const char* filepath | Path to file
 *
 * RETURN (same as fread, size_t read_size)
 * - 0  | Failed to read file, or bad input
 * - >0 | Number of read bytes
 */
size_t file_read(void* pointer, size_t size, const char* filepath)
{
  if(!pointer) return 0;

  FILE* stream = fopen(filepath, "rb");

  if(!stream) return 0;

  size_t read_size = fread(pointer, 1, size, stream);

  fclose(stream);

  return read_size;
}

/*
 * Write a number of bytes from memory at pointer to file
 *
 * The function returns the number of written bytes
 *
 * PARAMS
 * - const void* pointer  | Address to write data to
 * - size_t      size     | Number of bytes to write
 * - const char* filepath | Path to file
 *
 * RETURN (same as fwrite, size_t write_size)
 * - 0  | Error
 * - >0 | Success!
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
