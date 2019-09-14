#ifndef _HT16E07_H__
#define _HT16E07_H__

/*
    Version 0.0.2
*/

#include <stdint.h>
#include <Arduino.h>

//----------------------------------
// define commands and settings
// see document: https://www.holtek.com/documents/10179/116711/HT16E07v100.pdf
// see code: https://www.holtek.com/documents/10179/116745/an0461.zip
//----------------------------------
#define PSR_PanelSetting_CMD 0x00
#define PSR_PanelSetting_default 0x8f

#define PSR_BWR_Mark ~(1 << 7)
#define PSR_BWR_Red 0 << 7
#define PSR_BWR_BlackWhite 1 << 7
#define PSR_VDLH_EN_Mark ~(1 << 3)
#define PSR_VDLH_EN_External 0 << 3
#define PSR_VDLH_EN_Generate 1 << 3
#define PSR_DDX_Mark ~(1 << 2)
#define PSR_DDX_0_White_1_BlackRed 0 << 2
#define PSR_DDX_0_BlackRed_1_White 1 << 2
#define PSR_SHD_N_Mark ~(1 << 1)
#define PSR_SHD_N_ChargePumpOff 0 << 1
#define PSR_SHD_N_ChargePumpOn 1 << 1
#define PSR_REST_N_Mark ~(1 << 0)
#define PSR_REST_N_RestAll 0 << 0
#define PSR_REST_N_Normal 1 << 0
//----------------------------------
#define PWR_CMD 0x01
#define PWR_VDH_LV 0x28
#define PWR_VDL_LV 0x00
//----------------------------------
#define CPOF_ChargePumpOFF_CMD 0x02
#define CPOF_SGM_FL_Floating 0 << 1
#define CPOF_SGM_FL_Ground 1 << 1
#define CPOF_VCOM_FL_Floating 0 << 0
#define CPOF_VCOM_FL_Ground 1 << 0
//----------------------------------
#define CPON_ChargePumpON_CMD 0x04
//----------------------------------
#define CBS_COMBGSet_CMD 0x05
#define CBS_CBS3_AS_COM 0 << 3
#define CBS_CBS3_AS_BG 1 << 3
#define CBS_CBS2_AS_COM 0 << 2
#define CBS_CBS2_AS_BG 1 << 2
#define CBS_CBS1_AS_COM 0 << 1
#define CBS_CBS1_AS_BG 1 << 1
#define CBS_CBS0_AS_COM 0 << 0
#define CBS_CBS0_AS_BG 1 << 0
//----------------------------------
#define DTM_DataStartTransmission_CMD 0x10
//----------------------------------
#define DSP_DataStop_CMD 0x11
#define DSP_DataFlag_Mark 1 << 8
#define DSP_DataFlag_NotReceiveAllData_R 0 << 8
#define DSP_DataFlag_HasReceivedAllData_R 1 << 8
//----------------------------------
#define DRF_DisplayRefresh_CMD 0x12
//----------------------------------
#define LUTV_VcomLUT_CMD 0x20

#define LVL_x_VDL 0 << 7 | 0 << 6
#define LVL_x_VDH 0 << 7 | 1 << 6
#define LVL_x_0V 1 << 7 | 0 << 6
#define LVL_x_Floating 1 << 7 | 1 << 6

#define LUTV_FRM_x1 0x00
#define LUTV_FRM_x2 0x00
#define LUTV_PHS 0x00
//----------------------------------
#define LUT_KK_BlackBlackLUT_CMD 0x21
#define LUT_KK_FRM_x1 0x00
#define LUT_KK_FRM_x2 0x00
#define LUT_KK_PHS 0x00
//----------------------------------
#define LUT_KW_BlackWhiteLUT_CMD 0x22
#define LUT_KW_FRM_x1 0x00
#define LUT_KW_FRM_x2 0x00
#define LUT_KW_PHS 0x00
//----------------------------------
#define LUT_WK_WhiteBlackLUT_CMD 0x23
#define LUT_WK_FRM_x1 0x00
#define LUT_WK_FRM_x2 0x00
#define LUT_WK_PHS 0x00
//----------------------------------
#define LUT_WW_WhiteWhiteLUT_CMD 0x24
#define LUT_WW_FRM_x1 0x00
#define LUT_WW_FRM_x2 0x00
#define LUT_WW_PHS 0x00
//----------------------------------
#define FRC_FrameRateControl_CMD 0x30
#define FRC_SF 0x10
#define FRC_NF 0x00
#define FRC_P 0x03
//----------------------------------
#define TSC_TemperatureSensor_CMD 0x40
#define TSC_Busy_N_Mark 1 << 8
#define TSC_TS_Mark 0x0f
//----------------------------------
#define REV_Revision_CMD 0x70
//----------------------------------
#define FLG_GetStatus_CMD 0x71
#define FLG_DTS_ON_Mark 1 << 4
#define FLG_Data_Flag_Mark 1 << 3
#define FLG_PON_Mark 1 << 2
#define FLG_POFF_Mark 1 << 1
#define FLG_BUSY_N_Mark 1 << 0
//----------------------------------
#define CascadeSetting_CMD 0xE0
#define CascadeSetting_CAS_EN_Mark 1 << 0
//----------------------------------
#define TestMode_CMD 0xFE

//----------------------------------
// define pins (made such that it can easily connect the ESP8266 Wemos d1 mini
// to the pin-to-flatflex connector)
//----------------------------------
#define SPI_ENABLE D7
#define SPI_MOSI D6
#define IO_RST_N D5
#define SPI_CLOCK D1
#define IO_BUSY_N D2

//----------------------------------
// define groups of segments into logical shapes
//----------------------------------
#define TOP_LEFT_1 1
#define TOP_LEFT 2
#define TOP_MIDDLE 3
#define TOP_RIGHT 4
#define BOTTOM_LEFT 5
#define BOTTOM_RIGHT 6
#define BACKGROUND 7
#define BATTERY_LOW 8
#define DASHES 9
#define FACE 10
#define FACE_SMILE 11
#define FACE_FROWN 12
#define FACE_NEUTRAL 13
#define SUN 14
#define FIXED 15

//----------------------------------
// define the class for driving the Xiaomi MiaoMiaoCe e-ink display
//----------------------------------
class XiaomiMiaoMiaoCe
{
public:
    /**
     * Initialize the display
     * @param redraw    should the screen should have an black-white redraw?
     *                  redraw = 0: no redraw
     *                  redraw != 0: do a redraw 
     *                  normally redrawing is advisable (redraw != 0), but e.g. 
     *                  when using deep-sleep you may want to initialize the 
     *                  screen without the black-white transition
     */

    void init(uint8_t redraw);
    /** display the data
     */
    void write_display();

    /** 
     * Display a number at a specific place
     * @param number    The number to be displayed [0, 9]
     * @param where     The location where to display the number, there are 5 
     *                  locations: [TOP_LEFT, TOP_MIDDLE, TOP_RIGHT, 
     *                  BOTTOM_LEFT, BOTTOM_RIGHT]
     */
    void set_number(uint8_t number, uint8_t where);

    /** 
     * Display a defined shape
     * @param where     Which shapes should be turned on, there are several 
     *                  pre-defined shapes: [TOP_LEFT_1, BACKGROUND, 
     *                  BATTERY_LOW, DASHES, FACE, FACE_SMILE, FACE_FROWN, 
     *                  FACE_NEUTRAL, SUN, FIXED]
     *                  But if you want you can define your own, see the 
     *                  #defines above and the variables in XiaomiMiaoMiaoCe.cpp
     */
    void set_shape(uint8_t where);

    /**
     * Set a segment to a value
     * The e-ink display has 73 segments (the driver can handle 120) which are
     * distributed over 15 bytes with 8 bits each: 15*8 = 120
     * @param segment_byte  The byte in which the segments is located
     * @param segment_bit   The bit in the byte
     * @param value         0 = off, 1 = on
     */
    void set_segment(uint8_t segment_byte, uint8_t segment_bit, uint8_t value);

    /** 
     * Start building a new display
     * @param inverted  0 = inverted display, 1 = normal
    */
    void start_new_screen(uint8_t inverted = 0);

private:
    /**
     * Transmit data to the display via SPI
     * @param cd    is it a command or data? 0 = command, 1 = data
     * @param data  1 byte of data
     */
    void transmit(uint8_t cd, uint8_t data);

    /** 
     * Send data to the display
     * The e-ink display has 'waveforms' of voltages to be applied to the 
     * segments when changing from black to white or white to black. These
     * differ between initialising and updating the e-ink display
     * @param dataV     one of the waveforms
     * @param dataKK    one of the waveforms
     * @param dataKW    one of the waveforms
     * @param data      the data containing which segments are on or off
     * @param is_init   is this an initialisation or an update
     */
    void send_sequence(uint8_t *dataV, uint8_t *dataKK, uint8_t *dataKW,
                       uint8_t *data, uint8_t is_init);

    // boolean to indicate if the screen is inverted or not
    uint8_t inverted = 0;
    // The array in which the segments to be displayed are placed
    uint8_t display_data[16];
};

#endif