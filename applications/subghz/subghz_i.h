#pragma once

#include "subghz.h"
#include "views/receiver.h"
#include "views/transmitter.h"
#include "views/subghz_frequency_analyzer.h"
#include "views/subghz_read_raw.h"

#include "views/subghz_test_static.h"
#include "views/subghz_test_carrier.h"
#include "views/subghz_test_packet.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <dialogs/dialogs.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>

#include <subghz/scenes/subghz_scene.h>

#include <lib/subghz/subghz_worker.h>

#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>

#include "subghz_history.h"

#include <gui/modules/variable_item_list.h>

#define SUBGHZ_MAX_LEN_NAME 21

extern const char* const subghz_frequencies_text[];
extern const uint32_t subghz_frequencies[];
extern const uint32_t subghz_hopper_frequencies[];
extern const uint32_t subghz_frequencies_count;
extern const uint32_t subghz_hopper_frequencies_count;
extern const uint32_t subghz_frequencies_433_92;
extern const uint32_t subghz_frequencies_315_00;

/** SubGhzNotification state */
typedef enum {
    SubGhzNotificationStateStarting,
    SubGhzNotificationStateIDLE,
    SubGhzNotificationStateTX,
    SubGhzNotificationStateRX,
} SubGhzNotificationState;

/** SubGhzTxRx state */
typedef enum {
    SubGhzTxRxStateIDLE,
    SubGhzTxRxStateRx,
    SubGhzTxRxStateTx,
    SubGhzTxRxStateSleep,
} SubGhzTxRxState;

/** SubGhzHopperState state */
typedef enum {
    SubGhzHopperStateOFF,
    SubGhzHopperStateRunnig,
    SubGhzHopperStatePause,
    SubGhzHopperStateRSSITimeOut,
} SubGhzHopperState;

/** SubGhzRxKeyState state */
typedef enum {
    SubGhzRxKeyStateIDLE,
    SubGhzRxKeyStateNoSave,
    SubGhzRxKeyStateNeedSave,
    SubGhzRxKeyStateBack,
    SubGhzRxKeyStateAddKey,
    SubGhzRxKeyStateExit,
    SubGhzRxKeyStateRAWLoad,
    SubGhzRxKeyStateRAWSave,
} SubGhzRxKeyState;

/** SubGhzLoadKeyState state */
typedef enum {
    SubGhzLoadKeyStateUnknown,
    SubGhzLoadKeyStateOK,
    SubGhzLoadKeyStateParseErr,
    SubGhzLoadKeyStateOnlyRx,
} SubGhzLoadKeyState;

struct SubGhzTxRx {
    SubGhzWorker* worker;

    SubGhzEnvironment* environment;
    SubGhzReceiver* receiver;
    SubGhzTransmitter* transmitter;
    SubGhzProtocolDecoderBase* decoder_result;
    FlipperFormat* fff_data;

    uint32_t frequency;
    FuriHalSubGhzPreset preset;
    SubGhzHistory* history;
    uint16_t idx_menu_chosen;
    SubGhzTxRxState txrx_state;
    SubGhzHopperState hopper_state;
    uint8_t hopper_timeout;
    uint8_t hopper_idx_frequency;
    SubGhzRxKeyState rx_key_state;
};

typedef struct SubGhzTxRx SubGhzTxRx;

struct SubGhz {
    Gui* gui;
    NotificationApp* notifications;

    SubGhzTxRx* txrx;

    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    Submenu* submenu;
    Popup* popup;
    TextInput* text_input;
    Widget* widget;
    DialogsApp* dialogs;
    char file_name[SUBGHZ_MAX_LEN_NAME + 1];
    char file_name_tmp[SUBGHZ_MAX_LEN_NAME + 1];
    SubGhzNotificationState state_notifications;

    SubGhzViewReceiver* subghz_receiver;
    SubGhzViewTransmitter* subghz_transmitter;
    VariableItemList* variable_item_list;

    SubGhzFrequencyAnalyzer* subghz_frequency_analyzer;
    SubGhzReadRAW* subghz_read_raw;
    SubGhzTestStatic* subghz_test_static;
    SubGhzTestCarrier* subghz_test_carrier;
    SubGhzTestPacket* subghz_test_packet;
    string_t error_str;
};

typedef enum {
    SubGhzViewIdMenu,
    SubGhzViewIdReceiver,
    SubGhzViewIdPopup,
    SubGhzViewIdTextInput,
    SubGhzViewIdWidget,
    SubGhzViewIdTransmitter,
    SubGhzViewIdVariableItemList,
    SubGhzViewIdFrequencyAnalyzer,
    SubGhzViewIdReadRAW,

    SubGhzViewIdStatic,
    SubGhzViewIdTestCarrier,
    SubGhzViewIdTestPacket,
} SubGhzViewId;

bool subghz_set_preset(SubGhz* subghz, const char* preset);
void subghz_get_frequency_modulation(SubGhz* subghz, string_t frequency, string_t modulation);
void subghz_begin(SubGhz* subghz, FuriHalSubGhzPreset preset);
uint32_t subghz_rx(SubGhz* subghz, uint32_t frequency);
void subghz_rx_end(SubGhz* subghz);
void subghz_sleep(SubGhz* subghz);
bool subghz_tx_start(SubGhz* subghz, FlipperFormat* flipper_format);
void subghz_tx_stop(SubGhz* subghz);
void subghz_dialog_message_show_only_rx(SubGhz* subghz);
bool subghz_key_load(SubGhz* subghz, const char* file_path);
bool subghz_get_next_name_file(SubGhz* subghz, uint8_t max_len);
bool subghz_save_protocol_to_file(
    SubGhz* subghz,
    FlipperFormat* flipper_format,
    const char* dev_name);
bool subghz_load_protocol_from_file(SubGhz* subghz);
bool subghz_rename_file(SubGhz* subghz);
bool subghz_delete_file(SubGhz* subghz);
void subghz_file_name_clear(SubGhz* subghz);
uint32_t subghz_random_serial(void);
void subghz_hopper_update(SubGhz* subghz);
