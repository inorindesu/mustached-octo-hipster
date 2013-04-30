/* 
 * Copyright (C) 2013  Inori Sakura <inorindesu@gmail.com>
 * 
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <stdlib.h>
#include <stdio.h>

char defaultFont[] = "/usr/share/fonts/google-droid/DroidSans.ttf";
unsigned int defaultChar = 'm';

int test_move_to(const FT_Vector* to, void* user)
{
  FT_Vector* vec = (FT_Vector*)user;
  fprintf(stderr, "MOVE_TO: (%d, %d)\n", to->x, to->y);
  if (to->x > vec->x)
    vec->x = to->x;
  if (to->y > vec->y)
    vec->y = to->y;
  fprintf(stderr, "%d %d\n", vec->x, vec->y);
  return 0;
}

int test_line_to(const FT_Vector* to, void* user)
{
  FT_Vector* vec = (FT_Vector*)user;
  if (to->x > vec->x)
    vec->x = to->x;
  if (to->y > vec->y)
    vec->y = to->y;
  fprintf(stderr, "%d %d\n", vec->x, vec->y);
  return 0;
}

int test_conic_to(const FT_Vector* ctrl, const FT_Vector* to, void* user)
{
  FT_Vector* vec = (FT_Vector*)user;
  if (ctrl->x > vec->x)
    vec->x = ctrl->x;
  if (ctrl->y > vec->y)
    vec->y = ctrl->y;
  if (to->x > vec->x)
    vec->x = to->x;
  if (to->y > vec->y)
    vec->y = to->y;
  fprintf(stderr, "%d %d\n", vec->x, vec->y);
  return 0;
}

int test_cubic_to(const FT_Vector* ctrl1, const FT_Vector* ctrl2, const FT_Vector* to,
             void* user)
{
  FT_Vector* vec = (FT_Vector*)user;
  if (ctrl1->x > vec->x)
    vec->x = ctrl1->x;
  if (ctrl1->y > vec->y)
    vec->y = ctrl1->y;
  if (ctrl2->x > vec->x)
    vec->x = ctrl2->x;
  if (ctrl2->y > vec->y)
    vec->y = ctrl2->y;
  if (to->x > vec->x)
    vec->x = to->x;
  if (to->y > vec->y)
    vec->y = to->y;
  fprintf(stderr, "%d %d\n", vec->x, vec->y);
  return 0;
}

int move_to(const FT_Vector* to, void* user)
{
  FT_Vector* vec = (FT_Vector*)user;
  fprintf(stderr, "MOVE_TO: (%d, %d)\n", to->x, vec->y - to->y);
  printf(" M %d %d ", to->x, vec->y - to->y);
  return 0;
}

int line_to(const FT_Vector* to, void* user)
{
  FT_Vector* vec = (FT_Vector*)user;
  fprintf(stderr, "LINE_TO: (%d, %d)\n", to->x, vec->y - to->y);
  printf(" L %d %d ", to->x, vec->y - to->y);
  return 0;
}

int conic_to(const FT_Vector* ctrl, const FT_Vector* to, void* user)
{
  FT_Vector* vec = (FT_Vector*)user;
  fprintf(stderr, "CONIC_TO: *(%d, %d) (%d, %d)\n", ctrl->x, vec->y - ctrl->y, to->x, vec->y - to->y);
  printf(" Q %d %d %d %d ", ctrl->x, vec->y - ctrl->y, to->x, vec->y - to->y);
  return 0;
}

int cubic_to(const FT_Vector* ctrl1, const FT_Vector* ctrl2, const FT_Vector* to,
             void* user)
{
  FT_Vector* vec = (FT_Vector*)user;
  fprintf(stderr, "CUBIC_TO: *(%d, %d)*(%d, %d) (%d, %d)\n", ctrl1->x, vec->y - ctrl1->y
         , ctrl2->x, vec->y - ctrl2->y, to->x, vec->y - to->y);
  printf(" C %d %d %d %d ", ctrl1->x, vec->y - ctrl1->y, ctrl2->x, vec->y - ctrl2->y, to->x
         , vec->y - to->y);
  return 0;
}

int main(int argc, char** argv)
{
  char* fontPath = NULL;
  unsigned int charToPlot = 0;

  /* argument processing*/
  if (argc == 1)
    {
      fontPath = defaultFont;
      charToPlot = defaultChar;
    }
  else if(argc == 2)
    {
      fontPath = argv[1];
      charToPlot = defaultChar;
    }
  else
    {
      fontPath = argv[1];
      char leader = argv[2][0];

      if ((leader & 0xF0) == 0xF0)
        {
          charToPlot += (argv[2][0] & 0x07) << 18;
          //fprintf(stderr, "%.8x\n", charToPlot);
          charToPlot += (argv[2][1] & 0x3F) << 12;
          //fprintf(stderr, "%.8x\n", charToPlot);
          charToPlot += (argv[2][2] & 0x3F) << 6;
          //fprintf(stderr, "%.8x\n", charToPlot);
          charToPlot += (argv[2][3] & 0x3F);
          //fprintf(stderr, "%.8x\n", charToPlot);
        }
      else if ((leader & 0xE0) == 0xE0)
        {
          charToPlot += (argv[2][0] & 0x0F) << 12;
          //fprintf(stderr, "%.8x\n", charToPlot);
          charToPlot += (argv[2][1] & 0x3F) << 6;
          //fprintf(stderr, "%.8x\n", charToPlot);
          charToPlot += (argv[2][2] & 0x3F);
        }
      else if ((leader & 0xC0) == 0xC0)
        {
          charToPlot += (argv[2][0] & 0x1F) << 6;
          //fprintf(stderr, "%.8x\n", charToPlot);
          charToPlot += (argv[2][1] & 0x3F);
        }
      else
        {
          charToPlot = argv[2][0];
        }
      fprintf(stderr, "%.8x\n", charToPlot);
    }
  
  /* font check */
  FILE* f = fopen(fontPath, "r");
  if (f == NULL)
    {
      f = fopen(defaultFont, "r");
      if (f == NULL)
        {
          fprintf(stderr, "ERROR: cannot load default font.\n");
          exit(1);
        }
      fontPath = defaultFont;
    }
  fclose(f);
  
  fprintf(stderr, "Font selected:\n%s\n", fontPath);
  fprintf(stderr, "Char to render (in utf32):0x%.8x (%u)\n", charToPlot, charToPlot);
  
  FT_Library lib;
  int error;
  error = FT_Init_FreeType(&lib);
  if (error)
    {
      fprintf(stderr, "ERROR on init freetype2.\n");
      exit(1);
    }
  
  FT_Face face;
  error = FT_New_Face(lib, fontPath, 0, &face);
  if (error)
    {
      fprintf(stderr, "ERROR when loading font file.\n");
      exit(1);
    }
  
  error = FT_Set_Char_Size(face, 0, 16*64, 300, 300);
  if (error)
    {
      fprintf(stderr, "WARNING: cannot set character size\n");
    }
  
  unsigned int glyphIndex = FT_Get_Char_Index(face, charToPlot);
  if (glyphIndex == 0)
    {
      fprintf(stderr, "WARNING: requested character is not in the font\n");
    }
  
  error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
  if (error)
    {
      fprintf(stderr, "ERROR when loading glyph\n");
      exit(1);
    }
  
  if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
    {
      fprintf(stderr, "ERROR: I process only outline fonts.");
      exit(1);
    }

  FT_Vector vec;
  vec.x = 0;
  vec.y = 0;

  FT_Outline_Funcs testFuncs;
  testFuncs.shift = 0;
  testFuncs.delta = 0;
  testFuncs.move_to = test_move_to;
  testFuncs.line_to = test_line_to;
  testFuncs.conic_to = test_conic_to;
  testFuncs.cubic_to = test_cubic_to;
  
  FT_Outline_Funcs drawFuncs;
  drawFuncs.shift = 0;
  drawFuncs.delta = 0;
  drawFuncs.move_to = move_to;
  drawFuncs.line_to = line_to;
  drawFuncs.conic_to = conic_to;
  drawFuncs.cubic_to = cubic_to;

  fprintf(stderr, "Testing height..\n");
  FT_Outline_Decompose(&face->glyph->outline, &testFuncs, &vec);
  fprintf(stderr, "Highest x is: %d\nHighest y is: %d\n", vec.x, vec.y);

  fprintf(stderr, "Generating data..\n");
  /* coordinates are in 26.6 format. */
  printf("<?xml version=\"1.0\" standalone=\"no\"?>\n");
  printf("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%f\" height=\"%f\">\n"
         , vec.x / 64.0, vec.y / 64.0);
  printf("<path transform=\"scale(%f)\" d =\"", 1 / 64.0);
  FT_Outline_Decompose(&face->glyph->outline, &drawFuncs, &vec);
  printf("\" id=\"char_%.8x\">\n", charToPlot);
  printf("</path>\n</svg>\n");
  
  FT_Done_Face(face);
  FT_Done_FreeType(lib);
}

