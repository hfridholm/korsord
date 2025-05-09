/*
 * k-file.c - korsord file functions
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

/*
 *
 */
int grid_file_get(char* file, char* name)
{
  if (!file || !name)
  {
    return 1;
  }

  if (sprintf(file, "%s/.korsord/grids/%s.grid", getenv("HOME"), name) < 0)
  {
    return 2;
  }

  return 0;
}

/*
 *
 */
int clues_file_get(char* file, char* name)
{
  if (!file || !name)
  {
    return 1;
  }

  if (sprintf(file, "%s/.korsord/clues/%s.clues", getenv("HOME"), name) < 0)
  {
    return 2;
  }

  return 0;
}

/*
 *
 */
int words_file_get(char* file, char* name)
{
  if (!file || !name)
  {
    return 1;
  }

  if (sprintf(file, "%s/.korsord/words/%s.words", getenv("HOME"), name) < 0)
  {
    return 2;
  }

  return 0;
}

/*
 *
 */
int model_file_get(char* file, char* name)
{
  if (!file || !name)
  {
    return 1;
  }

  if (sprintf(file, "%s/.korsord/models/%s.model", getenv("HOME"), name) < 0)
  {
    return 2;
  }

  return 0;
}
