#include "ImageUtils.h"

/// <summary>
/// Uses lodepng to load and decode the png
/// </summary>
/// <param name="filename"></param>
/// <param name="image"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <returns></returns>
bool tryDecodeImage(const char* filename, std::vector<unsigned char>& image, unsigned int& width, unsigned int& height) {
    unsigned error;

    error = lodepng::decode(image, width, height, filename);
    if (error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
        return false;
    }

    return true;
}

/// <summary>
/// Uses lodepng to encode and write the texture to disk
/// </summary>
/// <param name="filename"></param>
/// <param name="image"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <returns></returns>
bool tryEncodeImage(const std::string filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
    //Encode the image
    unsigned error = lodepng::encode(filename, image, width, height);

    //if there's an error, display it
    if (error) {
        std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    return true;
}