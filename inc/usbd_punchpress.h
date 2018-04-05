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

#define PUNCHPRESS_EPIN_ADDR 0x81
#define PUNCHPRESS_EPIN_SIZE 16

#define USB_PUNCHPRESS_CONFIG_DESC_SIZ       34

extern USBD_ClassTypeDef  USBD_PUNCHPRESS_ClassDriver;
#define USBD_PUNCHPRESS_CLASS &USBD_PUNCHPRESS_ClassDriver

#ifdef __cplusplus
}
#endif

#endif  /* __USB_PUNCHPRESS_CORE_H */

uint8_t USBD_PUNCHPRESSS_SendPacket(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len);

uint8_t USBD_PUNCHPRESSS_SendResetMesage(USBD_HandleTypeDef *pdev);
uint8_t USBD_PUNCHPRESSS_SendFailMesage(USBD_HandleTypeDef *pdev);
uint8_t USBD_PUNCHPRESSS_SendHeadPositionMesage(USBD_HandleTypeDef *pdev, int32_t x, int32_t y);
uint8_t USBD_PUNCHPRESSS_SendHeadUpMesage(USBD_HandleTypeDef *pdev);
uint8_t USBD_PUNCHPRESSS_SendHeadDownMesage(USBD_HandleTypeDef *pdev);

typedef struct {
	bool idle;
} USBD_PUNCHPRESS_HandleTypeDef;

enum PunchpressMessage: uint8_t {
	INVALID = 0,
	REBOOT = 1,
	FAIL = 2,
	HEAD_POSITION = 3,
	HEAD_UP = 4,
	HEAD_DOWN = 5
};

typedef struct {
	uint8_t type;
	int32_t x;
	int32_t y;
} HeadPositionMessage;
