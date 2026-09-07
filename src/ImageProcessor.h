#pragma once
#include <wx/string.h>
// its not an actual processor calm down
namespace ImageProcessor {
    struct CompressResult {
        bool success = false;
        wxString errorMessage;
        wxString outputPath;
    };

// loads, changes image proportions and saves it
    CompressResult Compress(const wxString& inputPath, const wxString& outputDir, int qualityPercent, const wxString& outputExtension);
}
