// #include "address_map_arm.h"
#include "stdio.h"
#include "time.h"
#include "string.h"
#include "address_map_arm.h"
#include "hps.h"
#include "socal.h"
#include "hps_soc_system.h"



#define ROW                   240
#define COL                   320
#define TEXT                  0xC9000000

/*
 * Global Variables
 */
///////////////////////////////////////////////////////////////////////////////
///////////////////////NIOS flags and buffer///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

    volatile int * KEY_ptr              = (int *) KEY_BASE;
    volatile int * Video_In_DMA_ptr = (int *) VIDEO_IN_BASE;
    volatile short * Video_Mem_ptr  = (short *) FPGA_ONCHIP_START;
    volatile short * TXT    = (short *) TEXT;
    volatile int * SW_ptr               = (int *) SW_BASE;
    int finish = 0;    // global flag for finish
    int one_bit_pict[ROW*COL] ;    //every pixel is stored as a char, should be 1 or 0, in a quashed 1D array

    int combuff[ROW*COL];           //compressed data
    int counter_for_byte =0;
    int N =0;            // Number of compressed 
	int p =0;
	int r =0;
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


    *(Video_In_DMA_ptr + 3) = 0x0;         // Disable the video to capture one frame
	printf("preProcess starts\n");
    for (y = 0; y <= ROW; y++) {
        for (x = 0; x <= COL; x++) {
            short temp2 = *(Video_Mem_ptr + (y << 9) + x);
            
            if(temp2 <= 8210){
                temp2 = 0;                // black
                one_bit_pict[x+y*COL] = 1;

            }
            else if(temp2 > 8210){
                temp2 = 0xFFFF;           // white
                one_bit_pict[x+y*COL] = 0;

            }
            *(Video_Mem_ptr + (y << 9) + x) = temp2;
			
			//if(x+y*COL<320)
				//printf("%d ", one_bit_pict[x+y*COL]);
        }   
		
    }
	/*
	one_bit_pict[0]=0;
    one_bit_pict[1]=1;
	one_bit_pict[2]=0;
	one_bit_pict[3]=1;
    */
	return 0;
}

int compress(){
    /*
    *TODO: package the picture into stream of 8-bit segment and send them one to the compressor
    *all compressing handling are done in here 
    */
	//printf("compressing with seg: %d\n", counter_for_byte);
	// OUT FIFO empty
	
  	
	
    if( ROW*COL/ 8 <= counter_for_byte){        // if compres finished then just set end of steam and return 
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_FLUSH_PIO_BASE, 1);
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_FLUSH_PIO_BASE, 0);
		//printf("FUCK OFFF! And my \n");
        return 1;
    }
	while (alt_read_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_IN_FULL_PIO_BASE)){
		printf("FFFFFFFFFFuck! %d\n",p );
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_FLUSH_PIO_BASE, 1);
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_FLUSH_PIO_BASE, 0);
		}						//wait for FIFO to be empty
                                          // if not finished then kepp going 
		if(counter_for_byte ==0 ){
			alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_RESET_BASE, 1);
			alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_RESET_BASE, 0);
		}
        int x = one_bit_pict[counter_for_byte*8];
        int i;
        for(i = 1; i < 8; i++){
			//x = (x << 1);
            x = x + (one_bit_pict[i+counter_for_byte*8] << i);
			//printf("buff stuff: %x, %d, %d \n",x, i, counter_for_byte);
        }
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_IN_WRITE_REQ_PIO_BASE, 1);        //enable RLE input 
        alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + ODATA_PIO_BASE, (char)x);
		//printf("buff stuff: %x \n",alt_read_word(ALT_FPGA_BRIDGE_LWH2F_OFST + ODATA_PIO_BASE) );
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_IN_WRITE_REQ_PIO_BASE, 0);
		
		//printf("FIFO full: %d with count: %d, input x: %x ", Ready, counter_for_byte,(unsigned char)x);
        //while(Ready == 1){			//stall till the buffer is empty
		//	printf(" waiting for FIFO\n");
		//}         
        
       
		/* DEBUG
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_IN_WRITE_REQ_PIO_BASE, 1); 
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + ODATA_PIO_BASE, (char)0xff);
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_IN_WRITE_REQ_PIO_BASE, 0);
		
		
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_FLUSH_PIO_BASE, 1);
		*/
		//printf("after we sent a seg, FIFO: %d", alt_read_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_IN_FULL_PIO_BASE));
				//printf("input x: %d ", );
        
		
		
        counter_for_byte ++;
		//alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RESULT_READY_PIO_BASE, 0);        //disable RLE input
        return 0;
    
	
}

int takeCompress(){
    //take compressed data from FIFO and store into combuff, store into the combuff 
    //will be called repeatlly, if no data to be read, then just exist 
    
    //alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_IN_WRITE_REQ_PIO_BASE, 0);        
    
    int Ready, output,npix;
    npix = 0;
    Ready = alt_read_byte(ALT_FPGA_BRIDGE_LWH2F_OFST+ RESULT_READY_PIO_BASE);
	//printf("Compressed ready : %d \n", Ready);
    if (Ready==0){
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_OUT_READ_REQ_PIO_BASE, 1);
		output =alt_read_word(ALT_FPGA_BRIDGE_LWH2F_OFST + IDATA_PIO_BASE);
		alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_OUT_READ_REQ_PIO_BASE, 0);
		//printf("output data: %d, N: %d\n", output ,N);
		combuff[N] = output;
		
		npix =  output&0x7FFFFF;
		
		//printf("output data: %d, N: %d, dataType:%x, raw output:%x,\n", npix ,N, output & 0x800000, (int)output);
		//printf("WTF IS THIS COMBUFF:%x \n", combuff[N]);
		N++;
    }
    return npix;
}
int decode(){
	printf("decoding!\n");
    int i,j, count;
	i = 0;
    int data =0;
    int type =0;
	int x = 0;
	int y = 0; 
    int current =0;    // the index of pixel
     while(current<(ROW*COL)){
       data = combuff[i];
      // type = ((data>>23) & 1);   //take the 24th bit as value bit
       //count  = data-(type<<23);
	   type = data& 0x800000;
	   count = data &0x7FFFFF;
          if((current == 0) &&(r == 0) &&(type == 0)){
			  count -=8;
			  //r ++;
		  }
       for (j= 0; j<count; j++){

           if(type == 0){
           *(Video_Mem_ptr + (y<<9)+x) =0xFFFF;
           }
           else{
           *(Video_Mem_ptr + (y<<9)+x) =0x0000;
           }
		   //printf("current: %d, x: %d, y: %d, datatype: %d, data count: %d\n", current, x, y, type, count);
			current ++;
			x++;
			if(x == COL){
				x=0;
				y++;
			}
		}
		i++;
    //printf("current: %d, x: %d, y: %d\n", current, x, y);
	}
printf("decode done, current: %d, Real pixel #: %d, i: %d, N:%d\n", current, ROW*COL, i, N);
    return 0;
}

int check(){
    int i, data ,type;
    int index = 0; 
    for(i = 0; i<N+1; i++){
        data = combuff[i];
        type = ((data>>23) & 1); 
        index = index + (data-(type << 23));  
    }
    if(index == ROW*COL){
        return 1; // if the count is equal to row*col  
    }
    return 0;
}



int initialSys(){
  	/*
  	 * flush all RLE component and FIFO buffer 
  	 */
  
    alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_OUT_READ_REQ_PIO_BASE, 0);
  	alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RESULT_READY_PIO_BASE, 1);
  	alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_FLUSH_PIO_BASE, 1);
  	alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_FLUSH_PIO_BASE, 0);
	alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_RESET_BASE, 1);
  	alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + RLE_RESET_BASE, 0);
  	alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_IN_FULL_PIO_BASE, 0);
 	alt_write_byte(ALT_FPGA_BRIDGE_LWH2F_OFST + FIFO_OUT_READ_REQ_PIO_BASE, 0);
  
    return 0;
}




int process(void){
    /*
    * This is the moddified old image(), same idea, press once to capture, and no switch needed
    * press again to quit 
    */
	int i;
    for( i =0; i < ROW*COL; i++){            // intiating array 
        one_bit_pict[i] = (char)0;
        combuff[i] = 0;
    }
    *(Video_In_DMA_ptr + 3) = 0x4;              // Enable the video every time 
    while(*KEY_ptr != 0 );                      // catch long KEY holding, wait for release

        
    while (1)
    {
        if (*KEY_ptr != 0)                      // check if any KEY was pressed
        {   
                
                
                initialSys();
				printf("INTI finished\n");
                //invert();
                preProcess();
				printf("preProcess finished\n");

                /*
                * processing code here, 
                
                //printf("asdfffff: %d" );
                while(!(counter_for_byte*8 == ROW*COL)){ // doing stuff
                
                    compress();
					//printf("compressing, %d, %d, latest data:%d \n", counter_for_byte, N, combuff[N]);
                    takeCompress();
                    
                }
				*/
				int j = 0;
				for(j=0;j<ROW*COL;j++){
					combuff[j] = 0;
				}
				p = 0;
				N=0;
				counter_for_byte=0;
				while (p<ROW*COL){
						compress();
						p=p+takeCompress();
						if( ROW*COL/ 8 <= counter_for_byte){
							printf("p: %d, counter_for_byte: %d\n", p, counter_for_byte);
						}
						//if(N%10 == 0){
							//printf("p: %d\n", p);
						//}
						//printf("compression progress: %d \n", p);
				}
				printf("compression progress: %d, %d, %d \n", p, ROW*COL, N);
				//while(!compress() || !alt_read_byte(ALT_FPGA_BRIDGE_LWH2F_OFST+ RESULT_READY_PIO_BASE)){
					//takeCompress();
				//}
				//printf("end of compression\n");
              //  while(!check()){                    //already run out of raw data, just catching compressed data
                 //   takeCompress();
              //  }
				
				
                printf("compress done\n");
                
  			            
                
                while(*KEY_ptr != 0 );
                
                while(*KEY_ptr == 0 );          //wait for KEY press to end this cycle 
				
				decode();
				printf("decode done\n");
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
    
   
    while(1){
        process();  
            
        
    }       
    
    return 0;                                               //shouldn't reach here 
}
