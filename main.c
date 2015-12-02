//    This file is part of Pebble Seizure Detect.
//
//    Pebble Seizure Detect is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Pebble Seizure Detect is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    A copy of the GNU General Public License can be found at http://www.gnu.org/licenses/

[REMOVE THIS]

#include <pebble.h>

#define SEIZURE_MESSAGE_DELAY_SECONDS 15
    
// We're sampling at 10Hz, so, for example, 500 samples is 50 seconds
#define MAX_MOVING_AVERAGE_DURATION_IN_SECONDS 30
#define NUM_SAMPLES (MAX_MOVING_AVERAGE_DURATION_IN_SECONDS * 10)
#define NO_SAMPLE 1000
#define SEIZURE_DETECTION_VALUE 125
#define SPIKE_CUTOFF (SEIZURE_DETECTION_VALUE * 3)
#define NUM_TEXT_ROWS 6

#define MENU_MAIN 0
#define MENU_DATA 1
#define MENU_PANIC 2
#define MENU_TEMP_DISABLE 3
#define MENU_PANIC_SENT 4
#define MENU_SEIZURE_DETECT 5
#define MENU_SEIZURE_CANCELED 6
#define MENU_SEIZURE_SENT 7
#define MENU_BLUETOOTH_DISCONNECT 8
    
static Window *s_main_window;
static TextLayer *textRow[NUM_TEXT_ROWS];
static char textRowContents[NUM_TEXT_ROWS][128];
static int storedMagnitudes[NUM_SAMPLES];
static int tempMagnitudesFiltered[NUM_SAMPLES];
static int currentMenu;
static int lastMenu;
static int currentSelection;
static int panicPressesRemaining;
static int cancelPressesRemaining;
static time_t disableUntil;
static time_t lastTextRenderTime;
static time_t seizureSendTime;
static int lastLocationPingSecond;

//Forward declarations
static void RenderMenu();
void ResetMagnitudes();
static void send_state_message(int stateVal);

/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher  https://www-users.cs.york.ac.uk/~fisher/mkfilter/  */

#define NZEROS 20
#define NPOLES 20
#define GAIN   5.801011156e+03

static float xv[NZEROS+1], yv[NPOLES+1];

static void SetHighlightedRow(int rowNum)
{
    for (int i=0; i<NUM_TEXT_ROWS; i++)
    {
        if (i == rowNum)
        {
            text_layer_set_background_color(textRow[i], GColorBlack);
            text_layer_set_text_color(textRow[i], GColorWhite);
        }
        else
        {
            text_layer_set_background_color(textRow[i], GColorWhite);
            text_layer_set_text_color(textRow[i], GColorBlack);
        }
    }
}

static void SetMenuFonts()
{
    for (int i=0; i<NUM_TEXT_ROWS; i++)
    {
        text_layer_set_font(textRow[i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        if (currentMenu == MENU_MAIN)
        {
            if (i == 0) text_layer_set_font(textRow[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
            if (i == 4) text_layer_set_font(textRow[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
            if (i == 5) text_layer_set_font(textRow[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        }
        else if ((currentMenu == MENU_PANIC) || (currentMenu == MENU_SEIZURE_DETECT))
        {
            if (i == 0) text_layer_set_font(textRow[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        }
    }
}

static void ActivateMenu(int mn)
{
    panicPressesRemaining = 3;
    currentMenu = mn;
    lastMenu = -1;
    SetMenuFonts();
    currentSelection = -1;
    if (currentMenu == MENU_MAIN) currentSelection = 1;
    if (currentMenu == MENU_TEMP_DISABLE) currentSelection = 0;
    SetHighlightedRow(currentSelection);
    RenderMenu();
}

static void RenderMenu()
{
    if (currentMenu == MENU_MAIN)
    {
        snprintf(textRowContents[0], sizeof(textRowContents[0]), "       MAIN MENU       ");
        snprintf(textRowContents[1], sizeof(textRowContents[1]), "            PANIC");
        snprintf(textRowContents[2], sizeof(textRowContents[2]), "        SHOW DATA");
        snprintf(textRowContents[3], sizeof(textRowContents[3]), "     TEMP DISABLE");
        if (disableUntil > 0)
        {
            strftime(textRowContents[4], sizeof(textRowContents[4]), "Disabled to: %I:%M", localtime(&disableUntil));
        }
        else
        {
            snprintf(textRowContents[4], sizeof(textRowContents[4]), "\n");
        }
        time_t tempTime;
        tempTime = time(NULL);
        strftime(textRowContents[5], sizeof(textRowContents[5]), "            %I:%M", localtime(&tempTime));
    }
    else if (currentMenu == MENU_PANIC)
    {
        snprintf(textRowContents[0], sizeof(textRowContents[0]), "          PANIC     ");
        snprintf(textRowContents[1], sizeof(textRowContents[1]), "\n");
        snprintf(textRowContents[2], sizeof(textRowContents[2]), "  PRESS MIDDLE KEY");
        snprintf(textRowContents[3], sizeof(textRowContents[3]), "  %d TIMES TO SEND", panicPressesRemaining);
        snprintf(textRowContents[4], sizeof(textRowContents[4]), "   PANIC MESSAGE");
        snprintf(textRowContents[5], sizeof(textRowContents[5]), "\n");
    }
    else if (currentMenu == MENU_SEIZURE_DETECT)
    {
        snprintf(textRowContents[0], sizeof(textRowContents[0]), " SEIZURE DETECT");
        snprintf(textRowContents[1], sizeof(textRowContents[1]), "\n");
        snprintf(textRowContents[2], sizeof(textRowContents[2]), " PRESS MIDDLE KEY");
        snprintf(textRowContents[3], sizeof(textRowContents[3]), " %d TIMES TO DISARM", cancelPressesRemaining);
        snprintf(textRowContents[4], sizeof(textRowContents[4]), "\n");
        int tempTime = seizureSendTime - time(NULL);
        snprintf(textRowContents[5], sizeof(textRowContents[5]), "   Countdown: %d", tempTime);
        if (tempTime <= 0)
        {
			ResetMagnitudes();
			send_state_message(2);
            ActivateMenu(MENU_SEIZURE_SENT);
            ResetMagnitudes();
            return;
        }
        else if (tempTime % 2 == 0)
        {
            vibes_short_pulse();
        }
    }
    else if (currentMenu == MENU_PANIC_SENT)
    {
        snprintf(textRowContents[0], sizeof(textRowContents[0]), "\n");
        snprintf(textRowContents[1], sizeof(textRowContents[1]), "\n");
        snprintf(textRowContents[2], sizeof(textRowContents[2]), "   PANIC MESSAGE  ");
        snprintf(textRowContents[3], sizeof(textRowContents[3]), "            SENT");
        snprintf(textRowContents[4], sizeof(textRowContents[4]), "\n");
        snprintf(textRowContents[5], sizeof(textRowContents[5]), "\n");
    }
    else if (currentMenu == MENU_SEIZURE_CANCELED)
    {
        snprintf(textRowContents[0], sizeof(textRowContents[0]), "\n");
        snprintf(textRowContents[1], sizeof(textRowContents[1]), "\n");
        snprintf(textRowContents[2], sizeof(textRowContents[2]), "  SEIZURE MESSAGE ");
        snprintf(textRowContents[3], sizeof(textRowContents[3]), "       CANCELED");
        snprintf(textRowContents[4], sizeof(textRowContents[4]), "\n");
        snprintf(textRowContents[5], sizeof(textRowContents[5]), "\n");
    }
    else if (currentMenu == MENU_SEIZURE_SENT)
    {
        snprintf(textRowContents[0], sizeof(textRowContents[0]), "\n");
        snprintf(textRowContents[1], sizeof(textRowContents[1]), "\n");
        snprintf(textRowContents[2], sizeof(textRowContents[2]), "  SEIZURE MESSAGE ");
        snprintf(textRowContents[3], sizeof(textRowContents[3]), "            SENT");
        snprintf(textRowContents[4], sizeof(textRowContents[4]), "\n");
        snprintf(textRowContents[5], sizeof(textRowContents[5]), "\n");
    }
    else if (currentMenu == MENU_BLUETOOTH_DISCONNECT)
    {
        snprintf(textRowContents[0], sizeof(textRowContents[0]), "\n");
        snprintf(textRowContents[1], sizeof(textRowContents[1]), "\n");
        snprintf(textRowContents[2], sizeof(textRowContents[2]), "       BLUETOOTH");
        snprintf(textRowContents[3], sizeof(textRowContents[3]), "  CONNECTION LOST");
        snprintf(textRowContents[4], sizeof(textRowContents[4]), "\n");
        snprintf(textRowContents[5], sizeof(textRowContents[5]), "\n");
        if (time(NULL) % 2 == 0)
        {
            vibes_short_pulse();
        }
        ResetMagnitudes();
    }
    else if (currentMenu == MENU_TEMP_DISABLE)
    {
        snprintf(textRowContents[0], sizeof(textRowContents[0]), "1 minute");
        snprintf(textRowContents[1], sizeof(textRowContents[1]), "5 minutes");
        snprintf(textRowContents[2], sizeof(textRowContents[2]), "15 minutes");
        snprintf(textRowContents[3], sizeof(textRowContents[3]), "2 hours");
        snprintf(textRowContents[4], sizeof(textRowContents[4]), "8 hours");
        snprintf(textRowContents[5], sizeof(textRowContents[5]), "CANCEL");
    }
    
    // Show the text if it has changed
    if ((currentMenu != lastMenu) || (currentMenu == MENU_SEIZURE_DETECT))
    {
        lastMenu = currentMenu;
        lastTextRenderTime = time(NULL);
        for (int i=0; i<NUM_TEXT_ROWS; i++)
        {
            text_layer_set_text(textRow[i], textRowContents[i]);
        }
    }
}

static void filterloop()
{
    for (int i=0; i < NZEROS+1; i++) xv[i] = 0;
    for (int i=0; i < NPOLES+1; i++) yv[i] = 0;
    
    for (int i=0; i < NUM_SAMPLES; i++)
    { 
        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; xv[6] = xv[7]; xv[7] = xv[8]; xv[8] = xv[9]; xv[9] = xv[10]; xv[10] = xv[11]; xv[11] = xv[12]; xv[12] = xv[13]; xv[13] = xv[14]; xv[14] = xv[15]; xv[15] = xv[16]; xv[16] = xv[17]; xv[17] = xv[18]; xv[18] = xv[19]; xv[19] = xv[20]; 
        xv[20] = storedMagnitudes[i] / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; yv[6] = yv[7]; yv[7] = yv[8]; yv[8] = yv[9]; yv[9] = yv[10]; yv[10] = yv[11]; yv[11] = yv[12]; yv[12] = yv[13]; yv[13] = yv[14]; yv[14] = yv[15]; yv[15] = yv[16]; yv[16] = yv[17]; yv[17] = yv[18]; yv[18] = yv[19]; yv[19] = yv[20]; 
        yv[20] =   (xv[0] + xv[20]) - 10 * (xv[2] + xv[18]) + 45 * (xv[4] + xv[16])
                     - 120 * (xv[6] + xv[14]) + 210 * (xv[8] + xv[12]) - 252 * xv[10]
                     + ( -0.0005328813 * yv[0]) + (  0.0039404550 * yv[1])
                     + ( -0.0223293436 * yv[2]) + (  0.0882451411 * yv[3])
                     + ( -0.2950350020 * yv[4]) + (  0.8121323526 * yv[5])
                     + ( -1.9684625437 * yv[6]) + (  4.1410982665 * yv[7])
                     + ( -7.8199248672 * yv[8]) + ( 13.1010620020 * yv[9])
                     + (-19.8821187130 * yv[10]) + ( 26.9800579970 * yv[11])
                     + (-33.2126685690 * yv[12]) + ( 36.4724513370 * yv[13])
                     + (-36.1450425680 * yv[14]) + ( 31.4665485980 * yv[15])
                     + (-24.2813478320 * yv[16]) + ( 15.8052332930 * yv[17])
                     + ( -8.7640374167 * yv[18]) + (  3.5564167957 * yv[19]);
        tempMagnitudesFiltered[i] = yv[20];        
    }
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    if ((currentMenu == MENU_DATA) || (currentMenu == MENU_PANIC_SENT) || (currentMenu == MENU_SEIZURE_CANCELED) || (currentMenu == MENU_SEIZURE_SENT))
    {
        ActivateMenu(MENU_MAIN);
        return;
    }
    else if (currentMenu == MENU_PANIC)
    {
        panicPressesRemaining--;
        lastMenu = -1; //Hack to force it to re-render text
        if (panicPressesRemaining <= 0)
        {
			send_state_message(1);
            ActivateMenu(MENU_PANIC_SENT);
        }
        return;
    }
    else if (currentMenu == MENU_SEIZURE_DETECT)
    {
        cancelPressesRemaining--;
        lastMenu = -1; //Hack to force it to re-render text
        if (cancelPressesRemaining <= 0)
        {
            ActivateMenu(MENU_SEIZURE_CANCELED);
            ResetMagnitudes();
        }
        return;
    }
    else if (currentMenu == MENU_MAIN)
    {
        if (currentSelection == 1) ActivateMenu(MENU_PANIC);
        if (currentSelection == 2) ActivateMenu(MENU_DATA);
        if (currentSelection == 3) ActivateMenu(MENU_TEMP_DISABLE);
    }
    else if (currentMenu == MENU_TEMP_DISABLE)
    {
        if (currentSelection == 0) disableUntil = time(NULL) + 60;
        if (currentSelection == 1) disableUntil = time(NULL) + 60 * 5;
        if (currentSelection == 2) disableUntil = time(NULL) + 60 * 15;
        if (currentSelection == 3) disableUntil = time(NULL) + 60 * 2 * 60;
        if (currentSelection == 4) disableUntil = time(NULL) + 60 * 8 * 60;
        if (currentSelection == 5) disableUntil = 0;
        ActivateMenu(MENU_MAIN);
        return;
    }
}

void up_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    if ((currentMenu == MENU_DATA) || (currentMenu == MENU_PANIC) || (currentMenu == MENU_PANIC_SENT))
    {
        ActivateMenu(MENU_MAIN);
        return;
    }
    
    currentSelection--;
    if (currentSelection < 0) currentSelection = NUM_TEXT_ROWS - 1;
    if ((currentMenu == MENU_MAIN) && (currentSelection == 0)) currentSelection = 3;
    SetHighlightedRow(currentSelection);
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    if ((currentMenu == MENU_DATA) || (currentMenu == MENU_PANIC) || (currentMenu == MENU_PANIC_SENT))
    {
        ActivateMenu(MENU_MAIN);
        return;
    }

    currentSelection++;
    if (currentSelection >= NUM_TEXT_ROWS) currentSelection = 0;
    if ((currentMenu == MENU_MAIN) && (currentSelection >= 4)) currentSelection = 1;
    SetHighlightedRow(currentSelection);
}

void config_provider(Window *window)
{
    // single clicks
    window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
}

void ResetMagnitudes()
{
    for (int i=0; i<NUM_SAMPLES; i++)
    {
        storedMagnitudes[i] = 0;
    }
}

// A sqrt function I got from the web that uses integers only (since Pebble emulates floating point math)
#define iter1(N) try = root + (1 << (N)); if (n >= try << (N)) { n -= try << (N); root |= 2 << (N); }
uint32_t my_sqrt(uint32_t n)
{
    uint32_t root = 0, try;
    iter1 (15); iter1 (14); iter1 (13); iter1 (12); iter1 (11);
    iter1 (10); iter1 ( 9); iter1 ( 8); iter1 ( 7); iter1 ( 6);
    iter1 ( 5); iter1 ( 4); iter1 ( 3); iter1 ( 2); iter1 ( 1); iter1 ( 0);
    return root >> 1;
}

static void data_handler(AccelData *data, uint32_t num_samples)
{
    // Add this new data to the array
    for (int i=NUM_SAMPLES-1; i>0; i--)
    {
        storedMagnitudes[i] = storedMagnitudes[i-1];
    }
    // Ignore data during vibrations of the Pebble
    if (data[0].did_vibrate)
    {
        storedMagnitudes[0] = 0;
    }
    else
    {
        // Calc the magnitude of the vector
        storedMagnitudes[0] = (my_sqrt((data[0].x * data[0].x) + (data[0].y * data[0].y) + (data[0].z * data[0].z))) - NO_SAMPLE;
    }
    
    // Apply the 1Hz to 3Hz band pass filter
    filterloop();
    
    // Take the delta and clip
    for (int i=0; i < NUM_SAMPLES-1; i++)
    {
        int tempMag = tempMagnitudesFiltered[i+1] - tempMagnitudesFiltered[i];
        if (tempMag < 0) tempMag *= -1;
        if (tempMag > SPIKE_CUTOFF) tempMag = SPIKE_CUTOFF;
        tempMagnitudesFiltered[i] = tempMag;
    }
        
    // Calculate the moving averages
    int movingAverage1Second = 0;
    for (int i=10; i<20; i++)
    {
        movingAverage1Second += tempMagnitudesFiltered[i];
    }
    movingAverage1Second = movingAverage1Second / 10;
    int movingAverage10Second = 0;
    for (int i=0; i<100; i++)
    {
        movingAverage10Second += tempMagnitudesFiltered[i];
    }
    movingAverage10Second = movingAverage10Second / 100;
    int movingAverageRelevant = 0;
    for (int i=0; i<NUM_SAMPLES; i++)
    {
        movingAverageRelevant += tempMagnitudesFiltered[i];
    }
    movingAverageRelevant = movingAverageRelevant / NUM_SAMPLES;
    
    // End disable?
    if (disableUntil > 0)
    {
        if (disableUntil <= time(NULL))
        {
            disableUntil = 0;
            lastMenu = -1; //Hack to force it to re-render text
        }
    }
	
	// Time to send location ping?
	if ((time(NULL) % 60 == 0) && (lastLocationPingSecond != time(NULL)))
	{
		lastLocationPingSecond = time(NULL);
		send_state_message(3);
	}
    
    // Re-render as clock changed?
    if (lastTextRenderTime % 60 != (time(NULL) % 60)) lastMenu = -1;
    
    // Compose string of all data
    if (currentMenu == MENU_DATA)
    {
        snprintf(textRowContents[0], sizeof(textRowContents[0]), "BANDPASS");
        snprintf(textRowContents[1], sizeof(textRowContents[1]), "%d,%d,%d", data[0].x, data[0].y, data[0].z);
        snprintf(textRowContents[2], sizeof(textRowContents[2]), "MAG: %d", storedMagnitudes[0]);
        snprintf(textRowContents[3], sizeof(textRowContents[3]), "1sec AVG: %d", movingAverage1Second);
        snprintf(textRowContents[4], sizeof(textRowContents[4]), "10sec AVG: %d", movingAverage10Second);
        snprintf(textRowContents[5], sizeof(textRowContents[5]), "%dsec AVG: %d", MAX_MOVING_AVERAGE_DURATION_IN_SECONDS, movingAverageRelevant);
        
        // Show the data
        for (int i=0; i<NUM_TEXT_ROWS; i++)
        {
            text_layer_set_text(textRow[i], textRowContents[i]);
        }
    }
    else
    {
        RenderMenu();
    }
    
    // Seizure detected?
    if ((movingAverageRelevant >= SEIZURE_DETECTION_VALUE) && (currentMenu != MENU_SEIZURE_DETECT) && (currentMenu != MENU_SEIZURE_SENT) && (disableUntil <= 0))
    {
        seizureSendTime = time(NULL) + SEIZURE_MESSAGE_DELAY_SECONDS;
        ActivateMenu(MENU_SEIZURE_DETECT);
        cancelPressesRemaining = 3;
        vibes_long_pulse();
        ResetMagnitudes();
    }
}

static void main_window_load(Window *window)
{
    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);

    // Create output TextLayer
    for (int i=0; i<NUM_TEXT_ROWS; i++)
    {
        textRow[i] = text_layer_create(GRect(5, i * (window_bounds.size.h / NUM_TEXT_ROWS), window_bounds.size.w - 10, window_bounds.size.h / NUM_TEXT_ROWS));
        text_layer_set_font(textRow[i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_text(textRow[i], "");
        text_layer_set_overflow_mode(textRow[i], GTextOverflowModeWordWrap);
        layer_add_child(window_layer, text_layer_get_layer(textRow[i]));
    }
}

static void main_window_unload(Window *window)
{
    // Destroy output TextLayers
    for (int i=0; i<NUM_TEXT_ROWS; i++)
    {
        text_layer_destroy(textRow[i]);
    }
}

void bluetooth_connection_handler(bool connected)
{
    if (!connected)
    {
        ActivateMenu(MENU_BLUETOOTH_DISCONNECT);
    }
    // Reconnected?
    else if (currentMenu == MENU_BLUETOOTH_DISCONNECT)
    {
        ActivateMenu(MENU_MAIN);
    }
}

static void send_state_message(int stateVal)
{
	DictionaryIterator *iter;
	if (app_message_outbox_begin(&iter) != APP_MSG_OK) return;
	if (dict_write_uint8(iter, 0, stateVal) != DICT_OK) return;
	app_message_outbox_send();
}

static void app_message_init(void)
{
	app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
	app_message_open(64, 16);
}

static void init()
{
    // Create main Window
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers)
    {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true);
    
    // Subscribe to button clicks
    window_set_click_config_provider(s_main_window, (ClickConfigProvider) config_provider);
    
    // Subscribe to bluetooth connection status
    bluetooth_connection_service_subscribe(bluetooth_connection_handler);

    // Init stuff
	app_message_init();
    lastMenu = -1;
    currentSelection = -1;
    disableUntil = 0;
    lastTextRenderTime = 0;
	lastLocationPingSecond = -1;
    ActivateMenu(MENU_MAIN);
    ResetMagnitudes();
    
    // Subscribe to the accelerometer data service
    int num_samples = 1;
    accel_data_service_subscribe(num_samples, data_handler);

    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
}

static void deinit()
{
    // Destroy main Window
    window_destroy(s_main_window);

    accel_data_service_unsubscribe();
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
}
