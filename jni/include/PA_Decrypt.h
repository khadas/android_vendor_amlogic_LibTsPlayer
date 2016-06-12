/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PA_Decrypt_H_

#define PA_Decrypt_H_


//#include <utils/Errors.h>


#ifdef __cplusplus
extern "C" {
#endif


/*struct hevc_info{
	const uint8_t *sps;
	size_t sps_size;
	int mwidth;
	int mheight;
};*/


/*extern  int  PA_HEVC_FindNAL(
        const uint8_t *data, size_t size, unsigned nalType);


 int HEVC_decode_SPS(const uint8_t *buf,int size,struct hevc_info*info);
int HEVC_parse_keyframe(const uint8_t *buf,int size);

int32_t HEVCCastSpecificData(uint8_t * data, int32_t size);

extern  int PA_HEVC_getNextNALUnit(
			const uint8_t **_data, size_t *_size,
			const uint8_t **nalStart, size_t *nalSize,
			int startCodeFollows) ;*/
extern int  PA_DecryptContentData(unsigned char byEncryptFlag, unsigned char* pbyData, int* pnDataLen);


#ifdef __cplusplus
}
#endif
#endif

