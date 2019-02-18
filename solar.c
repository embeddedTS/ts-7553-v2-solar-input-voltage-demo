#include <cairo/cairo.h>
#include <dirent.h> 
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> 

#include "lcd-display.h"
#include "tsmicroctl.h"

static lcdInfo_t *lcd;

void clear_screen(cairo_t* cr)
{
    // Clear the screen first
    cairo_save (cr);
    cairo_set_source_rgba (cr, 0, 0, 0, 0);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint (cr);
    cairo_restore (cr);
}

// Scale voltage to silabs 0-2.5V
uint16_t inline sscale(uint16_t data){
	return data * (2.5/1023) * 1000;
}

// Scale voltage for resistor dividers
uint16_t inline rscale(uint16_t data, uint16_t r1, uint16_t r2)
{
	uint16_t ret = (data * (r1 + r2)/r2);
	return sscale(ret);
}

int read_vin(twifd) 
{
    uint8_t data[32];

    bzero(data, 32);
    read(twifd, data, 32);

    return rscale((data[6]<<8|data[7]), 1910, 172);
}

int read_supercap_pct(twifd) 
{
    uint8_t data[32];
    unsigned int pct;

    bzero(data, 32);
    read(twifd, data, 32);

    pct = (((data[2]<<8|data[3])*100/237));
    if (pct > 311) {
    pct = pct - 311;
    	if (pct > 100) pct = 100;
    } else {
    	pct = 0;
    }

    return pct > 100 ? 100 : pct;
}

int main(void)
{
    int twifd;
    cairo_t *cr;
    cairo_surface_t *sfc;
    char output[32];

    int vin;
    int current_vin;

    int supercap_pct;
    int current_supercap_pct;

    twifd = silabs_init();
    if(twifd == -1)
      return 1;

    if ((lcd = openDisplay()) == NULL) {
        fprintf(stderr, "ERROR: Can't open display\n");
        return 1;
    }

    sfc = cairo_image_surface_create_for_data((unsigned char *)lcd->frameBuffer, 
        CAIRO_FORMAT_A1, lcd->displayWidth, lcd->displayHeight, lcd->stride);

    if (sfc) {                        
        if ((cr = cairo_create(sfc))) {
            cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
            cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                CAIRO_FONT_WEIGHT_BOLD);

	    vin = 0;
            current_vin = 0;

	    supercap_pct = 0;
            current_supercap_pct = 0;

            supercap_pct = read_supercap_pct(twifd);
 

            while(1) {
                vin = read_vin(twifd);
                supercap_pct = read_supercap_pct(twifd);

                if (vin != current_vin || 
                    supercap_pct != current_supercap_pct) {

                    clear_screen(cr);

                    cairo_set_font_size(cr, 14.0);
                    cairo_move_to(cr, 10, 25);
                    snprintf(output, sizeof(output), "Vin:");
                    cairo_show_text(cr, output);

                    cairo_set_font_size(cr, 22.0);
                    cairo_move_to(cr, 10, 45);
                    snprintf(output, sizeof(output), "%.1f VDC", (float)vin/1000);
                    cairo_show_text(cr, output);

                    cairo_set_font_size(cr, 10.0);
                    cairo_move_to(cr, lcd->displayWidth - 125, lcd->displayHeight - 5);
                    snprintf(output, sizeof(output), "TS-SILO Charge: %d%%", supercap_pct);
                    cairo_show_text(cr, output);

                    current_vin = vin;
                    current_supercap_pct = supercap_pct;

                    cairo_surface_flush(sfc);
                }

                sleep(.1);
            }

        }
    }

    return 0;
}
