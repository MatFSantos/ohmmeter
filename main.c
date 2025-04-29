#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"

#include "lib/push_button.h"
#include "lib/ssd1306.h"

#define ADC_PIN 28
#define ADC_RESOLUTION 4095
#define R_KNOWN 9930 // valor real (medido com multímetro)

const float ADC_VREF = 3.3f;
static volatile bool view = true;
static volatile uint32_t last_time = 0;

ssd1306_t ssd;

/**
 * @brief Initialize the SSD1306 display
 *
 */
void init_display() {
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, I2C_ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
}

/**
 * @brief Initialize the all GPIOs that will be used in project
 *      - I2C;
 *      - Push button A
 */
void init_gpio() {
    /** initialize i2c communication */
    int baudrate = 400 * 1000; // 400kHz baud rate for i2c communication
    i2c_init(I2C_PORT, baudrate);

    // set GPIO pin function to I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);

    /** initialize buttons */
    init_push_button(PIN_BUTTON_A);
}

/**
 * @brief function responsible for encoding digits in colors
 *
 * @param
 *      digit - digit from 0 to 9
 *
 * @return
 *      string representing a color
 */
char *color_digit(int digit) {
    char *colors[] = {
        "Pr", "Mr", "Vm", "Lr", "Am",
        "Vr", "Az", "Vl", "Cz", "Bn"};
    if (digit >= 0 && digit <= 9)
        return colors[digit];
    else
        return "";
}

/**
 * @brief Adjusts the passed resistance value to the nearest commercial
 * E24 series resistor value.
 *
 * @param
 *      resistance -> resistance that will be adjusted.
 * 
 * @return
 *      adjusted resistance
 */
float find_nearest_e24(float resistance) {
    if (resistance <= 0)
        return 0;

    const float E24_values[] = {
        1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0,
        2.2, 2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.3,
        4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1, 10.0}; // Valores da série E24 normalizados (de 1 a 10, para fácil comparação)

    int order = (int)floor(log10(resistance));
    float normalized = resistance / pow(10, order); // normaliza para 1~10

    // buscar o valor mais próximo da E24 normalizada
    float min_diff = 1e9;
    float best_match = E24_values[0];
    for (int i = 0; i < sizeof(E24_values) / sizeof(E24_values[0]); i++) {
        float diff = fabs(normalized - E24_values[i]);
        if (diff < min_diff) {
            min_diff = diff;
            best_match = E24_values[i];
        }
    }

    // reconstruir o valor
    float adjusted = best_match * pow(10, order);
    return adjusted;
}

/**
 * @brief From the resistance value, assemble the color code for it, from the first
 * to the third band
 *
 * @param
 *      resistance -> numerical resistance that will be converted into colors
 *
 * @return
 *      array containing the strings with the colors, from the first to the third
 */
const char **print_resistor_colors(float resistance) {
    static const char *result[3] = {"", "", ""};

    if (resistance < 1) {
        printf("Resistencia muito baixa para detectar cores.\n");
        return result;
    }

    // aproxima o valor para o resistor da série E24 mais próximo
    float adjusted = find_nearest_e24(resistance);
    int order = (int)floor(log10(adjusted)); // ordem do número

    int first_digit = (int)(adjusted / pow(10, order)) % 10; // primeiro digito
    int second_digit = (int)(adjusted / pow(10, order - 1)) % 10; // segundo digito

    // cores correspondentes aos digitos
    result[0] = color_digit(first_digit);
    result[1] = color_digit(second_digit);
    result[2] = color_digit(order);

    printf("Resistencia medida: %.1f Ohms, ajustada para: %.1f Ohms\n", resistance, adjusted);

    return result;
}

/**
 * @brief Update the display informations
 */
void update_display(float digital, float analog) {
    char adc_value[5]; // Buffer para armazenar a string
    char r[5];         // Buffer para armazenar a string
    sprintf(adc_value, "%1.0f", digital);
    sprintf(r, "%1.0f", analog);

    printf("Valor digital: %s\n", adc_value);
    printf("Resistencia: %s\n", r);

    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 0, 0, 128, 64, true, false);
    if (view) {
        ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 6);
        ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 16);
        ssd1306_line(&ssd, 3, 25, 123, 25, true);
        ssd1306_draw_string(&ssd, "ADC", 13, 28);
        ssd1306_draw_string(&ssd, adc_value, 8, 39);
        ssd1306_line(&ssd, 44, 25, 44, 51, true);
        ssd1306_draw_string(&ssd, "Resisten.", 50, 28);
        ssd1306_draw_string(&ssd, r, 59, 39);
        ssd1306_line(&ssd, 3, 51, 123, 51, true);
        ssd1306_draw_string(&ssd, "BTN A - cores", 8, 54);
    } else {
        const char ** cod_colors = print_resistor_colors(analog);
        ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 6);
        ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 16);
        ssd1306_line(&ssd, 23, 25, 103, 25, true); // horizontal maior
        ssd1306_line(&ssd, 23, 45, 103, 45, true); // horizontal maior
        ssd1306_line(&ssd, 3, 35, 23, 35, true);   // horizontal menor
        ssd1306_line(&ssd, 103, 35, 123, 35, true); // horizontal menor
        ssd1306_line(&ssd, 23, 25, 23, 45, true); // vertical 1
        ssd1306_line(&ssd, 103, 25, 103, 45, true); // vertical 2
        ssd1306_draw_string(&ssd, cod_colors[0], 25, 31);
        ssd1306_draw_string(&ssd, cod_colors[1], 54, 31);
        ssd1306_draw_string(&ssd, cod_colors[2], 87, 31);
        ssd1306_draw_string(&ssd, "BTN A - valor", 8, 54);
    }
    ssd1306_send_data(&ssd); // update display
}


/**
 * @brief Handler function to interruption
 *
 * @param gpio GPIO that triggered the interruption
 * @param event The event which caused the interruption
 */
void gpio_irq_handler(uint gpio, uint32_t event) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_time >= 300) { // debounce
        view = !view;
        last_time = current_time;
    }
}

int main() {
    float R_X = 0.0f;

    // set clock freq to 128MHz
    bool ok = set_sys_clock_khz(128000, false);
    if (ok)
        printf("clock set to %ld\n", clock_get_hz(clk_sys));

    // init gpios and stdio functions
    stdio_init_all();
    init_gpio();

    // get ws and ssd struct
    init_display();

    // Clear display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // init adc channels
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(2);

    // configure interruptions handlers
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);

    sleep_ms(50);
    printf("Programm start...\n");
    while (1) {
        float sum = 0.0f;
        for (int i = 0; i < 500; i++) {
            sum += adc_read();
            sleep_ms(1);
        }
        float avr = sum / 500.0f; // faz média dos valores capturados em 500ms
        float v = avr * 3.3f / 4095.0f; // converte para tensão

        if (v > 0.0f && v < 3.3f) {
            R_X = (v * R_KNOWN)/ (3.3f - v);
        } else {
            printf("Valor fora da faixa\n");
            R_X = 0.0f;
        }

        update_display(avr, R_X);
        sleep_ms(100);
    }

    return 0;
}