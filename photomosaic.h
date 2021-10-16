/* Biblioteca feita por Luis Felipe Risch para o trabalho 1-fotomosaico de programação 2 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>

#define RGB_COMPONENT_COLOR 255
#define SIZE 1024

/* Estrutura que representa dados de um pixel de um ppm P6 */
typedef struct
{
  unsigned char r, g, b;
} P6_PIXEL_STRUCT;

/* Estrutura que representa dados de um pixel de um ppm P3 */
typedef struct
{
  int r, g, b;
} P3_PIXEL_STRUCT;

/* Estrutura que representa uma imagem ppm  */
typedef struct
{
  char type[3];          /* Tipo do ppm: P3 ou P6 */
  int width;             /* Largura */
  int height;            /* Altura */
  int max;               /* Valor máximo de um componente RGB */
  double red_avarage;    /* Média da soma do componente vermelho dos pixels da imagem ppm */
  double green_avarage;  /* Média da soma do componente verde dos pixels da imagem ppm */
  double blue_avarage;   /* Média da soma do componente azul dos pixels da imagem ppm */
  P3_PIXEL_STRUCT *data; /* Vetor de estruturas de pixel*/
} P3_IMAGE_STRUCT;

P3_IMAGE_STRUCT *read_image(FILE *file); 

void *allocate_array_tiles(int qntTiles); 

void build_mosaic(P3_IMAGE_STRUCT *image, P3_IMAGE_STRUCT **tiles, FILE *fileOutput, int qtdTiles);