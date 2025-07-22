//
// Created by CorruptionHades on 20/07/2025.
//

#ifndef MESHLOADER_H
#define MESHLOADER_H

#include "../model/vulkr_model.h"
#include <memory>

namespace vulkr {
    namespace MeshLoader {
        std::unique_ptr<VulkrModel> loadObjModel(VulkrDevice& device, const std::string& path);
        std::unique_ptr<VulkrModel> loadGltfModel(VulkrDevice& device, const std::string& path);
    };
}



#endif //MESHLOADER_H
