/**************************************************************************//**
 * @file     main.cpp
 * @version  V1.00
 * @brief    Object detection with FreeRTOS sample code
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <string>
#include <cinttypes>

#include "BoardInit.hpp"      /* Board initialisation */
#include "log_macros.h"      /* Logging macros (optional) */

#include "BufAttributes.hpp" /* Buffer attributes to be applied */
#include "InputFiles.hpp"             /* Baked-in input (not needed for live data) */
#include "Labels.hpp"

#include "InferenceTask.hpp"
#include "DetectorPostProcessing.hpp"
#include "YoloFastestModel.hpp"       /* Model API */

#include "imlib.h"			/* Image processing */
#include "framebuffer.h"

//#define __USE_CCAP__

#if defined (__USE_CCAP__)
#include "ImageSensor.h"
#endif

#undef PI /* PI macro conflict with CMSIS/DSP */
#include "NuMicro.h"

#define MAINLOOP_TASK_PRIO	3
#define INFERENCE_TASK_PRIO	4

#define NUM_FRAMEBUF 2

typedef enum{
	eFRAMEBUF_EMPTY,
	eFRAMEBUF_FULL,
	eFRAMEBUF_INF	
}E_FRAMEBUF_STATE;

typedef struct{
	E_FRAMEBUF_STATE eState;
	image_t frameImage;
	std::vector<object_detection::DetectionResult> results;
}S_FRAMEBUF;


S_FRAMEBUF s_asFramebuf[NUM_FRAMEBUF];


#ifdef __cplusplus
extern "C" {
#endif
extern void FreeRTOS_TickHook(uint32_t u32CurrentTickCnt);

#ifdef __cplusplus
}
#endif

namespace arm {
namespace app {
    /* Tensor arena buffer */
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

    /* Optional getter function for the model pointer and its size. */
    namespace yolofastest {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace yolofastest */
} /* namespace app */
} /* namespace arm */

/* Cache policy function */
enum { NonCache_index, WTRA_index, WBWARA_index };

static void initializeAttributes() {
    /* Initialize attributes corresponding to the enums defined in mpu.hpp */
    const uint8_t WTRA =
        ARM_MPU_ATTR_MEMORY_(1, 0, 1, 0); // Non-transient, Write-Through, Read-allocate, Not Write-allocate
    const uint8_t WBWARA = ARM_MPU_ATTR_MEMORY_(1, 1, 1, 1); // Non-transient, Write-Back, Read-allocate, Write-allocate

    ARM_MPU_SetMemAttr(NonCache_index, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));
    ARM_MPU_SetMemAttr(WTRA_index, ARM_MPU_ATTR(WTRA, WTRA));
    ARM_MPU_SetMemAttr(WBWARA_index, ARM_MPU_ATTR(WBWARA, WBWARA));
}

static void loadAndEnableConfig(ARM_MPU_Region_t const *table, uint32_t cnt) {
    initializeAttributes();

    ARM_MPU_Load(0, table, cnt);

    // Enable MPU with default priv access to all other regions
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}

//frame buffer managemnet function
static S_FRAMEBUF* get_empty_framebuf()
{
	int i;

	for(i = 0; i < NUM_FRAMEBUF; i ++)
	{
		if(s_asFramebuf[i].eState == eFRAMEBUF_EMPTY)
			return &s_asFramebuf[i];
	}

	return NULL;
}

static S_FRAMEBUF* get_full_framebuf()
{
	int i;

	for(i = 0; i < NUM_FRAMEBUF; i ++)
	{
		if(s_asFramebuf[i].eState == eFRAMEBUF_FULL)
			return &s_asFramebuf[i];
	}

	return NULL;
}

static S_FRAMEBUF* get_inf_framebuf()
{
	int i;

	for(i = 0; i < NUM_FRAMEBUF; i ++)
	{
		if(s_asFramebuf[i].eState == eFRAMEBUF_INF)
			return &s_asFramebuf[i];
	}

	return NULL;
}


/* Image processing initiate function */
//Used by omv library
#define GLCD_WIDTH 320
#define GLCD_HEIGHT 240

#undef OMV_FB_SIZE
#define OMV_FB_SIZE ((GLCD_WIDTH * GLCD_HEIGHT * 2) + 1024)

__attribute__((section(".bss.sram.data"), aligned(16))) static char fb_array[OMV_FB_SIZE + OMV_FB_ALLOC_SIZE];
__attribute__((section(".bss.sram.data"), aligned(16))) static char jpeg_array[OMV_JPEG_BUF_SIZE];
__attribute__((section(".bss.sram.data"), aligned(16))) static char frame_buf1[OMV_FB_SIZE];

char *_fb_base = NULL;
char *_fb_end = NULL;
char *_jpeg_buf = NULL;
char *_fballoc = NULL;

static void omv_init()
{
	image_t frameBuffer;
	int i;

	frameBuffer.w = GLCD_WIDTH;
	frameBuffer.h = GLCD_HEIGHT;
	frameBuffer.size = GLCD_WIDTH * GLCD_HEIGHT * 2;
	frameBuffer.pixfmt = PIXFORMAT_RGB565;

	_fb_base = fb_array;
	_fb_end =  fb_array + OMV_FB_SIZE - 1;
	_fballoc = _fb_base + OMV_FB_SIZE + OMV_FB_ALLOC_SIZE; 
	_jpeg_buf = jpeg_array;

	fb_alloc_init0();

	framebuffer_init0();
	framebuffer_init_from_image(&frameBuffer);

	for(i = 0 ; i < NUM_FRAMEBUF; i++)
	{
		s_asFramebuf[i].eState = eFRAMEBUF_EMPTY;
	}
	
	framebuffer_init_image(&s_asFramebuf[0].frameImage);
	s_asFramebuf[1].frameImage.w = GLCD_WIDTH;
	s_asFramebuf[1].frameImage.h = GLCD_HEIGHT;
	s_asFramebuf[1].frameImage.size = GLCD_WIDTH * GLCD_HEIGHT * 2;
	s_asFramebuf[1].frameImage.pixfmt = PIXFORMAT_RGB565;
	s_asFramebuf[1].frameImage.data = (uint8_t *)frame_buf1;
}

static bool PresentInferenceResult(const std::vector<arm::app::object_detection::DetectionResult>& results,
								   std::vector<std::string>& labels)
{
	/* If profiling is enabled, and the time is valid. */
	info("Final results:\n");
	info("Total number of inferences: 1\n");

	for (uint32_t i = 0; i < results.size(); ++i) {
		info("%" PRIu32 ") %s(%f) -> %s {x=%d,y=%d,w=%d,h=%d}\n", i,
			labels[results[i].m_cls].c_str(),
			results[i].m_normalisedVal, "Detection box:",
			results[i].m_x0, results[i].m_y0, results[i].m_w, results[i].m_h );
	}

	return true;
}


static void DrawImageDetectionBoxes(
	const std::vector<arm::app::object_detection::DetectionResult>& results,
	image_t *drawImg,
	std::vector<std::string>& labels)
{
	for (const auto& result: results) {
		imlib_draw_rectangle(drawImg, result.m_x0, result.m_y0, result.m_w, result.m_h, COLOR_B5_MAX, 1, false);
		imlib_draw_string(drawImg, result.m_x0, result.m_y0 - 16, labels[result.m_cls].c_str(), COLOR_B5_MAX, 2, 0, 0, false,
				   false, false, false, 0, false, false);
	}
}

static void main_task(void *pvParameters) 
{
	BaseType_t ret;
	int32_t i32Err = 0;
    char chStdIn;

    info("main task running...\n");
    /* Model object creation and initialisation. */
    arm::app::YoloFastestModel model;
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::yolofastest::GetModelPointer(),
                    arm::app::yolofastest::GetModelLen())) {
        printf_err("Failed to initialise model\n");
		vTaskDelete(nullptr);
		return;
	}

	/* Setup cache poicy of tensor arean buffer */ 
	info("Set tesnor arena cache policy to WTRA \n");
    const std::vector<ARM_MPU_Region_t> mpuConfig = {
        {
            // SRAM for tensor arena
            ARM_MPU_RBAR(((unsigned int)arm::app::tensorArena),        // Base
                         ARM_MPU_SH_NON,    // Non-shareable
                         0,                 // Read-only
                         1,                 // Non-Privileged
                         1),                // eXecute Never enabled
            ARM_MPU_RLAR((((unsigned int)arm::app::tensorArena) + ACTIVATION_BUF_SZ - 1),        // Limit
                         WTRA_index) // Attribute index - Write-Through, Read-allocate
	     }
	};

    // Setup MPU configuration
    loadAndEnableConfig(&mpuConfig[0], mpuConfig.size());

	// Setup inference resource and create task
	struct ProcessTaskParams taskParam;
	QueueHandle_t inferenceProcessQueue = xQueueCreate(1, sizeof(xInferenceJob *));
	QueueHandle_t inferenceResponseQueue = xQueueCreate(1, sizeof(xInferenceJob *));
	
	taskParam.model = &model;
	taskParam.queueHandle = inferenceProcessQueue;

	ret = xTaskCreate(inferenceProcessTask, "inference task", 1 * 1024, &taskParam, INFERENCE_TASK_PRIO, nullptr);
    if (ret != pdPASS) {
		printf_err("FreeRTOS: Failed to inference process task \n");
		vTaskDelete(nullptr);
		return;
	}	

    uint8_t u8ImgIdx = 0;

    TfLiteTensor* inputTensor   = model.GetInputTensor(0);
    TfLiteTensor* outputTensor = model.GetOutputTensor(0);

    if (!inputTensor->dims) {
        printf_err("Invalid input tensor dims\n");
		vTaskDelete(nullptr);
		return;
	}
	else if (inputTensor->dims->size < 3) {
		printf_err("Input tensor dimension should be >= 3\n");
		vTaskDelete(nullptr);
		return;
	}		

    TfLiteIntArray* inputShape = model.GetInputShape(0);

    const int inputImgCols = inputShape->data[arm::app::YoloFastestModel::ms_inputColsIdx];
    const int inputImgRows = inputShape->data[arm::app::YoloFastestModel::ms_inputRowsIdx];
    const uint32_t inputChannels = inputShape->data[arm::app::YoloFastestModel::ms_inputChannelsIdx];

	// postProcess
    arm::app::object_detection::DetectorPostprocessing postProcess(0.5, 0.45, numClasses, 0);

	//label information
    std::vector<std::string> labels;
    GetLabelsVector(labels);
	
	//display framebuffer
	image_t frameBuffer;
	rectangle_t roi;

	//omv library init
	omv_init();
	framebuffer_init_image(&frameBuffer);

	S_FRAMEBUF* infFramebuf;
	S_FRAMEBUF* fullFramebuf;
	S_FRAMEBUF* emptyFramebuf;

	struct xInferenceJob* inferenceJob = new(struct xInferenceJob);

#if defined (__USE_CCAP__)
	//Setup image senosr
	ImageSensor_Init();
	ImageSensor_Config(eIMAGE_FMT_RGB565, frameBuffer.w, frameBuffer.h);
#endif

#if 1
	info("Press 'n' to run next image inference \n");
	info("Press 'q' to exit program \n");

	while((chStdIn = getchar()))
	{
		if(chStdIn == 'q')
			break;
		else if(chStdIn != 'n')
			continue;
#else
	while(1)
	{
#endif

		const uint8_t* pu8ImgSrc = get_img_array(u8ImgIdx);
		
		if (nullptr == pu8ImgSrc) {
			printf_err("Failed to get image index %" PRIu32 " (max: %u)\n", u8ImgIdx,
					   NUMBER_OF_FILES - 1);
			vTaskDelete(nullptr);
			return;
		}		

		infFramebuf = get_inf_framebuf();

		if(infFramebuf)
		{
			/* Detector post-processing*/

			inferenceJob->responseQueue = inferenceResponseQueue;
			inferenceJob->pPostProc = &postProcess;
			inferenceJob->modelCols = inputImgCols;
			inferenceJob->mode1Rows = inputImgRows;
			inferenceJob->srcImgWidth = infFramebuf->frameImage.w;
			inferenceJob->srcImgHeight = infFramebuf->frameImage.h;
			inferenceJob->results = &infFramebuf->results; //&results;
			
			xQueueReceive(inferenceResponseQueue, &inferenceJob, portMAX_DELAY);
		}

		fullFramebuf = get_full_framebuf();

		if(fullFramebuf)
		{
			//resize full image to input tensor
			image_t resizeImg;

			roi.x = 0;
			roi.y = 0;
			roi.w = fullFramebuf->frameImage.w;
			roi.h = fullFramebuf->frameImage.h;

			resizeImg.w = inputImgCols;
			resizeImg.h = inputImgRows;
			resizeImg.data = (uint8_t *)inputTensor->data.data; //direct resize to input tensor buffer
			resizeImg.pixfmt = PIXFORMAT_RGB888;

			imlib_nvt_scale(&fullFramebuf->frameImage, &resizeImg, &roi);

			/* If the data is signed. */
			if (model.IsDataSigned()) {
				arm::app::image::ConvertImgToInt8(inputTensor->data.data, inputTensor->bytes);
			}
			
			//trigger inference
			inferenceJob->responseQueue = inferenceResponseQueue;
			inferenceJob->pPostProc = &postProcess;
			inferenceJob->modelCols = inputImgCols;
			inferenceJob->mode1Rows = inputImgRows;
			inferenceJob->srcImgWidth = fullFramebuf->frameImage.w;
			inferenceJob->srcImgHeight = fullFramebuf->frameImage.h;
			inferenceJob->results = &fullFramebuf->results;
			
			xQueueSend(inferenceProcessQueue, &inferenceJob, portMAX_DELAY);
			fullFramebuf->eState = eFRAMEBUF_INF;
		}

		if(infFramebuf)
		{
			//draw bbox and render
			/* Draw boxes. */
			DrawImageDetectionBoxes(infFramebuf->results, &infFramebuf->frameImage, labels);

			//TODO: display result image

			PresentInferenceResult(infFramebuf->results, labels);
			infFramebuf->eState = eFRAMEBUF_EMPTY;
		}

		emptyFramebuf = get_empty_framebuf();
		
		if(emptyFramebuf)
		{

#if defined (__USE_CCAP__)
			//capture frame from CCAP
			ImageSensor_Capture((uint32_t)(emptyFramebuf->frameImage.data));    
#else
			//copy source image to frame buffer
			image_t srcImg;				

			srcImg.w = IMAGE_WIDTH;
			srcImg.h = IMAGE_HEIGHT;
			srcImg.data = (uint8_t *)pu8ImgSrc;
			srcImg.pixfmt = PIXFORMAT_RGB888;
		
			roi.x = 0;
			roi.y = 0;
			roi.w = IMAGE_WIDTH;
			roi.h = IMAGE_HEIGHT;

			imlib_nvt_scale(&srcImg, &emptyFramebuf->frameImage, &roi);
#endif			
			emptyFramebuf->results.clear();
			emptyFramebuf->eState = eFRAMEBUF_FULL;
		}

		u8ImgIdx ++;
		if(u8ImgIdx >= NUMBER_OF_FILES)
			u8ImgIdx = 0;
	
	}

	vTaskDelete(nullptr);
}

int main()
{
	BaseType_t ret;

    /* Initialize the UART module to allow printf related functions (if using retarget) */
    BoardInit();

	/* Create main task. */
	ret = xTaskCreate(main_task, "main task", 2 * 1024, nullptr, MAINLOOP_TASK_PRIO, nullptr);
    if (ret != pdPASS) {
		info("FreeRTOS: Failed to create main look task \n");
		return -1;
	}	

    // Start Scheduler
    vTaskStartScheduler();

    /* This is unreachable without errors. */
    info("program terminating...\n");
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	info("vApplicationMallocFailedHook\n");
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	info("vApplicationStackOverflowHook\n");
	taskDISABLE_INTERRUPTS();

  //__BKPT();
	
    //printf("Stack overflow task name=%s\n", pcTaskName);
    
	
	for( ;; );
    
    
    
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()).  The code in this
	tick hook implementation is for demonstration only - it has no real
	purpose.  It just gives a semaphore every 50ms.  The semaphore unblocks a
	task that then toggles an LED.  Additionally, the call to
	vQueueSetAccessQueueSetFromISR() is part of the "standard demo tasks"
	functionality. */

	FreeRTOS_TickHook(xTaskGetTickCount());
}
/*-----------------------------------------------------------*/


