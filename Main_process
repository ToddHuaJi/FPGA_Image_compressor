// #include "address_map_arm.h"
#include "stdio.h"
#include "time.h"
#include "string.h"
#include "address_map_arm"
#include "hps"
#include "socal"
#include "hps_soc_system"

#define KEY_BASE              0xFF200050
#define VIDEO_IN_BASE         0xFF203060
#define FPGA_ONCHIP_BASE      0xC8000000
#define TEXT                  0xC9000000
#define SW_BASE               0xFF200040 

#define ROW                   240
#define COL                   320
/*
 * Global Variables
 */
///////////////////////////////////////////////////////////////////////////////
///////////////////////NIOS flags and buffer///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

    volatile int * KEY_ptr              = (int *) KEY_BASE;
    volatile int * Video_In_DMA_ptr = (int *) VIDEO_IN_BASE;
    volatile short * Video_Mem_ptr  = (short *) FPGA_ONCHIP_BASE;
    volatile short * TXT    = (short *) TEXT;
    volatile int * SW_ptr               = (int *) SW_BASE;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int invert(){
	/*
	* Capture image and convert into black and white 0x0000 or 0xFFFF
	* in place operation 
	*/
    int x,y;
	short arr[ROW][COL];
	*(Video_In_DMA_ptr + 3) = 0x0;          // Disable the video to capture one frame
    for (y = 0; y < 240; y++) {
        for (x = 0; x < 320; x++) {
            short temp2 = *(Video_Mem_ptr + (y << 9) + x);
			arr[y][x] = temp2;
            if(temp2 > 8210){
                temp2 = 0;
            }
            else if(temp2 < 8210){
                temp2 = 0xFFFF;
            }
            *(Video_Mem_ptr + (y << 9) + x) = temp2;
        }	
    }
	
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
//////////////////////Gongtao's functions//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Welcome(){
	char* welcome = "Press to take picture, and press again to process";
    int offset;
    
    offset = ( 1 << 7) + 1; 
    while ( *(welcome) )
    {
        *(TXT + offset) = *(welcome); // write to the character buffer
        //printf("%s",*(TXT + offset));
        ++welcome;
        ++offset;
    }
  
}

 
/*
* main code below, image() and  main()
*/
int convert(){
	/*
	* TODO: convert all black and white pixel into single digit representation, 1 and 0
	* Store in a some data structure(maybe glocbal)
	* NOTE: This is required by the instruction. We can do this in while compressing, but we can't 
	*/
	
	return 0;
}

int compress(){
	/*
	*TODO: package the picture into stream of 8-bit segment and send them one to the compressor
	*all compressing handling are done in here 
	*/
	
	return 0;
}

int process(void){
	/*
	* This is the moddified old image(), same idea, press once to capture, and no switch needed
	* press again to quit 
	*/
	
	*(Video_In_DMA_ptr + 3) = 0x4;              // Enable the video every time 
	while(*KEY_ptr != 0 );						// catch long KEY holding, wait for release

		
    while (1)
    {
        if (*KEY_ptr != 0)                      // check if any KEY was pressed
        {   
            
				invert();
				/*
				* processing code here, 
				*/
				convert();
				compress();
				
				while(*KEY_ptr != 0 );
                while(*KEY_ptr == 0 );          //wait for KEY press to end this cycle 
                return 0;
            
		}
	}
	
	return 0;
}


int main(void){                                             //Da main code
    //Welcome();
    /*
    * HERE comes da main code 
    */
	int y;
	text_ptr = " ";
    while(1){
        process();      
		for(y = 0 ; y<(80*60); y++){
			*(TXT + y) = *(text_ptr); // write to the character buffer
		}
	}		
    
    return 0;                                               //shouldn't reach here 
}
