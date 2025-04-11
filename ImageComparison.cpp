// ImageComparison.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "lodepng.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <iomanip>

/// <summary>
/// Compares 2 vectors and assumes each pixel is 4 bytes large ordered by RGBA
/// Does not compare alpha
/// </summary>
/// <param name="image"></param>
/// <param name="image2"></param>
/// <returns>Value from 0-1 where 1 is most similar and 0 is not similar</returns>
float compare(std::vector<unsigned char> &image, std::vector<unsigned char>& image2) {

    const float color_count = 255;
    float similarity_sum = 0;
    for (size_t i = 0; i < image.size(); i += 4)
    {
        float similarity = 0;
        similarity += 1 - std::abs(image[i] - image2[i]) / color_count;
        similarity += 1 - std::abs(image[i + 1] - image2[i + 1]) / color_count;
        similarity += 1 - std::abs(image[i + 2] - image2[i + 2]) / color_count;
        similarity /= 3;

        similarity_sum += similarity;
    }

    similarity_sum = std::roundf(similarity_sum / (image.size() / 4) * 1000) / 1000;

    return similarity_sum;
}

bool tryDecodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned int& width, unsigned int& height) {
    unsigned error;

    error = lodepng::decode(image, width, height, filename);
    if (error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    const char* filename = argc > 1 ? argv[1] : "image1.png";
    std::vector<unsigned char> image1;
    unsigned int width, height;
    if (!tryDecodeOneStep(filename, image1, width, height)) {
        std::cout << "Decoding image1 failed";
        return 1;
    }
    const char* filename2 = argc > 1 ? argv[1] : "image2.png";
    std::vector<unsigned char> image2;
    unsigned int width2, height2;
    if (!tryDecodeOneStep(filename2, image2, width2, height2)) {
        std::cout << "Decoding image2 failed";
        return 1;
    }

    
    float similarity_factor = compare(image1, image2);

    std::cout << "Images are a factor of " << similarity_factor << " similar";
}

