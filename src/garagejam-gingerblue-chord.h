/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/
 
 */

#ifndef _GINGERBLUE_CHORD_H_
#define _GINGERBLUE_CHORD_H_ 1

typedef struct _GingerblueChord GingerblueChord;

struct _GingerblueChord {
	char *root;
	char *file;
	/* struct Display *gui; */
	char e1;
	char b2;
	char g3;
	char d4;
	char a5;
	char e6;
};

struct _GingerblueChord gbc[] = {
  {"C",    "Gingerblue_Guitar_C.wav",  /* &console, */ '0', '1', '0', '2', '3', '0'},
  {"C#",   "Gingerblue_Guitar_C#.wav", /* &console, */ '1', '2', '1', '3', '4', '0'},
  {"Db",   "Gingerblue_Guitar_Db.wav", /* &console, */ '1', '2', '1', '3', '4', '0'},
  {"D",    "Gingerblue_Guitar_D.wav",  /* &console, */ '1', '2', '1', '0', '0', '0'},
  {"D#",   "Gingerblue_Guitar_D#.wav", /* &console, */ '3', '4', '2', '1', '0', '0'},
  {"Eb",   "Gingerblue_Guitar_Eb.wav", /* &console, */ '3', '4', '2', '1', '0', '0'},
  {"E",    "Gingerblue_Guitar_E.wav",  /* &console, */ '0', '0', '1', '2', '2', '0'},
  {"F",    "Gingerblue_Guitar_F.wav",  /* &console, */ '1', '1', '2', '3', '3', '1'},
  {"F#",   "Gingerblue_Guitar_F#.wav", /* &console, */ '2', '2', '3', '4', '4', '1'},
  {"Gb",   "Gingerblue_Guitar_Gb.wav", /* &console, */ '2', '2', '3', '4', '4', '1'},
  {"G",    "Gingerblue_Guitar_G.wav",  /* &console, */ '3', '0', '0', '0', '2', '3'},
  {"G#",   "Gingerblue_Guitar_G#.wav", /* &console, */ '0', '1', '1', '1', '3', '4'},
  {"Ab",   "Gingerblue_Guitar_Ab.wav", /* &console, */ '0', '1', '1', '1', '3', '4'},
  {"A",    "Gingerblue_Guitar_A.wav",  /* &console, */ '0', '2', '2', '2', '0', '0'},
  {"A#",   "Gingerblue_Guitar_A#.wav", /* &console, */ '1', '3', '3', '3', '1', '0'},
  {"Bb",   "Gingerblue_Guitar_Bb.wav", /* &console, */ '1', '3', '3', '3', '1', '-'},
  {"B",    "Gingerblue_Guitar_B.wav",  /* &console, */ '2', '4', '4', '4', '2', '0'},
  {"Bm",   "Gingerblue_Guitar_Bm.wav", /* &console, */ '2', '3', '4', '4', '2', '-'},
  {NULL,NULL}
};

#endif /* _GINGERBLUE_CHORD_H_ */
