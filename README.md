# Projeto de Controle de LEDs RGB e Display OLED com Joystick

Este projeto utiliza o Raspberry Pi Pico (RP2040) com a biblioteca SDK da Raspberry Pi para controlar LEDs RGB via PWM, um display OLED SSD1306 e um joystick. O código permite a movimentação de um quadrado no display OLED, o controle da intensidade dos LEDs com base na posição do joystick, além de interações com botões para alternar o estilo da borda do display e o estado dos LEDs PWM.

## Componentes Utilizados

- **Raspberry Pi Pico (RP2040)**: Microcontrolador utilizado para o controle de GPIOs e PWM.
- **Joystick Analógico**: Controla a posição de um quadrado no display OLED e ajusta a intensidade dos LEDs RGB.
- **LEDs RGB**: Controlados via PWM para variar sua intensidade de acordo com o movimento do joystick.
- **Display OLED SSD1306**: Exibe um quadrado que se move conforme o joystick e permite alternar o estilo da borda.
- **Botões**: Usados para alternar o estado de LEDs e borda do display.

## Funcionalidades

### 1. Controle de LEDs RGB com Joystick
- O LED **vermelho** é controlado pelo eixo **X** do joystick.
- O LED **azul** é controlado pelo eixo **Y** do joystick.
- A intensidade dos LEDs varia de acordo com a posição do joystick.

### 2. Movimento do Quadrado no Display OLED
- Um quadrado de 8x8 pixels se move com base na posição do joystick, sendo desenhado no display OLED SSD1306.

### 3. Interações com Botões
- **Botão do Joystick** (GPIO 22):
  - Alterna o estado do **LED verde** (liga/desliga).
  - Alterna o estilo da borda do display entre uma borda retangular normal e uma borda em zig-zag.
  
- **Botão A** (GPIO 5):
  - Alterna o estado dos LEDs PWM (liga/desliga).

## Como Funciona

1. O código configura as entradas e saídas necessárias, incluindo os pinos dos LEDs, joystick e botões.
2. O valor dos eixos **X** e **Y** do joystick é lido por ADCs e usado para controlar a intensidade dos LEDs RGB.
3. Um quadrado é desenhado no display OLED, com a posição ajustada de acordo com os valores lidos do joystick.
4. O código implementa debouncing para os botões, garantindo que as ações sejam registradas corretamente.
5. O estilo da borda do display e o estado do PWM podem ser alternados ao pressionar os botões.

## Configurações de Hardware

- **GPIO 11**: LED verde (PWM)
- **GPIO 12**: LED azul (PWM)
- **GPIO 13**: LED vermelho (PWM)
- **GPIO 22**: Botão do joystick (interrupção)
- **GPIO 5**: Botão A (interrupção)
- **GPIO 26**: Eixo X do joystick (entrada analógica)
- **GPIO 27**: Eixo Y do joystick (entrada analógica)
- **GPIOs 14 e 15**: Comunicação I2C com o display OLED SSD1306

## Bibliotecas Utilizadas

- **pico/stdlib.h**: Funções padrão de entrada/saída.
- **hardware/gpio.h**: Controle de GPIOs.
- **hardware/pwm.h**: Controle de PWM para modulação de intensidade dos LEDs.
- **ssd1306.h**: Controle do display OLED SSD1306 via I2C.
- **hardware/adc.h**: Leitura de entradas analógicas para o joystick.

## Instruções para Compilação e Execução

1. **Compilar o Código**:
   - Use o CMake para compilar o código. Inclua a biblioteca SDK do Raspberry Pi Pico.

2. **Carregar o Código no Pico**:
   - Carregue o código compilado no seu Raspberry Pi Pico utilizando o método de carregamento via USB.

3. **Verificar a Saída**:
   - Use o terminal serial para depuração. As informações de estado dos LEDs e o estilo da borda serão exibidas conforme as interações com os botões.

---

Sinta-se à vontade para modificar e adaptar o código conforme necessário!


[Clique aqui ⬆️ e assista o vídeo](https://youtu.be/0COPwlGtTs4)


## Autor
Desenvolvido por [Renan Ferreira].

