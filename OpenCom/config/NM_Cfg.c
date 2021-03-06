/* Copyright(C) 2013, OpenOSEK by Fan Wang(parai). All rights reserved.
 *
 * This file is part of OpenOSEK.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Email: parai@foxmail.com
 * Sourrce Open At: https://github.com/parai/OpenOSEK/
 */

/* ================================ INCLUDEs  =============================== */
#include "Com.h"
/* ================================ MACROs    =============================== */
#define LocalNodeId 0x5A
// NM Main Task Tick = 10 ms
#define tTyp 10
#define tMax 26
#define tError 100
#define tWBS   500
#define tTx    2

#define NM_PDUID 0xFF  // For NM purpose
/* ================================ TYPEs     =============================== */

/* ================================ DATAs     =============================== */
EXPORT uint32 argNMNodeId = NM_PDUID;

/* ================================ FUNCTIONs =============================== */
// This is an example of how to write NMInit()
EXPORT void NMInit(NetIdType NetId)
{
	uint8 config[32];
	NetworkStatusType status;
	(void)memset(config,0x01,32); //care node :0,8,16,24,32,...
	if(NetId == 0)
	{
		D_DefineWindow(NetId,0x4FF,0x400, NM_PDUID,8,8);
		InitNMType(NetId,NM_DIRECT);
		InitNMScaling(NetId,0xFF); // TODO: not used
		InitCMaskTable(NetId,NM_ckNormal,config);
		InitCMaskTable(NetId,NM_ckLimphome,config);
		InitIndDeltaConfig(NetId,NM_ckNormal,SignalEvent/*SignalEvent*/,TaskNmInd,EventNmNormal);
		InitIndDeltaConfig(NetId,NM_ckLimphome,SignalEvent/*SignalEvent*/,TaskNmInd,EventNmLimphome);
		memset(&status,0,sizeof(NetworkStatusType));
		status.W.NMactive = 1;
		InitSMaskTable(NetId,&status); // TODO : not implemented for indication
		InitTargetStatusTable(NetId,&status);
		InitIndDeltaStatus(NetId,SignalEvent/*SignalEvent*/,TaskNmInd,EventNmStatus);
		InitDirectNMParams(NetId,argNMNodeId /* LocalNodeId */,tTyp,tMax,tError,tWBS,tTx);
		InitIndRingData(NetId,SignalEvent/*SignalEvent*/,TaskNmInd,EventRingData);
	}
}

TASK(TaskNmInd)
{
	StatusType ercd;
	EventMaskType mask;
	for(;;)
	{
		ercd = WaitEvent(EventNmNormal|EventNmLimphome|EventNmStatus|EventRingData);
		if(E_OK == ercd)
		{
			GetEvent(TaskNmInd,&mask);
			if((mask&EventNmNormal) != 0)
			{
				printf("In NM normal state,config changed.\n");
			}
			if((mask&EventNmLimphome) != 0)
			{
				printf("In NM limphome state,config changed.\n");
			}
			if((mask&EventNmStatus) != 0)
			{
				printf("NM network status changed.\n");
			}
			if((mask&EventRingData) != 0)
			{
				printf("NM Ring Data ind.\n");
			}
			(void)ClearEvent(EventNmNormal|EventNmLimphome|EventNmStatus|EventRingData);
		}
		else
		{
			printf("Error when Wait,ercd = %d.\n",ercd);
		}
	}
	TerminateTask();
}
