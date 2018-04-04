/**
  ******************************************************************************
  * @file    usbd_template_core.h
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   Header file for the usbd_template_core.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/ 
#ifndef __USB_PUNCHPRESS_CORE_H
#define __USB_PUNCHPRESS_CORE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_PUNCHPRESS
  * @brief This file is the header file for usbd_template_core.c
  * @{
  */ 


/** @defgroup USBD_PUNCHPRESS_Exported_Defines
  * @{
  */ 
#define PUNCHPRESS_EPIN_ADDR                 0x81
#define PUNCHPRESS_EPIN_SIZE                 0x10

#define USB_PUNCHPRESS_CONFIG_DESC_SIZ       34

/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_ClassTypeDef  USBD_PUNCHPRESS_ClassDriver;
#define USBD_PUNCHPRESS_CLASS &USBD_PUNCHPRESS_ClassDriver
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif  /* __USB_PUNCHPRESS_CORE_H */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


uint8_t USBD_PUNCHPRESSS_SendPacket(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len);

typedef struct {
	bool idle;
} USBD_PUNCHPRESS_HandleTypeDef;

