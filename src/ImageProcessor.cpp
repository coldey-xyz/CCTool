#include "ImageProcessor.h"
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include <wx/filename.h>
#include <wx/datetime.h>
#include <cstdio>
#include <cstdlib>
namespace {
    FILE* OpenFileForBinaryIO(const wxString& path, const char* mode) {
        #ifdef _WIN32
            return _wfopen(path.wc_str(), wxString(mode).wc_str());
        #else
            return fopen(path.utf8_str(), mode);
        #endif
    }
    void WriteCallback(void* context, void*data, int size) {
        fwrite(data, 1, size, static_cast<FILE*>(context));
    }
}
namespace ImageProcessor {
    CompressResult Compress(const wxString& inputPath, const wxString& outputDir, int qualityPercent, const wxString& outputExtension) {
        CompressResult result;
        FILE* inFile = OpenFileForBinaryIO(inputPath, "rb");
        if (!inFile) {
            result.errorMessage = wxString::Format("couldn't read input file: %s", stbi_failure_reason());
            return result;
        }
        int origW, origH, origChannels;
        unsigned char* pixels = stbi_load_from_file(inFile, &origW, &origH, &origChannels, 3);
        fclose(inFile);

        if (!pixels) {
            result.errorMessage = "couldn't read input file";
            return result;
        }
        // scale
        double scale = qualityPercent / 100.0;
        int newW = std::max(1, (int)(origW * scale));
        int newH = std::max(1, (int)(origH * scale));
        unsigned char* resizedPixels = pixels;
        if (qualityPercent < 100) {
            resizedPixels = (unsigned char*)malloc((size_t)newW * newH * 3);
            void* ok = stbir_resize_uint8_srgb(pixels, origW, origH, 0, resizedPixels, newW, newH, 0, STBIR_RGB);
            if (!ok) {
                stbi_image_free(pixels);
                free(resizedPixels);
                result.errorMessage = "couldn't complete a step";
                return result;
            }
        }
        // setting up the output file name
        wxFileName inputFileName(inputPath);
        wxString baseName = inputFileName.GetName();
        wxString dateStr = wxDateTime::Now().Format("%d%m%Y");
        wxString outFileName = wxString::Format("%s-%s-cctool.%s", baseName, dateStr, outputExtension);
        wxFileName outputFileName(outputDir, outFileName);
        wxString outputPath = outputFileName.GetFullPath();
        
        // save stuff
        FILE* outFile = OpenFileForBinaryIO(outputPath, "wb");
        if (!outFile) {
            stbi_image_free(pixels);
            if (resizedPixels != pixels) free(resizedPixels);
            result.errorMessage = "couldn't create output file";
            return result;
        }
        int writeOK = 0;
        if (outputExtension == "png") {
            writeOK = stbi_write_png_to_func(WriteCallback, outFile, newW, newH, 3, resizedPixels, newW * 3);
        }
        else if (outputExtension == "jpeg") {
            int jpgQuality = std::max(1, std::min(100, qualityPercent));
            writeOK = stbi_write_jpg_to_func(WriteCallback, outFile, newW, newH, 3, resizedPixels, jpgQuality);
        }
        else if (outputExtension == "bmp") {
            writeOK = stbi_write_bmp_to_func(WriteCallback, outFile, newW, newH, 3, resizedPixels);
        }
        else if (outputExtension == "tga") {
            writeOK = stbi_write_tga_to_func(WriteCallback, outFile, newW, newH, 3, resizedPixels);
        }
        fclose(outFile);
        stbi_image_free(pixels);
        if (resizedPixels != pixels) free(resizedPixels);

        if (!writeOK) {
            result.errorMessage = "image encoding failed";
            return result;
        }
        result.success = true;
        result.outputPath = outputPath;
        return result;
    }
}