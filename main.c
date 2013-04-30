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
unsigned int defaultChar = 'A';

int move_to(const FT_Vector* to, void* user)
{
  fprintf(stderr, "MOVE_TO: (%d, %d)\n", to->x, to->y);
  return 0;
}

int line_to(const FT_Vector* to, void* user)
{
  fprintf(stderr, "LINE_TO: (%d, %d)\n", to->x, to->y);
  return 0;
}

int conic_to(const FT_Vector* ctrl, const FT_Vector* to, void* user)
{
  fprintf(stderr, "CUBIC_TO: *(%d, %d) (%d, %d)\n", ctrl->x, ctrl->y, to->x, to->y);
  return 0;
}

int cubic_to(const FT_Vector* ctrl1, const FT_Vector* ctrl2, const FT_Vector* to,
             void* user)
{
  fprintf(stderr, "CUBIC_TO: *(%d, %d)*(%d, %d) (%d, %d)\n", ctrl1->x, ctrl1->y
         , ctrl2->x, ctrl2->y, to->x, to->y);
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
  
  FT_Outline_Funcs funcs;
  funcs.shift = 0;
  funcs.delta = 0;
  funcs.move_to = move_to;
  funcs.line_to = line_to;
  funcs.conic_to = conic_to;
  funcs.cubic_to = cubic_to;
  
  fprintf(stderr, "Generating data..\n");
  /* coordinates are in 26.6 format. */
  FT_Outline_Decompose(&face->glyph->outline, &funcs, NULL);
  
  FT_Done_Face(face);
  FT_Done_FreeType(lib);
}

