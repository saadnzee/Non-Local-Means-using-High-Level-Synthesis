#ifndef NLM_H
#define NLM_H

// PARAMETERS FOR IMAGE
static const int ROWS = 256;
static const int COLS = 256;

// PARAMETERS FOR NLM
static const unsigned char PATCH_SIZE = 3;			// 3x3 Patch/Comparison Window
static const unsigned char SEARCH_SIZE = 2;			// 2 implies 5x5 Window.

// PARAMETERS FOR WINDOW
static const unsigned char WIN_SIZE = (2*SEARCH_SIZE) + PATCH_SIZE;
static const unsigned char PAD = WIN_SIZE / 2;

// h = 20 => h2 = 20 * 20 = 400 => inv_h2 = 1/400 = 0.0025
static const float inv_h2 = 0.0025f;

// Top Function Prototype
void nlm_core (const unsigned char in_img[ROWS * COLS], unsigned char out_img[ROWS * COLS]);

#endif
