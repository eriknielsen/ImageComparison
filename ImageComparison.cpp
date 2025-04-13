#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <filesystem>
#include <fstream>

#include "ImageUtils.h"

/// <summary>
/// Compares 2 vectors of equal size and assumes each pixel is 4 bytes large ordered by RGBA
/// Does not compare alpha
/// </summary>
/// <param name="image"></param>
/// <param name="image2"></param>
/// <returns>Value from 0-1 where 1 is most similar and 0 is not similar</returns>
float compare(std::vector<unsigned char> &image, std::vector<unsigned char>& image2) {
    if (image.size() != image2.size()) {
        std::cout << "Images must be same size" << std::endl;
        return -1;
    }

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

/// <summary>
/// If both images are the same size, fills the delta texture with the delta of all the pixels
/// </summary>
/// <param name="image"></param>
/// <param name="image2"></param>
/// <param name="delta"></param>
/// <returns></returns>
bool tryCreateDeltaImage(std::vector<unsigned char>& image, std::vector<unsigned char>& image2, std::vector<unsigned char>& delta) {
    if (image.size() != image2.size()) {
        std::cout << "Images must be same size" << std::endl;
        return false;
    }
    

    for (size_t i = 0; i < image.size(); i++)
    {
        int deltaColor = std::abs(image[i] - image2[i]);
        if ((i + 1) % 4 == 0) {
            deltaColor = 255;
        }
        delta[i] = deltaColor;
    }

    return true;
}

void copyFile(std::filesystem::path from, std::filesystem::path to) {
    try {
        std::filesystem::copy(from, to);
    }
    catch (const std::exception& e) {
        std::cout << "Exception " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Specify paths to 2 png images. Example: ImageComparsion.exe image1.png image2.png";
        return 1;
    }

    // -- Load the files --
    const char* filename = argv[1];
    std::vector<unsigned char> image1;
    unsigned int width, height;
    if (!tryDecodeImage(filename, image1, width, height)) {
        std::cout << "Decoding image1 failed" << std::endl;
        return 1;
    }

    const char* filename2 = argv[2];
    std::vector<unsigned char> image2;
    unsigned int width2, height2;
    if (!tryDecodeImage(filename2, image2, width2, height2)) {
        std::cout << "Decoding image2 failed" << std::endl;
        return 1;
    }

    // -- Compare the textures --
    float similarity_factor = compare(image1, image2);
    if (similarity_factor < 0) {
        std::cout << "Comparing images failed" << std::endl;
        return 1;
    }
    std::cout << "Images are a factor of " << similarity_factor << " similar" << std::endl;

    // -- Produce a image that contains the differences --
    std::vector<unsigned char> deltaImage(image1.size());
    if (!tryCreateDeltaImage(image1, image2, deltaImage)) {
        std::cout << "Creation of delta image failed" << std::endl;
        return 1;
    }

    std::cout << "Comparison and encoding complete" << std::endl;

    // -- Save the calculated result, delta image and inputs to a folder --
    std::filesystem::path resultsPath = std::filesystem::current_path();
    resultsPath /= "results";
    if(!std::filesystem::exists(resultsPath)) {
        if (!std::filesystem::create_directory(resultsPath)) {
            std::cout << "Creation of results folder failed" << std::endl;
            return 1;
        }
    }

    // -- Make sure the directory is empty --
    for (const auto& entry : std::filesystem::directory_iterator(resultsPath)) {
        std::filesystem::remove_all(entry.path());
    }

    resultsPath /= "delta.png";
    if (!tryEncodeImage(resultsPath.string(), deltaImage, width, height)) {
        std::cout << "Encoding deltaImage failed" << std::endl;
        return 1;
    }

    std::filesystem::path inputsPath = std::filesystem::current_path();
    resultsPath = resultsPath.replace_filename("a.png");
    inputsPath /= filename;
    copyFile(inputsPath, resultsPath);

    resultsPath = resultsPath.replace_filename("b.png");
    inputsPath = inputsPath.replace_filename(filename2);
    copyFile(inputsPath, resultsPath);

    resultsPath = resultsPath.replace_filename("info.txt");
    std::ofstream infoFile;
    infoFile.open(resultsPath.string());
    infoFile << similarity_factor << std::endl;
    infoFile.close();
}

