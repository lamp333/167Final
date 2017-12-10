#define _CRT_SECURE_NO_DEPRECATE
#pragma once
#include "util.h"

namespace util {
    unsigned char* loadPPM(const GLchar* filename, int& width, int& height)
    {
        const int BUFSIZE = 128;
        FILE* fp;
        unsigned int read;
        unsigned char* rawData;
        char buf[3][BUFSIZE];
        char* retval_fgets;
        size_t retval_sscanf;

        if ((fp = fopen(filename, "rb")) == NULL)
        {
            std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
            width = 0;
            height = 0;
            return NULL;
        }

        // Read magic number:
        retval_fgets = fgets(buf[0], BUFSIZE, fp);

        // Read width and height:
        do
        {
            retval_fgets = fgets(buf[0], BUFSIZE, fp);
        } while (buf[0][0] == '#');
        retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
        width = atoi(buf[1]);
        height = atoi(buf[2]);

        // Read maxval:
        do
        {
            retval_fgets = fgets(buf[0], BUFSIZE, fp);
        } while (buf[0][0] == '#');

        // Read image data:
        rawData = new unsigned char[width * height * 3];
        read = fread(rawData, width * height * 3, 1, fp);
        fclose(fp);
        if (read != 1)
        {
            std::cerr << "error parsing ppm file, incomplete data" << std::endl;
            delete[] rawData;
            width = 0;
            height = 0;
            return NULL;
        }

        return rawData;
    }

    float randomBetween(int min, int max) {
        return rand() % (max - min + 1) + min;
    }
}