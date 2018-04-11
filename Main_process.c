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
    
    int finish = 0;    // global flag for finish
    char one_bit_pict[ROW*COL] ;    //every pixel is stored as a char, should be 1 or 0, in a quashed 1D array
    //compressed data
int combuff[ROW*COL];
    // Number of compressed 
    N =0;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int preProcess(){
    /*
    * Capture image and convert into black and white 0x0000 or 0xFFFF
    * in place operation 
    */
    /*
    * TODO: convert all black and white pixel into single digit representation, 1 and 0
    * Store in a some data structure(maybe glocbal)
    * NOTE: This is required by the instruction. We can do this in while compressing, but we can't 
    */
    
    int x,y;
    short arr[ROW][COL];
    int count = 0
    *(Video_In_DMA_ptr + 3) = 0x0;          // Disable the video to capture one frame
    for (y = 0; y < 240; y++) {
        for (x = 0; x < 320; x++) {
            short temp2 = *(Video_Mem_ptr + (y << 9) + x);
            arr[y][x] = temp2;
            if(temp2 > 8210){
                temp2 = 0;
                one_bit_pict[count] = 0;
                count++;
            }
            else if(temp2 < 8210){
                temp2 = 0xFFFF;
                one_bit_pict[count] = 1;
                count++;
            }
            *(Video_Mem_ptr + (y << 9) + x) = temp2;
        }   
    }
    
    return 0;
}




int compress(){
    /*
    *TODO: package the picture into stream of 8-bit segment and send them one to the compressor
    *all compressing handling are done in here 
    */
    
    return 0;
}

int decompress(){

//take 24 bit number to binary
    Rready = alt_read_byte(ALT_FPGA_BRIDGE_LWH2F_OFST+RESULT_READY_PIO_BASE);
    if (Rready){
    alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_OUT_READ_REQ_PIO_BASE, 1);
    output =alt_read_hword(ALT_FPGA_BRIDGE_LWH2F_OFST+IDATA_PIO_SPAN);
    decode(out
    combuff[N] = output;
    }
    return 0;
}
int decode(){
    int i =0;
    int j =0;
    int data =0;
    int type =0;
    int current =0;
     for(i=0; i<N+1; i++){
      data = combuff[0];
       type = data>>23;
       count  = data- ((data>>23)<<23)
       
       for (j= 0; j<count; j++){
           if(type = 0){
          *(Video_Mem_ptr +current  = 0x0000
           }
           if(type =1){
           *(Video_Mem_ptr + current =0xFFFF
           }
       current ++;
     
     }
    
}
int combuff[ROW*COL];
    return 0;
}
int process(void){
    /*
    * This is the moddified old image(), same idea, press once to capture, and no switch needed
    * press again to quit 
    */
    
    *(Video_In_DMA_ptr + 3) = 0x4;              // Enable the video every time 
    while(*KEY_ptr != 0 );                      // catch long KEY holding, wait for release

        
    while (1)
    {
        if (*KEY_ptr != 0)                      // check if any KEY was pressed
        {   
            
                invert();
                preProcess();
                /*
                * processing code here, 
                */
                int n;
                while(!finish){ // doing stuff
                
                    compress();
                   
                    
                }
                while(*KEY_ptr != 0 );
                decode
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
