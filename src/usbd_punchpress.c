/**
  ******************************************************************************
  * @file    usbd_template.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                PUNCHPRESS Class  Description
  *          ===================================================================
  *          
  *
  *
  *
  *           
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_punchpress.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"

static uint8_t  USBD_PUNCHPRESS_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_PUNCHPRESS_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_PUNCHPRESS_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t  *USBD_PUNCHPRESS_GetCfgDesc (uint16_t *length);
static uint8_t  *USBD_PUNCHPRESS_GetDeviceQualifierDesc (uint16_t *length);
static uint8_t  USBD_PUNCHPRESS_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);


USBD_ClassTypeDef USBD_PUNCHPRESS_ClassDriver = {
  USBD_PUNCHPRESS_Init,
  USBD_PUNCHPRESS_DeInit,
  USBD_PUNCHPRESS_Setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  USBD_PUNCHPRESS_DataIn,
  NULL, /*DataOut*/
  NULL, /*SOF */
  NULL,
  NULL,      
  USBD_PUNCHPRESS_GetCfgDesc,
  USBD_PUNCHPRESS_GetCfgDesc,
  USBD_PUNCHPRESS_GetCfgDesc,
  USBD_PUNCHPRESS_GetDeviceQualifierDesc,
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
/* USB PUNCHPRESS device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_PUNCHPRESS_CfgDesc[USB_PUNCHPRESS_CONFIG_DESC_SIZ] __ALIGN_END = {
  0x09, /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_PUNCHPRESS_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xE0,         /*bmAttributes: bus powered and Supports Remote Wakeup */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
    
  /**********  Descriptor of PUNCHPRESS interface 0 **************/  
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0xff,         /*bInterfaceClass: vendor specific*/
  0xff,         /*bInterfaceSubClass: vendor specific*/
  0xff,         /*nInterfaceProtocol: vendor specific*/
  0,            /*iInterface: Index of string descriptor*/

  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07, // bLength: Endpoint Descriptor size
  USB_DESC_TYPE_ENDPOINT, // bDescriptorType:

  PUNCHPRESS_EPIN_ADDR, // bEndpointAddress: Endpoint Address (IN)
  USBD_EP_TYPE_INTR, // bmAttributes: Interrupt endpoint
  PUNCHPRESS_EPIN_SIZE, // wMaxPacketSize: Bytes
  0x00, 
  0x01, // bInterval: Polling Interval (1 ms)
  /* 34 */
};


#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
/* USB Standard Device Descriptor */
static uint8_t USBD_PUNCHPRESS_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] = {
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @brief  USBD_PUNCHPRESS_Init
  *         Initialize the PUNCHPRESS interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_PUNCHPRESS_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
	printf("USB Initialize\r\n");

	USBD_LL_OpenEP(pdev, PUNCHPRESS_EPIN_ADDR, USBD_EP_TYPE_INTR, PUNCHPRESS_EPIN_SIZE);

	pdev->pClassData = USBD_malloc(sizeof (USBD_PUNCHPRESS_HandleTypeDef));

	if(pdev->pClassData == NULL) {
		return 1;
	} else {
		((USBD_PUNCHPRESS_HandleTypeDef*)pdev->pClassData)->idle = 1;
	}
	return 0;
}

/**
  * @brief  USBD_PUNCHPRESS_Init
  *         DeInitialize the PUNCHPRESS layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_PUNCHPRESS_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
	printf("USB De-initialize\r\n");
	return USBD_OK;
}

/**
  * @brief  USBD_PUNCHPRESS_Setup
  *         Handle the PUNCHPRESS specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_PUNCHPRESS_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	printf("USBD_PUNCHPRESS_Setup\r\n");
	
	switch (req->bmRequest & USB_REQ_TYPE_MASK)
	{
		case USB_REQ_TYPE_CLASS:
			switch (req->bRequest)
			{

			default:
				USBD_CtlError (pdev, req);
				return USBD_FAIL;
			}
		break;

		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{

				default:
					USBD_CtlError (pdev, req);
				return USBD_FAIL;
			}
	}
	return USBD_OK;
}


/**
  * @brief  USBD_PUNCHPRESS_GetCfgDesc 
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_PUNCHPRESS_GetCfgDesc (uint16_t *length) {
	printf("USBD_PUNCHPRESS_GetCfgDesc\r\n");
	*length = sizeof (USBD_PUNCHPRESS_CfgDesc);
	return USBD_PUNCHPRESS_CfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_PUNCHPRESS_DeviceQualifierDescriptor(uint16_t *length) {
	printf("USBD_PUNCHPRESS_DeviceQualifierDescriptor\r\n");
	*length = sizeof (USBD_PUNCHPRESS_DeviceQualifierDesc);
	return USBD_PUNCHPRESS_DeviceQualifierDesc;
}


/**
  * @brief  USBD_PUNCHPRESS_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_PUNCHPRESS_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum) {
	((USBD_PUNCHPRESS_HandleTypeDef *)pdev->pClassData)->idle = 1;
	
	return USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_PUNCHPRESS_GetDeviceQualifierDesc (uint16_t *length) {
  *length = sizeof (USBD_PUNCHPRESS_DeviceQualifierDesc);
  return USBD_PUNCHPRESS_DeviceQualifierDesc;
}

uint8_t USBD_PUNCHPRESSS_SendPacket(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len) {
	if (pdev->dev_state == USBD_STATE_CONFIGURED) {
		if (((USBD_PUNCHPRESS_HandleTypeDef *)pdev->pClassData)->idle == 1) {
			((USBD_PUNCHPRESS_HandleTypeDef *)pdev->pClassData)->idle = 0;
			USBD_LL_Transmit (pdev, PUNCHPRESS_EPIN_ADDR, data, len);
		}
	}

	return USBD_OK;
}


uint8_t USBD_PUNCHPRESSS_SendResetMesage(USBD_HandleTypeDef *pdev) {
	uint8_t data = REBOOT;
	return USBD_PUNCHPRESSS_SendPacket(pdev, &data, sizeof(data));
}

uint8_t USBD_PUNCHPRESSS_SendFailMesage(USBD_HandleTypeDef *pdev) {
	uint8_t data = FAIL;
	return USBD_PUNCHPRESSS_SendPacket(pdev, &data, sizeof(data));
}

uint8_t USBD_PUNCHPRESSS_SendHeadPositionMesage(USBD_HandleTypeDef *pdev, int32_t x, int32_t y) {
	HeadPositionMessage data = {HEAD_POSITION, x, y};
	return USBD_PUNCHPRESSS_SendPacket(pdev, (uint8_t*)&data, sizeof(data));
}

uint8_t USBD_PUNCHPRESSS_SendHeadUpMesage(USBD_HandleTypeDef *pdev) {
	uint8_t data = HEAD_UP;
	return USBD_PUNCHPRESSS_SendPacket(pdev, &data, sizeof(data));
}

uint8_t USBD_PUNCHPRESSS_SendHeadDownMesage(USBD_HandleTypeDef *pdev) {
	uint8_t data = HEAD_DOWN;
	return USBD_PUNCHPRESSS_SendPacket(pdev, &data, sizeof(data));
}
