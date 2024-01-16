// do not forget to load the driver
// to compile: g++ -o macro-scaler macro-scaler.C -lCAENVME
// to run: ./macro-scaler

// This code Write, Read or Write&Read the internal registers of your choice (in hex)
// it prints the results in binary
// written by Massafferri

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "/opt/CAENVMELib-3.3.0/include/CAENVMElib.h"
#include "/opt/CAENVMELib-3.3.0/include/CAENVMEtypes.h"
#include "/opt/CAENVMELib-3.3.0/include/CAENVMEoslib.h"
#include <unistd.h>
#include <time.h>

int main(int argc, char **argv)
{

  // TIMING
  
  int AcqTime = 60; //default
  const char* Filename = "Output_scaler.dat";
  if (argc>1)
  {
      AcqTime = atoi(argv[1]); 
  }
  if (argc>2)
  {
      Filename = argv[2]; 
  }
  printf("INFO: Program will acquire for %d seconds!\n",AcqTime);
  
  time_t StartProgramTime;
  time(&StartProgramTime); 
    
  // DEFINITIONS    

  CVBoardTypes VMEBoard;
  short Link,Device;
  int32_t BHandle;
  
  VMEBoard = cvV1718;
  Link = 0;

  CVAddressModifier Am	        = cvA24_U_DATA;
  CVAddressModifier AmArray[16]	= { Am, Am, Am, Am, Am, Am, Am, Am, Am, Am, Am, Am, Am, Am, Am, Am };
  
  CVDataWidth Dw32		    = cvD32;
  CVDataWidth Dw32Array[16] = { Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32, Dw32 };
  CVErrorCodes ECs[16];

  // SETTINGS
  
  uint32_t Counts[16];
  uint32_t AddressBase = 0xB00000;

  uint32_t RegReadResetCh01	= (AddressBase+0x40);
  uint32_t RegReadResetCh02	= (AddressBase+0x44);
  uint32_t RegReadResetCh03	= (AddressBase+0x48);
  uint32_t RegReadResetCh04	= (AddressBase+0x4C);
  uint32_t RegReadResetCh05	= (AddressBase+0x50);
  uint32_t RegReadResetCh06	= (AddressBase+0x54);
  uint32_t RegReadResetCh07	= (AddressBase+0x58);
  uint32_t RegReadResetCh08	= (AddressBase+0x5C);
  uint32_t RegReadResetCh09	= (AddressBase+0x60);
  uint32_t RegReadResetCh10	= (AddressBase+0x64);
  uint32_t RegReadResetCh11	= (AddressBase+0x68);
  uint32_t RegReadResetCh12	= (AddressBase+0x6C);
  uint32_t RegReadResetCh13	= (AddressBase+0x70);
  uint32_t RegReadResetCh14	= (AddressBase+0x74);
  uint32_t RegReadResetCh15	= (AddressBase+0x78);
  uint32_t RegReadResetCh16	= (AddressBase+0x7C);
    
  uint32_t AddressArray[16]	= {RegReadResetCh01, RegReadResetCh02, RegReadResetCh03, RegReadResetCh04, RegReadResetCh05, RegReadResetCh06, RegReadResetCh07, RegReadResetCh08, RegReadResetCh09, RegReadResetCh10, RegReadResetCh11, RegReadResetCh12, RegReadResetCh13, RegReadResetCh14, RegReadResetCh15, RegReadResetCh16};
 
  // V1718 input channels 
  // Ch0: 2'b10, Ch1: 2'b01
  CVRegisters InputReg = (CVRegisters)0x8;

  // OPENING VME INTERFACE MODULE
  
  if (CAENVME_Init(VMEBoard, Device, Link, &BHandle) != cvSuccess)
  {

    printf("Error\n");
    return 0;

  } else {

    printf("INFO: You have sucessfully opened the module V1718\n");

  }

  CAENVME_SystemReset(BHandle);
  
  // RUNNING 
  
  unsigned int retVal;
  bool previousReadV1151=false;
  int ievt=0;
    
  printf("\n");
  FILE *f = fopen(Filename, "w");
  printf("HEADER: ievnt channel counts aquisition_time rate\n");
  fprintf(f, "ievnt channel counts aquisition_time rate\n");
  fclose(f);

  while(true)
  {
    CAENVME_ReadRegister(BHandle, InputReg, &retVal);
    usleep(100000);
    
    if ((retVal & 0x1)==0 && previousReadV1151==false)
    {
        
        CAENVME_MultiRead(BHandle, AddressArray, Counts, 16, AmArray, Dw32Array, ECs);

        for (int ch = 0; ch < 16; ch++)
        {            
            if (ievt>0)
            { 
                if (Counts[ch]>0.0)
                {
                    printf("%d %d %zu %d %f\n",ievt,(ch+1),Counts[ch],AcqTime,float(Counts[ch])/AcqTime);
                    FILE *f = fopen(Filename, "a");
                    fprintf(f, "%d %d %zu %d %f\n",ievt,(ch+1),Counts[ch],AcqTime,float(Counts[ch])/AcqTime);
                    fclose(f);
                }
            }
        }           
        
        ievt++;
        
        printf("\n");
    }
    
    previousReadV1151=!(retVal & 0x1);
    
  }
  
  CAENVME_End(BHandle);
  
  return 0;
  
}
