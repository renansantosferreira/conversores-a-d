#include <stdio.h>           // Biblioteca padrão de entrada e saída, utilizada para funções como printf() para depuração via serial.
#include <stdint.h>          // Biblioteca que define tipos inteiros com tamanho fixo (ex: uint8_t, uint16_t, uint32_t) para garantir portabilidade e controle de tamanho de dados.
#include "pico/stdlib.h"     // Biblioteca da SDK do Raspberry Pi Pico, fornecendo funções como stdio_init_all() e sleep_ms(), além de manipulação básica de GPIO.
#include "hardware/gpio.h"   // Biblioteca da SDK do Raspberry Pi Pico para controle de GPIOs (entradas e saídas), com funções como gpio_init(), gpio_set_dir(), gpio_put(), etc.
#include "hardware/pwm.h"    // Biblioteca da SDK do Raspberry Pi Pico para controle de PWM (modulação por largura de pulso), útil para controlar a intensidade de LEDs, por exemplo.
#include "ssd1306.h"         // Biblioteca para controlar displays OLED SSD1306 via I2C, com funções para inicialização, desenho de pixels, textos e gráficos no display.
#include "hardware/adc.h"  // Biblioteca para ADC

#define BUTTON_JOY_PIN 22  // Pino do botão do joystick
#define BUTTON_A_PIN 5     // Pino do botão A
#define LED_GREEN_PIN 11   // Pino do LED verde
#define LED_RED_PIN 13     // Pino do LED vermelho
#define LED_BLUE_PIN 12    // Pino do LED azul
#define JOY_X_PIN 26       // Pino do eixo X do joystick
#define JOY_Y_PIN 27       // Pino do eixo Y do joystick

#define ADC_WIDTH 12       // Resolução do ADC (12 bits: 0 a 4095)
#define JOY_CENTER 2048   // Valor central do joystick
#define DEBOUNCE_DELAY 200 // Delay para debouncing (em milissegundos)
#define DEAD_ZONE 180     // Zona morta para evitar acionamento involuntário

ssd1306_t disp;          // Display OLED
volatile bool led_pwm_state = true; // Estado dos LEDs PWM (ativo ou não)
volatile bool display_border_style = true; // Alternar estilo da borda do display
volatile bool led_green_state = false;  // Estado do LED verde
volatile uint32_t last_interrupt_time = 0; // Variável para controle de debouncing

uint32_t pwm_wrap = 4095; // Resolução do PWM (0-4095)

// Função para configurar PWM para o LED
void pwm_setup(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM); // Configura o pino para PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Obtém o número do slice de PWM
    pwm_set_wrap(slice_num, pwm_wrap); // Define o valor de wrap para o PWM
    pwm_set_enabled(slice_num, true); // Habilita o PWM
}

// Função para ajustar a intensidade do LED RGB via PWM
void update_led_pwm(uint16_t joy_x, uint16_t joy_y) {
    // Ajusta o brilho do LED vermelho com base no eixo X do joystick
    uint16_t red_brightness = (abs(joy_x - JOY_CENTER) > DEAD_ZONE) ? (abs(joy_x - JOY_CENTER) * 4095 / (JOY_CENTER)) : 0;
    // Ajusta o brilho do LED azul com base no eixo Y do joystick
    uint16_t blue_brightness = (abs(joy_y - JOY_CENTER) > DEAD_ZONE) ? (abs(joy_y - JOY_CENTER) * 4095 / (JOY_CENTER)) : 0;

    pwm_set_gpio_level(LED_BLUE_PIN, red_brightness);  // Controla o LED vermelho pelo eixo X
    pwm_set_gpio_level(LED_RED_PIN, blue_brightness); // Controla o LED azul pelo eixo Y
}

// Função para desenhar o quadrado que se move com o joystick
void draw_square(int x, int y) {
    ssd1306_clear(&disp);  // Limpa o display
    ssd1306_fill_rect(&disp, x, y, 8, 8, 1);  // Desenha um quadrado de 8x8
    ssd1306_show(&disp);  // Exibe o conteúdo no display
}

// Função de interrupção para o botão do joystick
void gpio_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());  // Obtém o tempo atual em milissegundos
    
    // Verifica se o tempo desde a última interrupção é superior ao delay de debounce
    if (current_time - last_interrupt_time > DEBOUNCE_DELAY) {
        last_interrupt_time = current_time;

        if (gpio == BUTTON_JOY_PIN) {  // Se o botão do joystick foi pressionado
            led_green_state = !led_green_state;  // Alterna o estado do LED verde
            gpio_put(LED_GREEN_PIN, led_green_state);  // Atualiza o LED verde
            printf("LED Verde: %s\n", led_green_state ? "Ligado" : "Desligado");

            display_border_style = !display_border_style;  // Alterna o estilo da borda do display
            ssd1306_clear(&disp);  // Limpa o display
            
            // Estilo de borda alternado
            if (display_border_style) {  
                // Borda retangular normal
                ssd1306_draw_rect(&disp, 0, 0, 128, 64, 1);
            } else {  
                // Borda em zig-zag
                for (int i = 0; i < 128; i += 16) {
                    ssd1306_draw_line(&disp, i, 0, 128 - i, 64, 1); // Desenha linhas diagonais
                }
            }

            ssd1306_show(&disp);  // Exibe o conteúdo no display
        } else if (gpio == BUTTON_A_PIN) {  // Se o botão A foi pressionado
            led_pwm_state = !led_pwm_state;  // Alterna o estado dos LEDs PWM
            printf("PWM LEDs: %s\n", led_pwm_state ? "Ativado" : "Desativado");
            if (!led_pwm_state) {  // Se o PWM estiver desativado, desliga os LEDs
                pwm_set_gpio_level(LED_RED_PIN, 0);
                pwm_set_gpio_level(LED_BLUE_PIN, 0);
            }
        }
    }
}

// Função para configurar o ADC para o joystick
void adc_setup() {
    adc_gpio_init(JOY_X_PIN);  // Inicializa o pino do eixo X do joystick para ADC
    adc_gpio_init(JOY_Y_PIN);  // Inicializa o pino do eixo Y do joystick para ADC
    adc_init();  // Inicializa o ADC
}

int main() {
    stdio_init_all();  // Inicializa a comunicação serial
    i2c_init(i2c1, 400000);  // Inicializa o I2C com velocidade de 400kHz
    gpio_set_function(14, GPIO_FUNC_I2C);  // Configura o pino 14 para I2C
    gpio_set_function(15, GPIO_FUNC_I2C);  // Configura o pino 15 para I2C
    gpio_pull_up(14);  // Habilita o pull-up no pino 14
    gpio_pull_up(15);  // Habilita o pull-up no pino 15
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);  // Inicializa o display OLED
    ssd1306_clear(&disp);  // Limpa o display
    ssd1306_show(&disp);  // Exibe o conteúdo no display

    gpio_init(LED_GREEN_PIN);  // Inicializa o pino do LED verde
    gpio_init(LED_RED_PIN);    // Inicializa o pino do LED vermelho
    gpio_init(LED_BLUE_PIN);   // Inicializa o pino do LED azul
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);  // Configura o pino do LED verde como saída
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);    // Configura o pino do LED vermelho como saída
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);   // Configura o pino do LED azul como saída

    gpio_init(BUTTON_JOY_PIN);  // Inicializa o pino do botão do joystick
    gpio_init(BUTTON_A_PIN);    // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_JOY_PIN, GPIO_IN);  // Configura o pino do botão do joystick como entrada
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);    // Configura o pino do botão A como entrada
    gpio_pull_up(BUTTON_JOY_PIN);  // Habilita o pull-up no pino do botão do joystick
    gpio_pull_up(BUTTON_A_PIN);    // Habilita o pull-up no pino do botão A
    gpio_set_irq_enabled_with_callback(BUTTON_JOY_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);  // Configura interrupção para o botão do joystick
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);    // Configura interrupção para o botão A

    pwm_setup(LED_RED_PIN);  // Configura o PWM para o LED vermelho
    pwm_setup(LED_BLUE_PIN); // Configura o PWM para o LED azul

    adc_setup();  // Configura o ADC

    while (1) {
        adc_select_input(0);  // Seleciona o canal do eixo X do joystick
        uint16_t joy_x = adc_read();  // Lê o valor do eixo X
        adc_select_input(1);  // Seleciona o canal do eixo Y do joystick
        uint16_t joy_y = adc_read();  // Lê o valor do eixo Y

        if (led_pwm_state) {  // Se o PWM estiver ativado, ajusta os LEDs
            update_led_pwm(joy_x, joy_y);
        }

        // Calcula a posição do quadrado com base nos valores do joystick
        int x_pos = (joy_y - JOY_CENTER) / 16 + 64; // Corrige inversão do eixo Y
        int y_pos = (JOY_CENTER - joy_x) / 16 + 32; // Mantém o eixo X correto
        
        draw_square(x_pos, y_pos);  // Desenha o quadrado no display
        sleep_ms(20);  // Aguarda 20ms antes da próxima leitura
    }
    return 0;
}
