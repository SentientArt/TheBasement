#include "ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10.h"
#include "ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082.h"
#include "ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d.h"
#include "ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b.h"

typedef union ffx_fsr2_tcr_autogen_pass_PermutationKey {
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
} ffx_fsr2_tcr_autogen_pass_PermutationKey;

typedef struct ffx_fsr2_tcr_autogen_pass_PermutationInfo {
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

} ffx_fsr2_tcr_autogen_pass_PermutationInfo;

static const uint32_t g_ffx_fsr2_tcr_autogen_pass_IndirectionTable[] = {
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
};

static const ffx_fsr2_tcr_autogen_pass_PermutationInfo g_ffx_fsr2_tcr_autogen_pass_PermutationInfo[] = {
    { g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_size, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_SampledImageResourceNames, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_SampledImageResourceBindings, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_SampledImageResourceCounts, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_SampledImageResourceSets, 4, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_StorageImageResourceNames, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_StorageImageResourceBindings, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_StorageImageResourceCounts, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_UniformBufferResourceNames, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_UniformBufferResourceBindings, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_UniformBufferResourceCounts, g_ffx_fsr2_tcr_autogen_pass_7979158f6e58c0607bc73141c47b3c10_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_size, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_SampledImageResourceNames, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_SampledImageResourceBindings, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_SampledImageResourceCounts, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_SampledImageResourceSets, 4, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_StorageImageResourceNames, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_StorageImageResourceBindings, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_StorageImageResourceCounts, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_UniformBufferResourceNames, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_UniformBufferResourceBindings, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_UniformBufferResourceCounts, g_ffx_fsr2_tcr_autogen_pass_e30e1632f5b745efc0ada1c6976be082_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_size, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_SampledImageResourceNames, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_SampledImageResourceBindings, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_SampledImageResourceCounts, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_SampledImageResourceSets, 4, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_StorageImageResourceNames, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_StorageImageResourceBindings, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_StorageImageResourceCounts, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_UniformBufferResourceNames, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_UniformBufferResourceBindings, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_UniformBufferResourceCounts, g_ffx_fsr2_tcr_autogen_pass_8eb629361f4e8321cf3f0457a6835d8d_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_size, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_SampledImageResourceNames, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_SampledImageResourceBindings, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_SampledImageResourceCounts, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_SampledImageResourceSets, 4, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_StorageImageResourceNames, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_StorageImageResourceBindings, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_StorageImageResourceCounts, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_UniformBufferResourceNames, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_UniformBufferResourceBindings, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_UniformBufferResourceCounts, g_ffx_fsr2_tcr_autogen_pass_b4fce174b60032b7b7d6696ae206474b_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

