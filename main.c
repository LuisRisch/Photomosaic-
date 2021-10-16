#include "photomosaic.h"

int filter(const struct dirent *dir)
{
  if (dir->d_type != DT_REG)
    return 0;
  return 1;
}

int main(int argc, char **argv)
{
  int option;
  char tilesPath[200] = "";
  char tilePath[200] = "";
  int qtdTiles;
  int count = 0;
  int i; 
  struct dirent **imagesPath = NULL;
  P3_IMAGE_STRUCT **tiles = NULL;
  FILE *imageInput = NULL;
  FILE *imageOutput = NULL;
  FILE *tileFile;
  P3_IMAGE_STRUCT *p3Image;

  while ((option = getopt(argc, argv, "p:i:o:h")) != -1)
  {
    switch (option)
    {
    case 'p':
      strcpy(tilesPath, optarg);
      break;
    case 'i':
      imageInput = fopen(optarg, "rb");
      break;
    case 'o':
      imageOutput = fopen(optarg, "wb");
      break;
    case 'h':
      fprintf(stderr, "Usar: %s [ -p diretório_pastilhas_caminho ] [ -i arquivo_imagem_caminho ] [ -o arquivo_saída_caminho ]\n", argv[0]);
      return 0;
    case '?':
      fprintf(stderr, "Opção desconhecida\n\nUsar: %s [ -p diretório_pastilhas_caminho ] [ -i arquivo_imagem_caminho ] [ -o arquivo_saída_caminho ]\n", argv[0]);
      return 0;
    default:
      fprintf(stderr, "Opção desconhecida\n\n'-%c'\nUsar: %s [ -p diretório_pastilhas_caminho ] [ -i arquivo_imagem_caminho ] [ -o arquivo_saída_caminho ]\n", optopt, argv[0]);
      return 0;
    }
  }

  if (!imageInput && !isatty(0))       
    imageInput = stdin;
  else if (!imageInput)
  {
    fprintf(stderr, "A imagem de entrada não foi informada!\n");
    exit(1);
  }

  if (!imageOutput && !isatty(1))
    imageOutput = stdout;
  else if (!imageOutput)
  {
    fprintf(stderr, "A imagem de saida não foi informada!\n");
    fclose(imageInput);
    exit(1);
  }

  p3Image = read_image(imageInput);

  if (!p3Image)
  {
    fprintf(stderr, "Falha durante a leitura da imagem de entrada\n");
    fclose(imageInput);
    fclose(imageOutput);
    exit(1);
  }

  fprintf(stderr, "Input image is PPM %s, %dx%d pixels\n", p3Image->type, p3Image->width, p3Image->height);

  if (!strlen(tilesPath))
    strcpy(tilesPath, "./tiles/");

  qtdTiles = scandir(tilesPath, &imagesPath, filter, alphasort);

  if (!qtdTiles || qtdTiles == -1)
  {
    fprintf(stderr, "O diretório das pastilhas está vazio ou não existe\n");
    fclose(imageInput);
    fclose(imageOutput);
    exit(1);
  }

  tiles = allocate_array_tiles(qtdTiles);

  if (!tiles)
  {
    fprintf(stderr, "Falha durante a alocação de memória\n");
    fclose(imageInput);
    fclose(imageOutput);
    exit(1);
  }

  fprintf(stderr, "Reading tiles from %s\n", tilesPath);

  for (i = 0; i < qtdTiles; i++, count++)
  {
    tilePath[0] = '\0';
    strcat(tilePath, tilesPath);
    strcat(tilePath, imagesPath[i]->d_name);

    tileFile = fopen(tilePath, "rb");

    if (!tileFile)
    {
      fprintf(stderr, "Falha durante a abertura do arquivo %s\n", tilePath);
      fclose(imageInput);
      fclose(imageOutput);
      exit(1);
    }

    tiles[count] = read_image(tileFile);

    if (!tiles[i])
      count--;

    fclose(tileFile);
    free(imagesPath[i]);
  }

  fprintf(stderr, "%d tiles read\n", qtdTiles);
  fprintf(stderr, "Tiles size is %dx%d\n", tiles[0]->width, tiles[0]->height);

  free(imagesPath);

  fprintf(stderr, "Building mosaic image\n");
  build_mosaic(p3Image, tiles, imageOutput, qtdTiles);

  free(p3Image->data);
  free(p3Image);

  for (i = 0; i < qtdTiles; i++)
  {
    free(tiles[i]->data);
    free(tiles[i]);
  }

  free(tiles);

  fclose(imageInput);
  fclose(imageOutput);

  return 0;
}