#include "tvm/runtime/c_runtime_api.h"
#ifdef __cplusplus
extern "C" {
#endif
__attribute__((section(".bss.noinit.tvm"), aligned(16)))
static uint8_t global_workspace[1509600];
#include <tvmgen_default.h>
TVM_DLL int32_t tvmgen_default___tvm_main__(void* tfl_quantize,void* output0,uint8_t* global_workspace_0_var,void* ethos_u);
int32_t tvmgen_default_run(struct tvmgen_default_inputs* inputs,struct tvmgen_default_outputs* outputs,struct tvmgen_default_devices* devices) {return tvmgen_default___tvm_main__(inputs->tfl_quantize,outputs->MobilenetV2_Predictions_Reshape_11,global_workspace,devices->ethos_u);
}
#ifdef __cplusplus
}
#endif
;