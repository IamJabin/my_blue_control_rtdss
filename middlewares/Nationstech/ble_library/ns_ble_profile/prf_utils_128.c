/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file prf_utils_128.c
 * @author Nations Firmware Team
 * @version v1.0.2
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

/**
 ****************************************************************************************
 * @addtogroup PRF_UTILS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_CLIENT_PRF)
#include <stdint.h>
#include <stdbool.h>
#include "ke_task.h"
#include "co_error.h"
#include "gattc_task.h"
#include "prf_utils_128.h"
#include "ke_mem.h"
#include "gap.h"
#include "gapc.h"
#include "prf.h"
#endif //(BLE_CLIENT_PRF)

#if (BLE_CLIENT_PRF)
 
void prf_search_chars_128(uint16_t svc_ehdl, uint8_t nb_chars,
                      struct prf_char_inf* chars, const struct prf_char_def_128* chars_req,
                      const struct gattc_disc_char_ind* param,
                      uint8_t* last_found_char)
{
    //Counters
    uint8_t cursor;

    //Look over requested characteristics
    for (cursor=0; cursor<nb_chars ; cursor++)
    {
        if(attm_uuid_comp((uint8_t*)param->uuid,  param->uuid_len,   (uint8_t*)chars_req[cursor].uuid, ATT_UUID_128_LEN))
        {
            //Save properties and handles
            chars[cursor].char_hdl       = param->attr_hdl;
            chars[cursor].val_hdl        = param->pointer_hdl;
            chars[cursor].prop           = param->prop;

            //Compute number of attribute in Char. using SVC edhl - Limited to 255
            chars[cursor].char_ehdl_off    = (uint8_t)(svc_ehdl - chars[cursor].char_hdl + 1);

            if (cursor != *last_found_char)
            {
                //Update number of attributes for the last found char.
                chars[*last_found_char].char_ehdl_off =
                        (uint8_t)(chars[cursor].char_hdl - chars[*last_found_char].char_hdl);
            }

            //Save code of the Char. which has been found.
            *last_found_char = cursor;
        }

    }
}
 
void prf_disc_svc_send_128(prf_env_t *env, uint8_t *uuid_128, uint8_t conidx)
{
    //send GATT discover primary services by UUID request: find by type request
    struct gattc_disc_cmd * svc_req = KE_MSG_ALLOC_DYN(GATTC_DISC_CMD,
                                                       KE_BUILD_ID(TASK_GATTC, conidx),
                                                       KE_BUILD_ID(env->prf_task, 0),
                                                       gattc_disc_cmd,
                                                       ATT_UUID_128_LEN);

    //gatt request type: by UUID
    svc_req->operation        = GATTC_DISC_BY_UUID_SVC;
    //start handle;
    svc_req->start_hdl        = ATT_1ST_REQ_START_HDL;
    //end handle
    svc_req->end_hdl          = ATT_1ST_REQ_END_HDL;

    // UUID search
    svc_req->uuid_len = ATT_UUID_128_LEN;
    
    svc_req->seq_num = 0;

    // set 128 bit UUID for searching
    memcpy(&svc_req->uuid[0], uuid_128, (sizeof(uint8_t) * ATT_UUID_128_LEN));

    //send the message to GATT, which will send back the response when it gets it
    ke_msg_send(svc_req);
}
 
uint8_t prf_check_svc_char_validity_128(uint8_t nb_chars,
                                    const struct prf_char_inf* chars,
                                    const struct prf_char_def_128* chars_req)
{
    uint8_t status = ATT_ERR_NO_ERROR;
    uint8_t i;

    for(i = 0; ((i < nb_chars) && (status == ATT_ERR_NO_ERROR)); i++)
    {
        if (chars[i].char_hdl == ATT_INVALID_HANDLE)
        {
            //If Characteristic is not present, check requirements
            if (chars_req[i].req_flag == ATT_MANDATORY)
            {
                status = PRF_ERR_STOP_DISC_CHAR_MISSING;
            }
        }
        else
        {
            //If Characteristic is present, check properties
            if((chars[i].prop & chars_req[i].prop_mand) != chars_req[i].prop_mand)
            {
                status = PRF_ERR_STOP_DISC_WRONG_CHAR_PROP;
            }
        }
    }

    return status;
}

void prf_disc_svc_sdp_send_128(prf_env_t *prf_env, uint8_t conidx,const uint8_t* uuid_128)
{
    //send GATT discover primary services by UUID request
    struct gattc_sdp_svc_disc_cmd * svc_req = KE_MSG_ALLOC(GATTC_SDP_SVC_DISC_CMD,
                                                           KE_BUILD_ID(TASK_GATTC, conidx),
                                                           prf_src_task_get(prf_env, conidx),
                                                           gattc_sdp_svc_disc_cmd);

    //gatt request type: by UUID
    svc_req->operation        = GATTC_SDP_DISC_SVC;
    //start handle;
    svc_req->start_hdl        = ATT_1ST_REQ_START_HDL;
    //end handle
    svc_req->end_hdl          = ATT_1ST_REQ_END_HDL;

    // UUID search
    svc_req->uuid_len = ATT_UUID_128_LEN;

    //set the first two bytes to the value array, LSB to MSB
    memcpy(svc_req->uuid, uuid_128, sizeof(uint8_t) * ATT_UUID_128_LEN);

    //send the message to GATT, which will send back the response when it gets it
    ke_msg_send(svc_req);
}

void prf_disc_char_by_uuid_128_send(prf_env_t *prf_env, uint8_t conidx, const uint8_t *uuid_128)
{
    //send GATT discover primary services by UUID request: find by type request
    struct gattc_disc_cmd * svc_req = KE_MSG_ALLOC_DYN(GATTC_DISC_CMD,
                                                       KE_BUILD_ID(TASK_GATTC, conidx),
                                                       prf_src_task_get(prf_env, conidx),
                                                       gattc_disc_cmd,
                                                       ATT_UUID_128_LEN);

    //gatt request type: by UUID
    svc_req->operation         = GATTC_DISC_BY_UUID_CHAR;
    //start handle;
    svc_req->start_hdl        = ATT_1ST_REQ_START_HDL;
    //end handle
    svc_req->end_hdl          = ATT_1ST_REQ_END_HDL;

    // UUID search
    svc_req->uuid_len = ATT_UUID_128_LEN;

    // set 128 bit UUID for searching
    memcpy(&svc_req->uuid[0], uuid_128, (sizeof(uint8_t) * ATT_UUID_128_LEN));

    //send the message to GATT, which will send back the response when it gets it
    ke_msg_send(svc_req);
}

void prf_extract_svc_sdp_info_128(const struct gattc_sdp_svc_ind *param,
                                  uint8_t nb_chars,
                                  const struct prf_char_def_128* chars_req,
                                  struct prf_char_inf* chars,
                                  uint8_t nb_descs,
                                  const struct prf_char_desc_def* descs_req,
                                  struct prf_char_desc_inf* descs)
{
    //Counters
    uint8_t svc_char;
    uint8_t svc_desc;
    uint8_t fnd_att;

    for (fnd_att=0; fnd_att< (param->end_hdl - param->start_hdl); fnd_att++)
    {
        if(param->info[fnd_att].att_type == GATTC_SDP_ATT_CHAR)
        {
            uint16_t char_hdl = param->start_hdl+ 1 + fnd_att;
            uint16_t val_hdl  = param->info[fnd_att].att_char.handle;
            uint8_t  char_idx = fnd_att;

            // check that value handle is in a valid range
            if((val_hdl <= param->end_hdl) && (val_hdl > (param->start_hdl + fnd_att)))
            {
                // retrieve value index
                uint8_t val_idx = (val_hdl - param->start_hdl - 1);
                //Look over requested characteristics
                for (svc_char=0; svc_char < nb_chars ; svc_char++)
                {
                    // check if attribute is valid
                    if((chars[svc_char].char_hdl == ATT_INVALID_HDL) &&
                            attm_uuid_comp((uint8_t*)param->info[val_idx].att.uuid, param->info[val_idx].att.uuid_len,
                                           (uint8_t*)chars_req[svc_char].uuid, ATT_UUID_128_LEN))
                    {
                        //Save properties and handles
                        chars[svc_char].char_hdl       = char_hdl;
                        chars[svc_char].val_hdl        = val_hdl;
                        chars[svc_char].prop           = param->info[fnd_att].att_char.prop;;

                        // find end of characteristic handle and discover descriptors
                        do
                        {
                            fnd_att++;
                            // found a descriptor
                            if(param->info[fnd_att].att_type == GATTC_SDP_ATT_DESC)
                            {
                                //Retrieve characteristic descriptor handle using UUID
                                for(svc_desc = 0; svc_desc < nb_descs; svc_desc++)
                                {
                                    // check if it's expected descriptor
                                    if ((descs[svc_desc].desc_hdl == ATT_INVALID_HANDLE)
                                        && (descs_req[svc_desc].char_code == svc_char)
                                        && (attm_uuid16_comp((uint8_t*)param->info[fnd_att].att.uuid,
                                                param->info[fnd_att].att.uuid_len, descs_req[svc_desc].uuid)))
                                    {
                                        descs[svc_desc].desc_hdl = param->start_hdl + 1 + fnd_att;
                                        // search for next descriptor
                                        break;
                                    }
                                }
                            }
                        }while(((param->start_hdl+ 1 + fnd_att) <= param->end_hdl)
                                && (param->info[fnd_att].att_type != GATTC_SDP_ATT_CHAR)
                                && (param->info[fnd_att].att_type != GATTC_SDP_INC_SVC));
                        // return to previous valid value
                        fnd_att--;
                        // previous handle was end of the characteristic
                        chars[svc_char].char_ehdl_off    = fnd_att - char_idx;

                        // search next characteristic
                        break;
                    }
                }
            }
        }
    }
}
 
#endif /*BLE_CLIENT_PRF */

/// @} PRF_UTILS
