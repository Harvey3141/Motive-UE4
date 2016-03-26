//=============================================================================
// Original Code Extended By Harvey Buchan


// Copyright © NaturalPoint, Inc. All Rights Reserved.
// 
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall NaturalPoint, Inc. or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//=============================================================================

/*
	Based on the Unity sample project. This Unreal plugin receives motion data from motive and exposes to unreal using blueprints
*/

#include "MotivePluginPrivatePCH.h"

NatNetClient* theClient;
void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData);		// receives data from the server
void __cdecl MessageHandler(int msgType, char* msg);		            // receives NatNet error mesages

char szMyIPAddress[128] = "127.0.0.1";
char szServerIPAddress[128] = "127.0.0.1";

bool clientEnabled = false;
bool isReadyAndListening = false;

TArray<FMoCapDataStruct> fMocapDataArray;

int iConnectionType = ConnectionType_Multicast;

// forward function declarations
int CreateClient(int iConnectionType);

TArray<FMoCapDataStruct> UMotiveBPFunctionLibrary::ReceiveMotive()
{
	int iResult;

	// Create NatNet client
	if (!clientEnabled) {
		iResult = CreateClient(iConnectionType);
		if (iResult != ErrorCode_OK)
		{
			UE_LOG(MotiveLog,Error, TEXT("Error initializing client.  See log for details.  Exiting"));
		}
		else
		{
			UE_LOG(MotiveLog, Warning, TEXT("Client initialized and ready.\n"));
			clientEnabled = true;
		}
	}

	if (!isReadyAndListening && clientEnabled) {
		// Send/Receive test request
		UE_LOG(MotiveLog, Warning, TEXT("[SampleClient] Sending Test Request\n"));
		void* response;
		int nBytes;
		iResult = theClient->SendMessageAndWait("TestRequest", &response, &nBytes);
		if (iResult == ErrorCode_OK)
		{
			UE_LOG(MotiveLog, Warning, TEXT("[SampleClient] Received"));
		}

		// Retrieve Data Descriptions from server
		UE_LOG(MotiveLog, Warning, TEXT("[SampleClient] Requesting Data Descriptions..."));
		sDataDescriptions* pDataDefs = NULL;
		int nBodies = theClient->GetDataDescriptions(&pDataDefs);
		if (!pDataDefs)
		{
			UE_LOG(MotiveLog, Warning, TEXT("[SampleClient] Unable to retrieve Data Descriptions."));
		}
		else
		{
			UE_LOG(MotiveLog, Warning, TEXT("[SampleClient] Received %d Data Descriptions:\n"), pDataDefs->nDataDescriptions);
			for (int i = 0; i < pDataDefs->nDataDescriptions; i++)
			{
				UE_LOG(MotiveLog, Warning, TEXT("Data Description # %d (type=%d)\n"), i, pDataDefs->arrDataDescriptions[i].type);
				if (pDataDefs->arrDataDescriptions[i].type == Descriptor_MarkerSet)
				{
					// MarkerSet
					sMarkerSetDescription* pMS = pDataDefs->arrDataDescriptions[i].Data.MarkerSetDescription;
					UE_LOG(MotiveLog, Warning, TEXT("MarkerSet Name : %s\n"), *FString(ANSI_TO_TCHAR(pMS->szName)));
					for (int i = 0; i < pMS->nMarkers; i++)
							UE_LOG(MotiveLog, Warning, TEXT("%s\n"), *FString(ANSI_TO_TCHAR(pMS->szMarkerNames[i])));
				}
				else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_RigidBody)
				{ 
					// RigidBody
					sRigidBodyDescription* pRB = pDataDefs->arrDataDescriptions[i].Data.RigidBodyDescription;
					UE_LOG(MotiveLog, Warning, TEXT("RigidBody Name : %s\n"), *FString(ANSI_TO_TCHAR(pRB->szName)));
					UE_LOG(MotiveLog, Warning, TEXT("RigidBody ID : %d"), pRB->ID);
					UE_LOG(MotiveLog, Warning, TEXT("RigidBody Parent ID : %d"), pRB->parentID);
					UE_LOG(MotiveLog, Warning, TEXT("Parent Offset : %3.2f,%3.2f,%3.2f"), pRB->offsetx, pRB->offsety, pRB->offsetz);
				}
				else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_Skeleton)
				{
					// Skeleton
					sSkeletonDescription* pSK = pDataDefs->arrDataDescriptions[i].Data.SkeletonDescription;
					UE_LOG(MotiveLog, Warning, TEXT("Skeleton Name : %s\n"), *FString(ANSI_TO_TCHAR(pSK->szName)));
					UE_LOG(MotiveLog, Warning, TEXT("Skeleton ID : %d\n"), pSK->skeletonID);
					UE_LOG(MotiveLog, Warning, TEXT("RigidBody (Bone) Count : %d\n"), pSK->nRigidBodies);
					for (int j = 0; j < pSK->nRigidBodies; j++)
					{
						sRigidBodyDescription* pRB = &pSK->RigidBodies[j];
						//printf("  Skeleton RigidBody Name : %s\n", pRB->szName);
						UE_LOG(MotiveLog, Warning, TEXT("  Skeleton RigidBody ID : %d"), pRB->ID);
						UE_LOG(MotiveLog, Warning, TEXT("  Skeleton RigidBody Parent ID : %d"), pRB->parentID);
						UE_LOG(MotiveLog, Warning, TEXT("  Parent Offset : %3.2f,%3.2f,%3.2f"), pRB->offsetx, pRB->offsety, pRB->offsetz);

						//gBoneNames[pRB->ID] = pRB->szName;
					}
				}
				else 
				{
					UE_LOG(MotiveLog, Warning, TEXT("Unknown data type"));
					// Unknown
				}
			}
			isReadyAndListening = true;
		}
	}

	return fMocapDataArray;
}

// Establish a NatNet Client connection
int CreateClient(int iConnectionType)
{
	// release previous server
	if (theClient)
	{
		theClient->Uninitialize();
		delete theClient;
	}
	
	// create NatNet client
	theClient = new NatNetClient(iConnectionType);

	// [optional] use old multicast group
	//theClient->SetMulticastAddress("224.0.0.1");
	
	// print version info
	unsigned char ver[4];
	theClient->NatNetVersion(ver);
	
	UE_LOG(MotiveLog, Warning, TEXT("NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n"), ver[0], ver[1], ver[2], ver[3]);
	
	// Set callback handlers
	theClient->SetMessageCallback(MessageHandler);
	theClient->SetVerbosityLevel(Verbosity_Debug);
	theClient->SetDataCallback(DataHandler, theClient);	// this function will receive data from the server
	
	// Init Client and connect to NatNet server
	// to use NatNet default port assigments

	int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress);
	// to use a different port for commands and/or data:
	//int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress, MyServersCommandPort, MyServersDataPort);
	if (retCode != ErrorCode_OK)
	{
		UE_LOG(MotiveLog, Error, TEXT("Unable to connect to server.  Error code: %d. Exiting"), retCode);
		return ErrorCode_Internal;
	}
	else
	{
		// print server info
		sServerDescription ServerDescription;
		memset(&ServerDescription, 0, sizeof(ServerDescription));
		theClient->GetServerDescription(&ServerDescription);
		
		if (!ServerDescription.HostPresent)
		{
			UE_LOG(MotiveLog, Error, TEXT("Unable to connect to server. Host not present. Exiting."));
			return 1;
		}
		
		UE_LOG(MotiveLog, Warning, TEXT("[SampleClient] Server application info:"));

		UE_LOG(MotiveLog, Warning, TEXT("Application: %s (ver. %d.%d.%d.%d)\n"), *FString(ANSI_TO_TCHAR(ServerDescription.szHostApp)), ServerDescription.HostAppVersion[0],
			ServerDescription.HostAppVersion[1], ServerDescription.HostAppVersion[2], ServerDescription.HostAppVersion[3]);
		
		UE_LOG(MotiveLog, Warning, TEXT("NatNet Version: %d.%d.%d.%d\n"), ServerDescription.NatNetVersion[0], ServerDescription.NatNetVersion[1],
			ServerDescription.NatNetVersion[2], ServerDescription.NatNetVersion[3]);
		UE_LOG(MotiveLog, Warning, TEXT("Client IP:%s"), *FString(ANSI_TO_TCHAR(szMyIPAddress)));
		UE_LOG(MotiveLog, Warning, TEXT("Server IP:%s"), *FString(ANSI_TO_TCHAR(szServerIPAddress)));
		UE_LOG(MotiveLog, Warning, TEXT("Server Name:%s"), *FString(ANSI_TO_TCHAR(ServerDescription.szHostComputerName)));
		
	}
	return ErrorCode_OK;

}


// DataHandler receives data from the server
void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	NatNetClient* pClient = (NatNetClient*)pUserData;
	//UE_LOG(MotiveLog, Warning, TEXT("Received frame %d\n"), data->iFrame);

	// Not sure this is thread safe, should use a mutex? 
	if (data->Skeletons>0)
	{
		if (data->nRigidBodies>0)
		{
			TArray<FMoCapDataStruct> newFMocapDataArray;
			for (int i = 0; i < data->nRigidBodies; i++) {
				FMoCapDataStruct moCapData;
				sRigidBodyData rbData = data->RigidBodies[i];
				moCapData.ID = rbData.ID;
				// Adjust for Unreal coordinate system
				moCapData.x = rbData.z * 100;
				moCapData.y = -rbData.x * 100;
				moCapData.z = rbData.y * 100;
				moCapData.qx = rbData.qx;
				moCapData.qy = rbData.qz;
				moCapData.qz = rbData.qy;
				moCapData.qw = -rbData.qw;
				newFMocapDataArray.Add(moCapData);
			}
			fMocapDataArray = newFMocapDataArray;
		}
	}
}

FMoCapDataStruct UMotiveBPFunctionLibrary::getRigidbodyData(TArray<FMoCapDataStruct> MoCapDataArray, int32 ID){
	FMoCapDataStruct fmocapDataStruct;
	for (int32 i = 0; i < MoCapDataArray.Num(); i++) {
		if (MoCapDataArray[i].ID == ID) {
			//UE_LOG(MotiveLog, Warning, TEXT("ID FOUND: %d"), MoCapDataArray[i].ID);
			fmocapDataStruct = MoCapDataArray[i];
		}
	}
	return fmocapDataStruct;
}

FVector UMotiveBPFunctionLibrary::FQuatToEuler(FQuat quaternion, EVectorOrder vectorOrder, bool reverseX, bool reverseY, bool reverseZ) {
	FVector vec = FVector(0,0,0);
	switch (vectorOrder) {
		case EVectorOrder::M_XYZ:
			vec.X = quaternion.Euler().X;
			vec.Y = quaternion.Euler().Y;
			vec.Z = quaternion.Euler().Z;
			break;
		case EVectorOrder::M_XZY:
			vec.X = quaternion.Euler().X;
			vec.Y = quaternion.Euler().Z;
			vec.Z = quaternion.Euler().Y;
			break;
		case EVectorOrder::M_YXZ:
			vec.X = quaternion.Euler().Y;
			vec.Y = quaternion.Euler().X;
			vec.Z = quaternion.Euler().Z;
			break;
		case EVectorOrder::M_YZX:
			vec.X = quaternion.Euler().Y;
			vec.Y = quaternion.Euler().Z;
			vec.Z = quaternion.Euler().X;
			break;
		case EVectorOrder::M_ZXY:
			vec.X = quaternion.Euler().Z;
			vec.Y = quaternion.Euler().X;
			vec.Z = quaternion.Euler().Y;
			break;
		case EVectorOrder::M_ZYX:
			vec.X = quaternion.Euler().Z;
			vec.Y = quaternion.Euler().X;
			vec.Z = quaternion.Euler().Y;
			break;
		default:
			break;
	}

	if (reverseX) vec.X *= -1;
	if (reverseY) vec.Y *= -1;
	if (reverseZ) vec.Z *= -1;

	return vec;
}

// MessageHandler receives NatNet error/debug messages
void __cdecl MessageHandler(int msgType, char* msg)
{
	if (msgType == 0) 
	{
		UE_LOG(MotiveLog, Warning, TEXT("Motive message handler: "), *FString(ANSI_TO_TCHAR(msg)));
	}
	else if (msgType == 1) UE_LOG(MotiveLog, Error, TEXT("Motive message handler: "), *FString(ANSI_TO_TCHAR(msg)));
}

void UMotiveBPFunctionLibrary::CloseMotiveClient() {
	theClient->Uninitialize();
	clientEnabled = false;
	isReadyAndListening = false;
	UE_LOG(MotiveLog, Warning, TEXT("Motive Client Closed"));
}

