#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "barcode.h"
#include <string.h>
char character = '^';

void barcode()
{   
    uint16_t raw_value = adc_read();

    // Color 0 = white , black = 0
    int color = colorDetection(raw_value * (3.3f / (1 << 12)));

    static int previous_color = 1, color_change_count = 0, start_time = 0, end_time = 0, unit_time = 0, index = 0;
    static int sampling_time[10] = {0}, message[16] = {0};
    static absolute_time_t responseTime0 ;
    absolute_time_t  currentTime0 = get_absolute_time();
       
    if(absolute_time_diff_us(responseTime0,currentTime0)  >  3000000 && color_change_count != 0 ){
        // If there is no response of color change within a 3 second buffer , barcode it will reset.
        // Reset all variables
        previous_color = 1, color_change_count = 0, start_time = 0, end_time = 0, unit_time = 0, index = 0;
        // currentTime = 0 ;    
        memset(sampling_time, 0, sizeof sampling_time);
        memset(message, 0, sizeof message);
        printf("Time out Barcode will reset, please try again.\n");
    }

    if(color != previous_color)
    {   
        responseTime0 = get_absolute_time();
        
        
        //If detect color change enter here
        if(color_change_count == 0)
        {   
            //If detect first color change ,which is the white paper.
            printf("Barcode Detected.\n");
            
        }
        else if(color_change_count == 1)
        {   
             //If detect second color , entering black which is the start of black barcode 
            start_time = to_us_since_boot(get_absolute_time());
        }
        else
        {   
            end_time = to_us_since_boot(get_absolute_time());

            // Get time difference
            int time_taken = end_time - start_time; 

            printf("Time taken for %i color change: %i\n", color_change_count - 1, time_taken);

            if(color_change_count <= 11)
            {   
                // Asterisk sampling
                // If the color change count is within the 11, we take a fixed sample of the asterisk
                // This is to unsure the average of 1 unit (thin barcode is detected) is detected.
                // There will be more allowance for inconsistancy.
                sampling_time[color_change_count - 2] = time_taken;

                if(color_change_count == 11)
                {   
                    // Get the averaging of 1 unit time.
                    int total_time = 0;
                    for(int counter = 1; counter < sizeof(sampling_time)/sizeof(sampling_time[0]); counter++)
                    {
                        total_time += sampling_time[counter];
                    }

                    unit_time = (total_time / (sizeof(message)/sizeof(message[0])));

                    printf("Unit time: %i\n", unit_time);
                    printf("====================\n");
                }

                start_time = end_time;
                previous_color = color;
                color_change_count++;
                return;
            }
            if (time_taken > (unit_time * 5) && color_change_count % 11 == 0)
            // if (time_taken > (unit_time * 5) && color_change_count-1 % 10 == 0)
            {   
                // "Mercy condition"
                // If anomaly of 5 times of 1 unit and it is the end of a char sequence is automatic padded
                // To allow more flexibility in error , this will allow flexibility if it is the last char.
                message[index] = 0;
            }
            // Unit time * 2 as the thin : thick ratio is 1:3 , so (1+3)/2 = 2 . So 2 times should be the sample classification range.
            else if(time_taken > (unit_time * 2))
            {   
                
                for(int counter = 0; counter < 3; counter++)
                {
                    // +2 padding to message for the thick lines.
                    message[index + counter] = previous_color;
                    index += 1;
                }

                printf("Thick Barcode Detected.\n");
            }
            else
            {
                message[index] = previous_color;

                printf("Thin Barcode Detected.\n");
            }

            printf("Stored Color Detected: %i \nLast Index Stored: %d \nCurrent Message: ", previous_color, index);

            for(int counter = 0; counter < sizeof(message)/sizeof(message[0]); counter++)
            {
                printf("Current Message:%i \n", message[counter]);

                if(counter == (sizeof(message)/sizeof(message[0])) - 1)
                {
                    printf("\n\n");
                }
            }

            index++;

            if(index == 16)
            {   
                // If unit count is 16, when a char is completed.
                printf("enter 16");
                for(int counter = 0; counter < sizeof(message)/sizeof(message[0]); counter++)
                {
                    printf("Message stored: %i", message[counter]);

                    if(counter == (sizeof(message)/sizeof(message[0])) - 1)
                    {   
                        
                        printf("\n\n");
                    }
                }

                index = 0;
                // Decode the character
                character = decode(message);

                if(character == '*')
                {   
                    // If a * is detected , end barcode 
                    printf("Finish Reading Barcode.\n");

                    previous_color = 1;
                    color_change_count = index = 0;

                    return;
                }
                else
                {   
                    // Print charracter and continue algorithem.
                    printf("Message Decoded: %c\n", character);
                }
            }
        }

        // end:
        start_time = end_time;
        previous_color = color;
        color_change_count++;
    }
}

char decode(int message[])
{
    char character_map[44][18] = 
    {
        "01010001110111010", 
        "11110100010101110", 
        "21011100010101110", 
        "31110111000101010", 
        "41010001110101110", 
        "51110100011101010", 
        "61011100011101010", 
        "71010001011101110", 
        "81110100010111010", 
        "91011100010111010", 
        "A1110101000101110", 
        "B1011101000101110", 
        "C1110111010001010", 
        "D1010111000101110", 
        "E1110101110001010", 
        "F1011101110001010", 
        "G1010100011101110", 
        "H1110101000111010", 
        "I1011101000111010", 
        "J1010111000111010", 
        "K1110101010001110", 
        "L1011101010001110", 
        "M1110111010100010", 
        "N1010111010001110", 
        "O1110101110100010", 
        "P1011101110100010", 
        "Q1010101110001110", 
        "R1110101011100010", 
        "S1011101011100010", 
        "T1010111011100010", 
        "U1110001010101110", 
        "V1000111010101110", 
        "W1110001110101010", 
        "X1000101110101110", 
        "Y1110001011101010", 
        "Z1000111011101010", 
        "-1000101011101110", 
        ".1110001010111010", 
        " 1000111010111010", 
        "$1000100010001010", 
        "/1000100010100010", 
        "+1000101000100010", 
        "%1010001000100010", 
        "*1000101110111010"
    };

    int correctRow = -1;

    //Compare all data to array of string.
    for(int row = 0; row < 44; row++)
    {   
        for(int column = 1 ; column < 17 ; column++ )
        {
            correctRow = row;
            
            if(message[column - 1] + '0' !=  character_map[row][column])
            {   
                //If any difference will break , return not correct Row.
                correctRow = -1;
                break;
            }
        }

        if(correctRow != -1)
        {   
            //If it is not a incorrect row , it will return and end function.
            return character_map[correctRow][0];
        }
    }
    //If it a none encode charater , it will return '^'
    return '^';
}

int colorDetection(float voltage)
{   
    static int black_count = 1, white_count = 1;
    static float black_voltage = 3.09, white_volatage = 1.10;
    float threshold = (black_voltage / black_count + white_volatage / white_count) / 2;

    // "2 Area Average classification"
    // 2 Area Average classification to prevent noise from interupting.
    // With dynamic threshold that will find the most suitable threshold line.

    if (voltage > threshold)
    {   
        // Get average of black 
        black_voltage += voltage;
        black_count++;

        return 1;
    }
    else if(voltage <= threshold)
    {   
        // Get average of white 
        white_volatage += voltage;
        white_count++;

        return 0;
    }
}

char getCharacter(){
    return character;
}

// void initBarcode(speedA,speedB){
void initBarcode(){
    // Init barcode PIN 
    adc_gpio_init(BARCODE_PIN);
    adc_select_input(0);
    adc_init();

    while(1)
    {
        //Polling
        barcode();
        sleep_ms(10);
    }
}