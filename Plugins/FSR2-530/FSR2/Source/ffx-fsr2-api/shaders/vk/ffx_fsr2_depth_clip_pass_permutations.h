#include "ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777.h"
#include "ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2.h"
#include "ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a.h"
#include "ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62.h"
#include "ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b.h"
#include "ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc.h"
#include "ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc.h"
#include "ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765.h"
#include "ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41.h"
#include "ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122.h"
#include "ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad.h"
#include "ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29.h"
#include "ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0.h"
#include "ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93.h"
#include "ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2.h"
#include "ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f.h"

typedef union ffx_fsr2_depth_clip_pass_PermutationKey {
    struct {
        uint32_t FFX_FSR2_OPTION_REPROJECT_USE_LANCZOS_TYPE : 1;
        uint32_t FFX_FSR2_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR2_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR2_OPTION_APPLY_SHARPENING : 1;
        uint32_t FFX_HALF : 1;
    };
    uint32_t index;
} ffx_fsr2_depth_clip_pass_PermutationKey;

typedef struct ffx_fsr2_depth_clip_pass_PermutationInfo {
    const uint32_t       blobSize;
    const unsigned char* blobData;

    const uint32_t  numSamplerResources;
    const char**    samplerResourceNames;
    const uint32_t* samplerResourceBindings;
    const uint32_t* samplerResourceCounts;
    const uint32_t* samplerResourceSets;

    const uint32_t  numCombinedSamplerResources;
    const char**    combinedSamplerResourceNames;
    const uint32_t* combinedSamplerResourceBindings;
    const uint32_t* combinedSamplerResourceCounts;
    const uint32_t* combinedSamplerResourceSets;

    const uint32_t  numSampledImageResources;
    const char**    sampledImageResourceNames;
    const uint32_t* sampledImageResourceBindings;
    const uint32_t* sampledImageResourceCounts;
    const uint32_t* sampledImageResourceSets;

    const uint32_t  numStorageImageResources;
    const char**    storageImageResourceNames;
    const uint32_t* storageImageResourceBindings;
    const uint32_t* storageImageResourceCounts;
    const uint32_t* storageImageResourceSets;

    const uint32_t  numUniformTexelBufferResources;
    const char**    uniformTexelBufferResourceNames;
    const uint32_t* uniformTexelBufferResourceBindings;
    const uint32_t* uniformTexelBufferResourceCounts;
    const uint32_t* uniformTexelBufferResourceSets;

    const uint32_t  numStorageTexelBufferResources;
    const char**    storageTexelBufferResourceNames;
    const uint32_t* storageTexelBufferResourceBindings;
    const uint32_t* storageTexelBufferResourceCounts;
    const uint32_t* storageTexelBufferResourceSets;

    const uint32_t  numUniformBufferResources;
    const char**    uniformBufferResourceNames;
    const uint32_t* uniformBufferResourceBindings;
    const uint32_t* uniformBufferResourceCounts;
    const uint32_t* uniformBufferResourceSets;

    const uint32_t  numStorageBufferResources;
    const char**    storageBufferResourceNames;
    const uint32_t* storageBufferResourceBindings;
    const uint32_t* storageBufferResourceCounts;
    const uint32_t* storageBufferResourceSets;

    const uint32_t  numInputAttachmentResources;
    const char**    inputAttachmentResourceNames;
    const uint32_t* inputAttachmentResourceBindings;
    const uint32_t* inputAttachmentResourceCounts;
    const uint32_t* inputAttachmentResourceSets;

    const uint32_t  numRTAccelerationStructureResources;
    const char**    rtAccelerationStructureResourceNames;
    const uint32_t* rtAccelerationStructureResourceBindings;
    const uint32_t* rtAccelerationStructureResourceCounts;
    const uint32_t* rtAccelerationStructureResourceSets;

} ffx_fsr2_depth_clip_pass_PermutationInfo;

static const uint32_t g_ffx_fsr2_depth_clip_pass_IndirectionTable[] = {
    14,
    14,
    14,
    14,
    6,
    6,
    6,
    6,
    10,
    10,
    10,
    10,
    2,
    2,
    2,
    2,
    12,
    12,
    12,
    12,
    4,
    4,
    4,
    4,
    9,
    9,
    9,
    9,
    0,
    0,
    0,
    0,
    14,
    14,
    14,
    14,
    6,
    6,
    6,
    6,
    10,
    10,
    10,
    10,
    2,
    2,
    2,
    2,
    12,
    12,
    12,
    12,
    4,
    4,
    4,
    4,
    9,
    9,
    9,
    9,
    0,
    0,
    0,
    0,
    15,
    15,
    15,
    15,
    7,
    7,
    7,
    7,
    11,
    11,
    11,
    11,
    3,
    3,
    3,
    3,
    13,
    13,
    13,
    13,
    5,
    5,
    5,
    5,
    8,
    8,
    8,
    8,
    1,
    1,
    1,
    1,
    15,
    15,
    15,
    15,
    7,
    7,
    7,
    7,
    11,
    11,
    11,
    11,
    3,
    3,
    3,
    3,
    13,
    13,
    13,
    13,
    5,
    5,
    5,
    5,
    8,
    8,
    8,
    8,
    1,
    1,
    1,
    1,
};

static const ffx_fsr2_depth_clip_pass_PermutationInfo g_ffx_fsr2_depth_clip_pass_PermutationInfo[] = {
    { g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_size, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_data, 1, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_1a9472f0bdb51cef907b10a62046b777_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_size, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_data, 1, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_ba62f190ca1d973cbd99e1c9f5944ba2_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_size, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_data, 1, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_31ee73b56178d45a9daf97a777cbcb8a_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_size, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_data, 1, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_c7e556f7f955aeb1871fead419040b62_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_size, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_data, 1, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_81c77abfcbf8dadb1c5335305dd9b71b_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_size, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_data, 1, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_41b3c034da52e3fac673ce293e37adcc_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_size, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_data, 1, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_1fad923493dad552f11d5d66e657ccfc_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_size, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_data, 1, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_87fea2a655c4d1efe7982363a5b78765_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_size, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_data, 1, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_302c2a2bd444a1dd5642d05cb42afe41_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_size, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_data, 1, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_2fe191a94e03cccebdfcd210d3577122_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_size, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_data, 1, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_4c8d2599f7389af35008fce66fa9c9ad_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_size, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_data, 1, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_1dbf715f842e15757d8a84df7de5dc29_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_size, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_data, 1, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_fa08c7c17df91a42d8b92573274993f0_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_size, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_data, 1, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_5f9a4abbe962a2209af0f2848db62f93_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_size, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_data, 1, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_0e850597160b3950387ae475f87487a2_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_size, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_data, 1, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_SamplerResourceSets, 0, 0, 0, 0, 0, 9, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_SampledImageResourceNames, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_SampledImageResourceBindings, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_SampledImageResourceCounts, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_SampledImageResourceSets, 2, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_StorageImageResourceNames, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_StorageImageResourceBindings, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_StorageImageResourceCounts, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_UniformBufferResourceNames, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_UniformBufferResourceBindings, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_UniformBufferResourceCounts, g_ffx_fsr2_depth_clip_pass_42c541a51c17b6859e56c3a41519276f_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

