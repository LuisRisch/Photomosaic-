/* Biblioteca feita por Luis Felipe Risch para o trabalho 1-fotomosaico de programação 2 */

#include "photomosaic.h"

/* Aloca memória para imagem P3 */
P3_IMAGE_STRUCT *allocate_p3_image()
{
  return (P3_IMAGE_STRUCT *)malloc(sizeof(P3_IMAGE_STRUCT));
}

/* Aloca memória para um vetor de P3_IMAGE_STRUCT */
void *allocate_array_tiles(int qntTiles)
{
  return malloc(sizeof(P3_IMAGE_STRUCT *) * qntTiles);
}

/* Aloca memória para um vetor de P3_PIXEL_STRUCT */
P3_PIXEL_STRUCT *allocate_array_p3_pixels(int width, int height)
{
  return (P3_PIXEL_STRUCT *)malloc(sizeof(P3_PIXEL_STRUCT) * width * height);
}

/* Aloca memória para um vetor de P6_PIXEL_STRUCT */
P6_PIXEL_STRUCT *allocate_array_p6_pixels(int width, int height)
{
  return (P6_PIXEL_STRUCT *)malloc(sizeof(P6_PIXEL_STRUCT) * (width * height + 1));
}

/* Calcula a média das cores */
void calculate_media_pixels(P3_IMAGE_STRUCT *image)
{
  int size;
  int i;

  image->red_avarage = 0;   /* Reseta */
  image->green_avarage = 0; /* Reseta */
  image->blue_avarage = 0;  /* Reseta */

  size = image->height * image->width;

  for (i = 0; i < size; i++)
  {
    image->red_avarage += image->data[i].r;
    image->green_avarage += image->data[i].g;
    image->blue_avarage += image->data[i].b;
  }

  image->red_avarage = image->red_avarage / size;
  image->green_avarage = image->green_avarage / size;
  image->blue_avarage = image->blue_avarage / size;
}

/* Função que lê uma arquivo de entrada, pode ser uma imagem P3 OU P6 */
P3_IMAGE_STRUCT *read_image(FILE *file)
{
  char line[SIZE + 1];
  P3_IMAGE_STRUCT *p3Image = allocate_p3_image();

  if (!p3Image)
  {
    fprintf(stderr, "Erro durante a alocação de memória\n");
    fclose(file);
    return NULL;
  }

  fgets(line, SIZE, file);
  while (
      line[0] == '\0' ||
      !strcmp(line, "\n") ||
      line[0] == '#')
  {
    fgets(line, SIZE, file);
  };

  if (
      sscanf(line, "%s", p3Image->type) != 1 ||
      (strcmp(p3Image->type, "P3") &&
       strcmp(p3Image->type, "P6")))
  {
    fprintf(stderr, "Formato incompatível");
    fclose(file);
    return NULL;
  }

  fgets(line, SIZE, file);
  while (
      line[0] == '\0' ||
      !strcmp(line, "\n") ||
      line[0] == '#')
  {
    fgets(line, SIZE, file);
  };

  if (sscanf(line, "%d %d", &p3Image->width, &p3Image->height) != 2)
  {
    fprintf(stderr, "Erro durante a leitura das dimensões da imagem\n");
    fclose(file);
    return NULL;
  }

  fgets(line, SIZE, file);
  while (
      line[0] == '\0' ||
      !strcmp(line, "\n") ||
      line[0] == '#')
  {
    fgets(line, SIZE, file);
  };

  if (sscanf(line, "%d", &p3Image->max) != 1)
  {
    fprintf(stderr, "Falha durante a leitura de cor máximo\n");
    fclose(file);
    return NULL;
  }

  if (p3Image->max != RGB_COMPONENT_COLOR)
  {
    fprintf(stderr, "Valor máximo é diferente de 255\n");
    fclose(file);
    return NULL;
  }

  p3Image->data = allocate_array_p3_pixels(p3Image->width, p3Image->height);

  if (!p3Image->data)
  {
    fprintf(stderr, "Falha durante a alocação de memória\n");
    fclose(file);
    return NULL;
  }

  int size = p3Image->width * p3Image->height;
  int i;

  if (!strcmp(p3Image->type, "P3"))
    for (i = 0; i < size; i++)
    {
      fscanf(file, "%d ", &p3Image->data[i].r);
      fscanf(file, "%d ", &p3Image->data[i].g);
      fscanf(file, "%d ", &p3Image->data[i].b);
    }
  else
  {
    P6_PIXEL_STRUCT *temp = allocate_array_p6_pixels(p3Image->width, p3Image->height);

    if (!temp)
    {
      fprintf(stderr, "Falha durante a alocação de memória\n");
      fclose(file);
      return NULL;
    }

    if (fread(temp, sizeof(P6_PIXEL_STRUCT), size, file) != size)
    {
      fprintf(stderr, "Falha durante a leitura dos pixels\n");
      fclose(file);
      return NULL;
    }

    /* Convertendo para o formato P3, para facilitar as contas e o processo de montagem do mosaico */
    for (i = 0; i < size; i++)
    {
      p3Image->data[i].r = (int)temp[i].r;
      p3Image->data[i].g = (int)temp[i].g;
      p3Image->data[i].b = (int)temp[i].b;
    }
    free(temp);
  }

  calculate_media_pixels(p3Image);

  return p3Image;
}

/* Escreve no arquivo de saida o mosaico, no formato(P3 OU P6) original da imagem */
void write_ppm(FILE *fileOutput, P3_IMAGE_STRUCT *image)
{
  int size = image->width * image->height;

  fprintf(fileOutput, "%s\n", image->type);
  fprintf(fileOutput, "\n");
  fprintf(fileOutput, "%d %d\n", image->width, image->height);
  fprintf(fileOutput, "%d\n", RGB_COMPONENT_COLOR);

  if (!strcmp(image->type, "P3"))
    for (int i = 0; i < size; i++)
    {
      fprintf(fileOutput, "%d ", image->data[i].r);
      fprintf(fileOutput, "%d ", image->data[i].g);
      fprintf(fileOutput, "%d ", image->data[i].b);
    }
  else
  {
    P6_PIXEL_STRUCT *auxArrayPixelP6 = allocate_array_p6_pixels(image->width, image->height);

    if (!auxArrayPixelP6)
    {
      fprintf(stderr, "Falha durante a alocação de memória\n");
      exit(1);
    }

    int i;

    /* Convertendo para o formato P6 */
    for (i = 0; i < size; i++)
    {
      auxArrayPixelP6[i].r = (unsigned char)image->data[i].r;
      auxArrayPixelP6[i].g = (unsigned char)image->data[i].g;
      auxArrayPixelP6[i].b = (unsigned char)image->data[i].b;
    }

    if (fwrite(auxArrayPixelP6, sizeof(P6_PIXEL_STRUCT), size, fileOutput) != size)
    {
      fprintf(stderr, "Erro durante a escrita dos pixels\n");
      exit(1);
    }

    free(auxArrayPixelP6);
  }
}

/* Copia um bloco da imagem original na imagem auxiliar */
void copy_part_image(
    P3_IMAGE_STRUCT *image,
    P3_IMAGE_STRUCT *auxImage,
    int tileWidth,
    int tileHeight,
    int i,
    int j)
{
  int iniLine = tileHeight * i;
  int iniCol = tileWidth * j;
  int limitLine = tileHeight + iniLine;
  int limitCol = tileWidth + iniCol;
  int lin, col, count = 0;

  for (lin = iniLine; lin < limitLine && lin < image->height; lin++)
    for (col = iniCol; col < limitCol && col < image->width; col++)
    {
      auxImage->data[count].r = image->data[image->width * lin + col].r;
      auxImage->data[count].g = image->data[image->width * lin + col].g;
      auxImage->data[count].b = image->data[image->width * lin + col].b;
      count++;
    }

  auxImage->height = tileHeight;
  auxImage->width = tileWidth;
}

/* Calcula a distância entre as cores pelo método red mean */
double calculate_distance(P3_IMAGE_STRUCT *image1, P3_IMAGE_STRUCT *image2)
{
  double mRed, red, green, blue, deltaRed, deltaGreen, deltaBlue;

  deltaRed = image1->red_avarage - image2->red_avarage;
  deltaGreen = image1->green_avarage - image2->green_avarage;
  deltaBlue = image1->blue_avarage - image2->blue_avarage;
  mRed = (image1->red_avarage + image2->red_avarage) / 2.0;

  red = (2.0 + mRed / 256.0) * pow(deltaRed, 2.0);

  green = 4 * pow(deltaGreen, 2);

  blue = (2 + (255 - mRed) / 256.0) * pow(deltaBlue, 2.0);

  return sqrt(red + green + blue);
}

/* Acha uma pastilha, no vetor de pastilhas, que possui a menor distância 
 * com relação a um bloco da imagem original */
int find_lower_distance(P3_IMAGE_STRUCT *image, P3_IMAGE_STRUCT **tiles, int qtdTiles)
{
  double currDistance;
  double lowerDistance = calculate_distance(tiles[0], image);
  int lowerIndex = 0;

  for (int i = 1; i < qtdTiles; i++)
  {
    currDistance = calculate_distance(tiles[i], image);
    if (currDistance < lowerDistance)
    {
      lowerIndex = i;
      lowerDistance = currDistance;
    }
  }

  return lowerIndex;
}

/* Substitui um bloco da imagem original por uma pastilha */
void replace_tile(
    P3_IMAGE_STRUCT *image,
    P3_IMAGE_STRUCT *tile,
    int tileWidth,
    int tileHeight,
    int i,
    int j)
{
  int iniLine = tileHeight * i;
  int iniCol = tileWidth * j;
  int limitLine = tileHeight + tileHeight * i;
  int limitCol = tileWidth + tileWidth * j;
  int count = 0;

  for (int lin = iniLine; lin < limitLine && lin < image->height; lin++)
    for (int col = iniCol; col < limitCol && col < image->width; col++)
    {
      image->data[image->width * lin + col].r = tile->data[count].r;
      image->data[image->width * lin + col].g = tile->data[count].g;
      image->data[image->width * lin + col].b = tile->data[count].b;
      count++;
    }
}

/* Constroi o mosaico */
void build_mosaic(P3_IMAGE_STRUCT *image, P3_IMAGE_STRUCT **tiles, FILE *fileOutput, int qtdTiles)
{
  int tileWidth = tiles[0]->width;
  int tileHeight = tiles[0]->height;

  P3_IMAGE_STRUCT *auxImage = allocate_p3_image();

  if (!auxImage)
  {
    fprintf(stderr, "Erro durante a alocação de memória\n");
    exit(1);
  }

  auxImage->data = allocate_array_p3_pixels(tileWidth, tileHeight);

  if (!auxImage->data)
  {
    fprintf(stderr, "Erro durante a alocação de memória\n");
    exit(1);
  }

  for (int i = 0; i < ceil(image->height / tileHeight); i++) /* Divide a imagem em vários blocos iguais de altura das pastilhas */
    for (int j = 0; j < ceil(image->width / tileWidth); j++) /* Divide a imagem em vários blocos iguais de largura das pastilhas */
    {
      copy_part_image(image, auxImage, tileWidth, tileHeight, i, j); /* Copia um bloco da imagem orginal na imagem auxiliar */
      calculate_media_pixels(auxImage);
      replace_tile(image, tiles[find_lower_distance(auxImage, tiles, qtdTiles)], tileWidth, tileHeight, i, j); /* Substitui um bloco da imagem original pela pastilha que possui a menor distância */
    }

  free(auxImage->data);
  free(auxImage);

  fprintf(stderr, "Writing output file\n");
  write_ppm(fileOutput, image);
}