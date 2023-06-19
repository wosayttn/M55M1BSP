#ifndef TVMGEN_DEFAULT_H_
#define TVMGEN_DEFAULT_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Input tensor tfl_quantize size (in bytes) for TVM module "default" 
 */
#define TVMGEN_DEFAULT_TFL_QUANTIZE_SIZE 150528
/*!
 * \brief Output tensor MobilenetV2_Predictions_Reshape_11 size (in bytes) for TVM module "default" 
 */
#define TVMGEN_DEFAULT_MOBILENETV2_PREDICTIONS_RESHAPE_11_SIZE 1001
/*!
 * \brief Input tensor pointers for TVM module "default" 
 */
struct tvmgen_default_inputs {
  void* tfl_quantize;
};

/*!
 * \brief Output tensor pointers for TVM module "default" 
 */
struct tvmgen_default_outputs {
  void* MobilenetV2_Predictions_Reshape_11;
};

/*!
 * \brief Device context pointers for TVM module "default" 
 */
struct tvmgen_default_devices {
  void* ethos_u;
};

/*!
 * \brief entrypoint function for TVM module "default"
 * \param inputs Input tensors for the module 
 * \param outputs Output tensors for the module 
 * \param devices Device context pointers for the module 
 */
int32_t tvmgen_default_run(
  struct tvmgen_default_inputs* inputs,
  struct tvmgen_default_outputs* outputs,
  struct tvmgen_default_devices* devices
);
/*!
 * \brief Workspace size for TVM module "default" 
 */
#define TVMGEN_DEFAULT_WORKSPACE_SIZE 1509600

#ifdef __cplusplus
}
#endif

#endif // TVMGEN_DEFAULT_H_
