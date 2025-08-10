#include "nlm.h"
#include <hls_math.h>

// Gaussian Kernel Matrix
static const float gaussian_kernel[PATCH_SIZE][PATCH_SIZE] = {
    {0.101868056f, 0.115431629f, 0.101868056f},
    {0.115431629f, 0.130801171f, 0.115431629f},
    {0.101868056f, 0.115431629f, 0.101868056f}
};

void nlm_core(
    const unsigned char in_img[ROWS * COLS], unsigned char out_img[ROWS * COLS]
) {
    #pragma HLS INTERFACE ap_memory     port=in_img
    #pragma HLS INTERFACE ap_memory     port=out_img
    #pragma HLS INTERFACE ap_ctrl_hs    port=return

    // Line buffer
    unsigned char lb[WIN_SIZE][COLS];
    #pragma HLS ARRAY_PARTITION variable=lb complete dim=1

    // Temporary patches
    unsigned char ref_patch[PATCH_SIZE][PATCH_SIZE];
    unsigned char cand_patch[PATCH_SIZE][PATCH_SIZE];
    #pragma HLS ARRAY_PARTITION variable=ref_patch complete dim=0
    #pragma HLS ARRAY_PARTITION variable=cand_patch complete dim=0

    // Initialize line buffer
    for (int r = 0; r < WIN_SIZE; r++) {
        for (int c = 0; c < COLS; c++) {
            #pragma HLS PIPELINE II=1
            lb[r][c] = in_img[r * COLS + c];
        }
    }

    int current_row = WIN_SIZE;

    for (int row = PAD; row < ROWS - PAD; row++) {
        for (int col = PAD; col < COLS - PAD; col++) {
		#pragma HLS PIPELINE II=8

            // Extract 7x7 window
            unsigned char win[WIN_SIZE][WIN_SIZE];
            #pragma HLS ARRAY_PARTITION variable=win complete dim=0

            for (int i = 0; i < WIN_SIZE; i++) {
                for (int j = 0; j < WIN_SIZE; j++) {
                    win[i][j] = lb[i][col - PAD + j];
                }
            }

            // Extract 3x3 reference patch
            for (int i = 0; i < PATCH_SIZE; i++) {
                for (int j = 0; j < PATCH_SIZE; j++) {
                    ref_patch[i][j] = win[i + SEARCH_SIZE][j + SEARCH_SIZE];
                }
            }

            float weight_sum = 0;
            float pixel_sum = 0;

            for (int i = 0; i <= WIN_SIZE - PATCH_SIZE; i++) {
                for (int j = 0; j <= WIN_SIZE - PATCH_SIZE; j++) {
                    // Extract candidate patch
                    for (int pi = 0; pi < PATCH_SIZE; pi++) {
                        for (int pj = 0; pj < PATCH_SIZE; pj++) {
                            cand_patch[pi][pj] = win[i + pi][j + pj];
                        }
                    }

                    // L2 distance
                    float dist = 0;
                    for (int pi = 0; pi < PATCH_SIZE; pi++) {
                        for (int pj = 0; pj < PATCH_SIZE; pj++) {
                            float diff = ref_patch[pi][pj] - cand_patch[pi][pj];
                            dist += gaussian_kernel[pi][pj] * diff * diff;
                        }
                    }

                    // Weight and accumulate
                    float weight = hls::expf(-dist * inv_h2);
                    weight_sum += weight;
                    pixel_sum += weight * win[i + 1][j + 1];
                }
            }

            pixel_sum /= weight_sum;
            unsigned char final_pixel = (unsigned char)(
                pixel_sum > 255 ? 255 : (pixel_sum < 0 ? 0 : pixel_sum)
            );

            out_img[row * COLS + col] = final_pixel;
        }

        // Slide buffer down
        if (current_row < ROWS) {
            for (int r = 0; r < WIN_SIZE - 1; r++) {
                for (int c = 0; c < COLS; c++) {
				#pragma HLS PIPELINE II=1
                    lb[r][c] = lb[r + 1][c];
                }
            }
            for (int c = 0; c < COLS; c++) {
                #pragma HLS PIPELINE II=1
                lb[WIN_SIZE - 1][c] = in_img[current_row * COLS + c];
            }
            current_row++;
        }
    }

}
