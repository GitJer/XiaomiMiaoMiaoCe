#include "XiaomiMiaoMiaoCe.h"

/*
    Version 0.0.2
*/

//----------------------------------
// LUTV, LUT_KK and LUT_KW values taken from the actual device with a
// logic analyzer
//----------------------------------
uint8_t T_LUTV_init[15] = {0x47, 0x47, 0x01, 0x87, 0x87, 0x01, 0x47, 0x47, 0x01, 0x87, 0x87, 0x01, 0x81, 0x81, 0x01};
uint8_t T_LUT_KK_init[15] = {0x87, 0x87, 0x01, 0x87, 0x87, 0x01, 0x47, 0x47, 0x01, 0x47, 0x47, 0x01, 0x81, 0x81, 0x01};
uint8_t T_LUT_KW_init[15] = {0x47, 0x47, 0x01, 0x47, 0x47, 0x01, 0x87, 0x87, 0x01, 0x87, 0x87, 0x01, 0x81, 0x81, 0x01};

uint8_t T_LUTV_update_1[15] = {0x47, 0x47, 0x01, 0x4C, 0xC2, 0x01, 0x42, 0x82, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t T_LUT_KK_update_1[15] = {0x47, 0x47, 0x01, 0x4C, 0xC2, 0x01, 0x42, 0x82, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t T_LUT_KW_update_1[15] = {0x87, 0x87, 0x01, 0x8C, 0xC2, 0x01, 0x82, 0x82, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t T_LUTV_update_2[15] = {0x90, 0x90, 0x01, 0x90, 0x90, 0x01, 0x82, 0x82, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t T_LUT_KK_update_2[15] = {0x50, 0x50, 0x01, 0x50, 0x50, 0x01, 0x82, 0x82, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t T_LUT_KW_update_2[15] = {0x90, 0x90, 0x01, 0x90, 0x90, 0x01, 0x82, 0x82, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t T_DTM_init[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
uint8_t top_left_1[2] = {7, 1};
uint8_t top_left[22] = {11, 3, 10, 0, 10, 5, 9, 3, 7, 5, 7, 3, 9, 7, 10, 3, 10, 2, 10, 1, 10, 4};
uint8_t top_middle[22] = {11, 4, 11, 5, 9, 2, 9, 4, 8, 1, 8, 0, 9, 1, 9, 0, 11, 7, 11, 6, 10, 7};
uint8_t top_right[22] = {9, 5, 8, 3, 8, 5, 6, 1, 6, 5, 6, 3, 7, 7, 8, 4, 8, 2, 9, 6, 8, 7};
uint8_t bottom_left[22] = {4, 0, 4, 4, 3, 0, 0, 3, 0, 7, 0, 4, 4, 2, 6, 7, 5, 6, 5, 4, 4, 6};
uint8_t bottom_right[22] = {2, 2, 2, 6, 0, 0, 0, 1, 0, 6, 0, 5, 0, 2, 2, 0, 3, 6, 3, 2, 2, 4};
uint8_t background[2] = {12, 7};
uint8_t battery_low[2] = {11, 0};
uint8_t dashes[4] = {11, 2, 10, 6};
uint8_t face[14] = {1, 1, 1, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7};
uint8_t face_smile[4] = {1, 3, 1, 6};
uint8_t face_frown[4] = {1, 2, 1, 4};
uint8_t face_neutral[6] = {1, 2, 1, 3, 1, 5};
uint8_t sun[2] = {1, 0};
uint8_t fixed[2] = {11, 1};

// now define how the numbers map to the segments:
int numbers[10][11] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0},  // 0
    {2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0},   // 1
    {1, 2, 3, 5, 6, 7, 8, 10, 11, 0, 0}, // 2
    {1, 2, 3, 4, 5, 6, 10, 11, 0, 0, 0}, // 3
    {2, 3, 4, 8, 9, 10, 11, 0, 0, 0, 0}, // 4
    {1, 3, 4, 5, 6, 8, 9, 10, 11, 0, 0}, // 5
    {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0}, // 6
    {1, 2, 3, 4, 10, 0, 0, 0, 0, 0, 0},  // 7
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, // 8
    {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 0}  // 9
};

void XiaomiMiaoMiaoCe::init(uint8_t redraw)
{
    // set the pin modes (note: no hardware SPI is used)
    pinMode(SPI_ENABLE, OUTPUT);
    pinMode(SPI_MOSI, OUTPUT);
    pinMode(IO_RST_N, OUTPUT);
    pinMode(SPI_CLOCK, OUTPUT);
    pinMode(IO_BUSY_N, INPUT);

    // set all output to 0
    digitalWrite(SPI_ENABLE, 0);
    digitalWrite(SPI_MOSI, 0);
    digitalWrite(IO_RST_N, 0);
    digitalWrite(SPI_CLOCK, 0);

    // disable SPI (SPI enable is low active)
    digitalWrite(SPI_ENABLE, 1);

    // after some delay set RST to 1
    delayMicroseconds(100000);
    digitalWrite(IO_RST_N, 1);

    if (redraw != 0)
        // start an initialisation sequence
        send_sequence(T_LUTV_init, T_LUT_KK_init,
                      T_LUT_KW_init, T_DTM_init, 1);
}

void XiaomiMiaoMiaoCe::send_sequence(uint8_t *dataV, uint8_t *dataKK,
                                     uint8_t *dataKW, uint8_t *data,
                                     uint8_t is_init)
{
    // send PSR_PanelSetting_CMD and PWR_CMD, then start the charge pump
    transmit(0, PSR_PanelSetting_CMD);
    transmit(1, 0x0F);
    transmit(0, PWR_CMD);
    transmit(1, 0x0F);
    transmit(1, 0x0F);
    transmit(0, CPON_ChargePumpON_CMD);

    // wait for the display to become ready to receive new
    // commands/data: when ready, the display sets IO_BUSY_N to 1
    while (digitalRead(IO_BUSY_N) == 0)
        delay(1);

    // send two FRC commands as initialisation, afterwards only one
    transmit(0, FRC_FrameRateControl_CMD);
    transmit(1, 0x05);
    if (is_init == 1)
    {
        transmit(0, FRC_FrameRateControl_CMD);
        transmit(1, 0x01);
    }

    // send the e-paper voltage settings (waves)
    transmit(0, LUTV_VcomLUT_CMD);
    for (int i = 0; i < 15; i++)
        transmit(1, dataV[i]);
    transmit(0, LUT_KK_BlackBlackLUT_CMD);
    for (int i = 0; i < 15; i++)
        transmit(1, dataKK[i]);
    transmit(0, LUT_KW_BlackWhiteLUT_CMD);
    for (int i = 0; i < 15; i++)
        transmit(1, dataKW[i]);

    // send the actual data
    transmit(0, DTM_DataStartTransmission_CMD);
    for (int i = 0; i < 13; i++)
        transmit(1, data[i]);
    transmit(0, DSP_DataStop_CMD);
    transmit(0, DRF_DisplayRefresh_CMD);

    // wait for the display to become ready to receive new
    // commands/data: when ready, the display sets IO_BUSY_N to 1
    while (digitalRead(IO_BUSY_N) == 0)
        delay(1);

    // send the CPOF_ChargePumpOFF_CMD command
    transmit(0, CPOF_ChargePumpOFF_CMD);
    transmit(1, 0x03);
    delay(5);
}

void XiaomiMiaoMiaoCe::transmit(uint8_t cd, uint8_t data_to_send)
{
    // enable SPI
    digitalWrite(SPI_ENABLE, 0);
    delayMicroseconds(40);

    // send the first bit, this indicates if the following is a command or data
    digitalWrite(SPI_CLOCK, 0);
    if (cd != 0)
        digitalWrite(SPI_MOSI, 1);
    else
        digitalWrite(SPI_MOSI, 0);
    delayMicroseconds(40);
    digitalWrite(SPI_CLOCK, 1);
    delayMicroseconds(40);

    // send 8 bytes
    for (int i = 0; i < 8; i++)
    {
        // start the clock cycle
        digitalWrite(SPI_CLOCK, 0);
        // set the MOSI according to the data
        if (data_to_send & 0x80)
            digitalWrite(SPI_MOSI, 1);
        else
            digitalWrite(SPI_MOSI, 0);
        // prepare for the next bit
        data_to_send = (data_to_send << 1);
        delayMicroseconds(40);
        // the data is read at rising clock (halfway the time MOSI is set)
        digitalWrite(SPI_CLOCK, 1);
        delayMicroseconds(40);
    }

    // finish by ending the clock cycle and disabling SPI
    digitalWrite(SPI_CLOCK, 0);
    delayMicroseconds(60);
    digitalWrite(SPI_ENABLE, 1);
    delayMicroseconds(60);
}

void XiaomiMiaoMiaoCe::write_display()
{
    // Indicate that new data is going to be send
    digitalWrite(IO_RST_N, 1);
    // remove the current displayed segments in preparation of
    // the new segments to be displayed
    send_sequence(T_LUTV_update_1, T_LUT_KK_update_1, T_LUT_KW_update_1,
                  display_data, 0);
    // display the new segments
    send_sequence(T_LUTV_update_2, T_LUT_KK_update_2, T_LUT_KW_update_2,
                  display_data, 0);
    // Indicate end of new data
    digitalWrite(IO_RST_N, 0);
}

void XiaomiMiaoMiaoCe::set_number(uint8_t number, uint8_t where)
{
    // check if the input is valid
    if ((number >= 0) and (number < 10) and (where >= 2) and (where <= 6))
    {
        // set which segments are to be used
        uint8_t *segments;
        if (where == TOP_LEFT)
            segments = top_left;
        else if (where == TOP_MIDDLE)
            segments = top_middle;
        else if (where == TOP_RIGHT)
            segments = top_right;
        else if (where == BOTTOM_LEFT)
            segments = bottom_left;
        else if (where == BOTTOM_RIGHT)
            segments = bottom_right;
        else if (where == TOP_MIDDLE)
            segments = top_middle;
        else if (where == TOP_MIDDLE)
            segments = top_middle;

        // set the segments, there are up to 11 segments in a number
        int segment_byte;
        int segment_bit;
        for (int i = 0; i < 11; i++)
        {
            // get the segment needed to display the number 'number',
            // this is stored in the array 'numbers'
            int segment = numbers[number][i] - 1;
            // segment = -1 indicates that there are no more segments to display
            if (segment >= 0)
            {
                segment_byte = segments[2 * segment];
                segment_bit = segments[1 + 2 * segment];
                set_segment(segment_byte, segment_bit, 1);
            }
            else
                // there are no more segments to be displayed
                break;
        }
    }
}

void XiaomiMiaoMiaoCe::set_shape(uint8_t where)
{
    int num_of_segments = 0;
    uint8_t *segments = NULL;

    // set the number of segments and which segments has to be displayed
    if (where == TOP_LEFT_1)
    {
        num_of_segments = 1;
        segments = top_left_1;
    }
    else if (where == BATTERY_LOW)
    {
        num_of_segments = 1;
        segments = battery_low;
    }
    else if (where == DASHES)
    {
        num_of_segments = 2;
        segments = dashes;
    }
    else if (where == FACE)
    {
        num_of_segments = 7;
        segments = face;
    }
    else if (where == FACE_SMILE)
    {
        num_of_segments = 2;
        segments = face_smile;
    }
    else if (where == FACE_FROWN)
    {
        num_of_segments = 2;
        segments = face_frown;
    }
    else if (where == FACE_NEUTRAL)
    {
        num_of_segments = 3;
        segments = face_neutral;
    }
    else if (where == SUN)
    {
        num_of_segments = 1;
        segments = sun;
    }
    else if (where == FIXED)
    {
        num_of_segments = 1;
        segments = fixed;
    }
    else
        return;

    // set the segments
    for (uint8_t segment = 0; segment < num_of_segments; segment++)
    {
        uint8_t segment_byte = segments[2 * segment];
        uint8_t segment_bit = segments[1 + 2 * segment];
        set_segment(segment_byte, segment_bit, 1);
    }
}

void XiaomiMiaoMiaoCe::set_segment(uint8_t segment_byte, uint8_t segment_bit,
                                   uint8_t value)
{
    // depending on whether the display is inverted and the desired value
    // the bit needs to be set or cleared
    if (((inverted == 0) and (value == 1)) or
        ((inverted == 1) and (value == 0)))
        // remove the bit
        display_data[segment_byte] &= ~(1 << segment_bit);
    else
        // set the bit
        display_data[segment_byte] |= (1 << segment_bit);
}

void XiaomiMiaoMiaoCe::start_new_screen(uint8_t _inverted)
{
    if (_inverted == 1)
        inverted = 1;
    else
        inverted = 0;

    // prepare the data to be displayed, assume all segments are on or off
    if (_inverted)
    {
        for (int i = 0; i < 15; i++)
            display_data[i] = 0x00;
        // set the bit to switch the background to black,
        // use value=0 because of inversion
        set_segment(12, 7, 0);
    }
    else
        for (int i = 0; i < 15; i++)
            display_data[i] = 0xFF;
}