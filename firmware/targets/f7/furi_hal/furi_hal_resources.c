#include <furi_hal_resources.h>
#include <furi.h>

const GpioPin vibro_gpio = {.port = VIBRO_GPIO_Port, .pin = VIBRO_Pin};
const GpioPin ibutton_gpio = {.port = iBTN_GPIO_Port, .pin = iBTN_Pin};

const GpioPin gpio_cc1101_g0 = {.port = CC1101_G0_GPIO_Port, .pin = CC1101_G0_Pin};
const GpioPin gpio_rf_sw_0 = {.port = RF_SW_0_GPIO_Port, .pin = RF_SW_0_Pin};

const GpioPin gpio_subghz_cs = {.port = CC1101_CS_GPIO_Port, .pin = CC1101_CS_Pin};
const GpioPin gpio_display_cs = {.port = DISPLAY_CS_GPIO_Port, .pin = DISPLAY_CS_Pin};
const GpioPin gpio_display_rst = {.port = DISPLAY_RST_GPIO_Port, .pin = DISPLAY_RST_Pin};
const GpioPin gpio_display_di = {.port = DISPLAY_DI_GPIO_Port, .pin = DISPLAY_DI_Pin};
const GpioPin gpio_sdcard_cs = {.port = SD_CS_GPIO_Port, .pin = SD_CS_Pin};
const GpioPin gpio_sdcard_cd = {.port = SD_CD_GPIO_Port, .pin = SD_CD_Pin};
const GpioPin gpio_nfc_cs = {.port = NFC_CS_GPIO_Port, .pin = NFC_CS_Pin};

const GpioPin gpio_button_up = {.port = GPIOB, .pin = LL_GPIO_PIN_10};
const GpioPin gpio_button_down = {.port = GPIOC, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_button_right = {.port = GPIOB, .pin = LL_GPIO_PIN_12};
const GpioPin gpio_button_left = {.port = GPIOB, .pin = LL_GPIO_PIN_11};
const GpioPin gpio_button_ok = {.port = GPIOH, .pin = LL_GPIO_PIN_3};
const GpioPin gpio_button_back = {.port = GPIOC, .pin = LL_GPIO_PIN_13};

const GpioPin gpio_spi_d_miso = {.port = SPI_D_MISO_GPIO_Port, .pin = SPI_D_MISO_Pin};
const GpioPin gpio_spi_d_mosi = {.port = SPI_D_MOSI_GPIO_Port, .pin = SPI_D_MOSI_Pin};
const GpioPin gpio_spi_d_sck = {.port = SPI_D_SCK_GPIO_Port, .pin = SPI_D_SCK_Pin};
const GpioPin gpio_spi_r_miso = {.port = SPI_R_MISO_GPIO_Port, .pin = SPI_R_MISO_Pin};
const GpioPin gpio_spi_r_mosi = {.port = SPI_R_MOSI_GPIO_Port, .pin = SPI_R_MOSI_Pin};
const GpioPin gpio_spi_r_sck = {.port = SPI_R_SCK_GPIO_Port, .pin = SPI_R_SCK_Pin};

const GpioPin gpio_ext_pc0 = {.port = GPIOC, .pin = LL_GPIO_PIN_0};
const GpioPin gpio_ext_pc1 = {.port = GPIOC, .pin = LL_GPIO_PIN_1};
const GpioPin gpio_ext_pc3 = {.port = GPIOC, .pin = LL_GPIO_PIN_3};
const GpioPin gpio_ext_pb2 = {.port = GPIOB, .pin = LL_GPIO_PIN_2};
const GpioPin gpio_ext_pb3 = {.port = GPIOB, .pin = LL_GPIO_PIN_3};
const GpioPin gpio_ext_pa4 = {.port = GPIOA, .pin = LL_GPIO_PIN_4};
const GpioPin gpio_ext_pa6 = {.port = GPIOA, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_ext_pa7 = {.port = GPIOA, .pin = LL_GPIO_PIN_7};

const GpioPin gpio_rfid_pull = {.port = RFID_PULL_GPIO_Port, .pin = RFID_PULL_Pin};
const GpioPin gpio_rfid_carrier_out = {.port = RFID_OUT_GPIO_Port, .pin = RFID_OUT_Pin};
const GpioPin gpio_rfid_data_in = {.port = RFID_RF_IN_GPIO_Port, .pin = RFID_RF_IN_Pin};
const GpioPin gpio_rfid_carrier = {.port = RFID_CARRIER_GPIO_Port, .pin = RFID_CARRIER_Pin};

const GpioPin gpio_infrared_rx = {.port = IR_RX_GPIO_Port, .pin = IR_RX_Pin};
const GpioPin gpio_infrared_tx = {.port = IR_TX_GPIO_Port, .pin = IR_TX_Pin};

const GpioPin gpio_usart_tx = {.port = USART1_TX_Port, .pin = USART1_TX_Pin};
const GpioPin gpio_usart_rx = {.port = USART1_RX_Port, .pin = USART1_RX_Pin};

const GpioPin gpio_i2c_power_sda = {.port = GPIOA, .pin = LL_GPIO_PIN_10};
const GpioPin gpio_i2c_power_scl = {.port = GPIOA, .pin = LL_GPIO_PIN_9};

const GpioPin gpio_speaker = {.port = GPIOB, .pin = LL_GPIO_PIN_8};

const GpioPin periph_power = {.port = PERIPH_POWER_GPIO_Port, .pin = PERIPH_POWER_Pin};

const InputPin input_pins[] = {
    {.gpio = &gpio_button_up, .key = InputKeyUp, .inverted = true, .name = "Up"},
    {.gpio = &gpio_button_down, .key = InputKeyDown, .inverted = true, .name = "Down"},
    {.gpio = &gpio_button_right, .key = InputKeyRight, .inverted = true, .name = "Right"},
    {.gpio = &gpio_button_left, .key = InputKeyLeft, .inverted = true, .name = "Left"},
    {.gpio = &gpio_button_ok, .key = InputKeyOk, .inverted = false, .name = "Ok"},
    {.gpio = &gpio_button_back, .key = InputKeyBack, .inverted = true, .name = "Back"},
};

const size_t input_pins_count = sizeof(input_pins) / sizeof(InputPin);

void furi_hal_resources_init(void) {
    // Button pins
    for(size_t i = 0; i < input_pins_count; i++) {
        furi_hal_gpio_init(
            input_pins[i].gpio, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedLow);
    }

    // External header pins
    furi_hal_gpio_init(&gpio_ext_pc0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pc1, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pc3, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pb2, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pb3, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pa4, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pa7, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    // Display pins
    furi_hal_gpio_init(&gpio_display_rst, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_display_rst, 0);

    furi_hal_gpio_init(&gpio_display_di, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_display_di, 0);

    // SD pins
    furi_hal_gpio_init(&gpio_sdcard_cd, GpioModeInput, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_sdcard_cd, 0);

    furi_hal_gpio_init(&vibro_gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&ibutton_gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&gpio_rfid_pull, GpioModeInterruptRise, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&gpio_rf_sw_0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&periph_power, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);

    NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI0_IRQn);

    NVIC_SetPriority(EXTI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI1_IRQn);

    NVIC_SetPriority(EXTI2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI2_IRQn);

    NVIC_SetPriority(EXTI3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI3_IRQn);

    NVIC_SetPriority(EXTI4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI4_IRQn);

    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}
