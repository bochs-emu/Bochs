

#define USB_DEBUG_UHCI_DLG      3900
#define USB_DEBUG_OHCI_DLG      3901
#define USB_DEBUG_EHCI_DLG      3902
#define USB_DEBUG_XHCI_DLG      3903

#define USB_DEBUG_UHCI_DLG_QUEUE           4000
#define USB_DEBUG_UHCI_DLG_TD              4001

#define USB_DEBUG_DLG_DUMP                 4010


#define USB_DEBUG_XHCI_DLG_NORM_TRB        4104
#define USB_DEBUG_XHCI_DLG_SETUP_TRB       4105
#define USB_DEBUG_XHCI_DLG_DATA_TRB        4106
#define USB_DEBUG_XHCI_DLG_STATUS_TRB      4107
#define USB_DEBUG_XHCI_DLG_LINK_TRB        4108
#define USB_DEBUG_XHCI_DLG_EVENT_TRB       4109
#define USB_DEBUG_XHCI_DLG_NOOP_TRB        4110
#define USB_DEBUG_XHCI_DLG_ENSLOT_TRB      4111
#define USB_DEBUG_XHCI_DLG_DISSLOT_TRB     4112
#define USB_DEBUG_XHCI_DLG_ADDRESS_TRB     4113
#define USB_DEBUG_XHCI_DLG_CONFIGEP_TRB    4114
#define USB_DEBUG_XHCI_DLG_EVALUATE_TRB    4115
#define USB_DEBUG_XHCI_DLG_RESETEP_TRB     4116
#define USB_DEBUG_XHCI_DLG_STOPEP_TRB      4117
#define USB_DEBUG_XHCI_DLG_SETTRPTR_TRB    4118
#define USB_DEBUG_XHCI_DLG_RESETDEV_TRB    4119
#define USB_DEBUG_XHCI_DLG_FORCE_TRB       4120
#define USB_DEBUG_XHCI_DLG_SETLAT_TRB      4121
#define USB_DEBUG_XHCI_DLG_GETBAND_TRB     4122
#define USB_DEBUG_XHCI_DLG_FORCEHDR_TRB    4123
#define USB_DEBUG_XHCI_DLG_TRANSEVENT_TRB  4124
#define USB_DEBUG_XHCI_DLG_CMNDCOMP_TRB    4125
#define USB_DEBUG_XHCI_DLG_PSCHANGE_TRB    4126
#define USB_DEBUG_XHCI_DLG_BANDREQU_TRB    4127
#define USB_DEBUG_XHCI_DLG_DOORBELL_TRB    4128
#define USB_DEBUG_XHCI_DLG_HOSTEVENT_TRB   4129
#define USB_DEBUG_XHCI_DLG_DEVNOTEVENT_TRB 4130

#define USB_DEBUG_XHCI_DLG_NECFW_TRB       4140
#define USB_DEBUG_XHCI_DLG_NECUN_TRB       4141
#define USB_DEBUG_XHCI_DLG_NECFWEVENT_TRB  4142

#define IDD_ATTRIBUTE                      4150
#define USB_DEBUG_XHCI_DLG_CONTEXT         4151
#define USB_DEBUG_XHCI_DLG_STR_CONTEXT     4152

#define IDC_LIST                        4160
#define IDC_CAPTION                     4161
#define IDC_DUMP                        4162

// common to all for HCs
#define ID_QUIT_EMU                     4200
#define ID_CONTINUE_EMU                 4201
#define IDC_PORT_ADDR                   4202
#define IDC_FRAME_ADDRESS               4203
#define ID_APPLY                        4204
#define IDC_STACK                       4205
#define IDC_TREE_COMMENT                4206

#define IDC_STATIC                      -1


// UHCI items start with 5000 
// (items that we want to trigger an EN_CHANGE must start with 5000
//  and end in 5020)
#define IDC_U_EN_START                   5000
#define IDC_U_REG_COMMAND                 5000
#define IDC_U_REG_STATUS                  5001
#define IDC_U_REG_INTERRUPT               5002
#define IDC_U_REG_FRAME_NUM               5003
#define IDC_U_REG_FRAME_ADDRESS           5004
#define IDC_U_REG_SOF                     5005
#define IDC_U_REG_PORT0                   5006
#define IDC_U_REG_PORT1                   5007
#define IDC_U_EN_END                     5020

#define IDC_U_REG_COMMAND_B               5100
#define IDC_U_REG_STATUS_B                5101
#define IDC_U_REG_INTERRUPT_B             5102
#define IDC_U_REG_PORT0_B                 5103
#define IDC_U_REG_PORT1_B                 5104

#define IDC_U_REG_PORT0_TYPE              5140
#define IDC_U_REG_PORT1_TYPE              5141

#define IDC_HORZ_PTR                      5200
#define IDC_HORZ_T                        5201
#define IDC_HORZ_Q                        5202
#define IDC_VERT_PTR                      5203
#define IDC_VERT_T                        5204
#define IDC_VERT_Q                        5205
#define IDC_VERT_VF                       5206

#define IDC_LINK_PTR                      5210
#define IDC_LINK_T                        5211
#define IDC_LINK_Q                        5212
#define IDC_LINK_VF                       5213
#define IDC_STATUS_LS                     5214
#define IDC_ACTUAL_LEN                    5215
#define IDC_STATUS_NAK                    5216
#define IDC_STATUS_CRC                    5217
#define IDC_STATUS_BITSTUFF               5218
#define IDC_STATUS_RSVD                   5219
#define IDC_STATUS_ACTIVE                 5220
#define IDC_STATUS_STALLED                5221
#define IDC_STATUS_DATA_BUFF_ERR          5222
#define IDC_STATUS_BABBLE                 5223
#define IDC_STATUS_SPD                    5224
#define IDC_STATUS_IOC                    5225
#define IDC_STATUS_ISO                    5226
#define IDC_STATUS_CERR                   5227
#define IDC_DEVICE_ADDR                   5228
#define IDC_DEVICE_EP                     5229
#define IDC_DEVICE_MAXLEN                 5230
#define IDC_DEVICE_TOGGLE                 5231
#define IDC_DEVICE_BUFFER                 5232
#define IDC_COMBO_PID                     5233
#define IDC_DUMP_BUFFER                   5234



// OHCI items start with 6000



// EHCI items start with 7000



// xHCI items start with 8000
// (items that we want to trigger an EN_CHANGE must start with 8000
//  and end in 8019)
#define IDC_X_EN_START                   8000
#define IDC_X_REG_CAPLENGTH               8000
#define IDC_X_REG_HCSPARAMS1              8001
#define IDC_X_REG_HCSPARAMS2              8002
#define IDC_X_REG_HCSPARAMS3              8003
#define IDC_X_REG_HCCPARAMS1              8004
#define IDC_X_REG_DBOFF                   8005
#define IDC_X_REG_RTSOFF                  8006
#define IDC_X_REG_HCCPARAMS2              8007
#define IDC_X_REG_COMMAND                 8008
#define IDC_X_REG_STATUS                  8009
#define IDC_X_REG_PAGESIZE                8010
#define IDC_X_REG_DEVICE_NOTE             8011
#define IDC_X_REG_COMMAND_RING            8012
#define IDC_X_REG_DEV_CONTEXT_BASE        8013
#define IDC_X_REG_CONFIGURE               8014
#define IDC_X_REG_PORT0                   8015
#define IDC_X_REG_PORT1                   8016
#define IDC_X_REG_PORT2                   8017
#define IDC_X_REG_PORT3                   8018
#define IDC_X_REG_PORT4                   8019
#define IDC_X_REG_PORT5                   8020
#define IDC_X_REG_PORT6                   8021
#define IDC_X_REG_PORT7                   8022
#define IDC_X_REG_PORT8                   8023
#define IDC_X_REG_PORT9                   8024
#define IDC_X_REG_MFINDEX                 8025
#define IDC_X_EN_END                     8025

#define IDC_X_REG_EPORT0                  8040
#define IDC_X_REG_EPORT1                  8041
#define IDC_X_REG_EPORT2                  8042
#define IDC_X_REG_EPORT3                  8043
#define IDC_X_REG_EPORT4                  8044
#define IDC_X_REG_EPORT5                  8045
#define IDC_X_REG_EPORT6                  8046
#define IDC_X_REG_EPORT7                  8047
#define IDC_X_REG_EPORT8                  8048
#define IDC_X_REG_EPORT9                  8049

#define IDC_X_REG_PORT0_B                 8050
#define IDC_X_REG_PORT1_B                 8051
#define IDC_X_REG_PORT2_B                 8052
#define IDC_X_REG_PORT3_B                 8053
#define IDC_X_REG_PORT4_B                 8054
#define IDC_X_REG_PORT5_B                 8055
#define IDC_X_REG_PORT6_B                 8056
#define IDC_X_REG_PORT7_B                 8057
#define IDC_X_REG_PORT8_B                 8058
#define IDC_X_REG_PORT9_B                 8059

#define IDC_X_REG_PORT0_TYPE              8060
#define IDC_X_REG_PORT1_TYPE              8061
#define IDC_X_REG_PORT2_TYPE              8062
#define IDC_X_REG_PORT3_TYPE              8063
#define IDC_X_REG_PORT4_TYPE              8064
#define IDC_X_REG_PORT5_TYPE              8065
#define IDC_X_REG_PORT6_TYPE              8066
#define IDC_X_REG_PORT7_TYPE              8067
#define IDC_X_REG_PORT8_TYPE              8068
#define IDC_X_REG_PORT9_TYPE              8069

#define IDC_X_REG_PORT0_ETYPE             8070
#define IDC_X_REG_PORT1_ETYPE             8071
#define IDC_X_REG_PORT2_ETYPE             8072
#define IDC_X_REG_PORT3_ETYPE             8073
#define IDC_X_REG_PORT4_ETYPE             8074
#define IDC_X_REG_PORT5_ETYPE             8075
#define IDC_X_REG_PORT6_ETYPE             8076
#define IDC_X_REG_PORT7_ETYPE             8077
#define IDC_X_REG_PORT8_ETYPE             8078
#define IDC_X_REG_PORT9_ETYPE             8079

#define IDC_VIEW_TRB                      8080
#define IDC_VIEW_TD                       8080

#define IDC_DEBUG_RESET                 8081
#define IDC_DEBUG_ENABLE                8082
#define IDC_DEBUG_DOORBELL              8083
#define IDC_DEBUG_EVENT                 8084
#define IDC_DEBUG_SOF                   8085
#define IDC_DEBUG_NONEXIST              8086

#define IDC_TRB_DATA_PTR                  8100
#define IDC_TRB_INT_TARGET                8101
#define IDC_TRB_TD_SIZE                   8102
#define IDC_TRB_TRANS_LEN                 8103
#define IDC_TRB_TYPE                      8104
#define IDC_TRB_BEI                       8105
#define IDC_TRB_IDT                       8106
#define IDC_TRB_IOC                       8107
#define IDC_TRB_CH                        8108
#define IDC_TRB_NS                        8109
#define IDC_TRB_ISP                       8110
#define IDC_TRB_ENT                       8111
#define IDC_TRB_C                         8112

#define IDC_TRB_WVALUE                    8120
#define IDC_TRB_BREQUEST                  8121
#define IDC_TRB_BREQUESTTYPE              8122
#define IDC_TRB_WLENGTH                   8123
#define IDC_TRB_WINDEX                    8124
#define IDC_TRB_TRT                       8125

#define IDC_TRB_DIR                       8126
#define IDC_TRB_TC                        8127
#define IDC_TRB_SLOT_TYPE                 8128
#define IDC_TRB_SLOT_ID                   8129
#define IDC_TRB_BSR                       8130
#define IDC_TRB_DECONFIG                  8131
#define IDC_TRB_EP_ID                     8132
#define IDC_TRB_TSP                       8133
#define IDC_TRB_SP                        8134
#define IDC_TRB_STREAMID                  8135
#define IDC_TRB_VF_ID                     8136
#define IDC_TRB_BELT                      8137
#define IDC_TRB_DEV_SPEED                 8138
#define IDC_TRB_SCT                       8139
#define IDC_TRB_DCS                       8140
#define IDC_TRB_HDR_HI                    8141
#define IDC_TRB_FTYPE                     8142
#define IDC_TRB_COMP_CODE                 8143
#define IDC_TRB_ED                        8144
#define IDC_TRB_COMP_LPARAM               8145
#define IDC_TRB_COMP_HPARAM               8146
#define IDC_TRB_NOT_TYPE                  8147
#define IDC_RING_TYPE                     8148
#define IDC_IN_CONTEXT                    8149

// context structure
#define IDC_CONTEXT_NEXT                8200
#define IDC_CONTEXT_PREV                8201
#define IDC_CONTEXT_DROP                8202
#define IDC_CONTEXT_DROP_B              8203
#define IDC_CONTEXT_ADD                 8204
#define IDC_CONTEXT_ADD_B               8205
#define IDC_CONTEXT_ALT_SETTING         8206
#define IDC_CONTEXT_INTFACE_NUM         8207
#define IDC_CONTEXT_CONFIG_VALUE        8208
#define IDC_CONTEXT_OF_STR              8209
#define IDC_CONTEXT_STREAM_CONTEXT      8210

#define IDC_CONTEXT_ENTRIES             8220
#define IDC_CONTEXT_HUB                 8221
#define IDC_CONTEXT_MTT                 8222
#define IDC_CONTEXT_SPEED               8223
#define IDC_CONTEXT_ROUTE_STRING        8224
#define IDC_CONTEXT_NUM_PORTS           8225
#define IDC_CONTEXT_RH_PORT_NUM         8226
#define IDC_CONTEXT_MAX_EXIT_LAT        8227
#define IDC_CONTEXT_INT_TARGET          8228
#define IDC_CONTEXT_TTT                 8229
#define IDC_CONTEXT_TT_PORT_NUM         8230
#define IDC_CONTEXT_TT_HUB_SLOT_ID      8231
#define IDC_CONTEXT_SLOT_STATE          8232
#define IDC_CONTEXT_DEV_ADDRESS         8233
#define IDC_CONTEXT_RSVDO_SLOT_0        8234
#define IDC_CONTEXT_RSVDO_SLOT_1        8235
#define IDC_CONTEXT_RSVDO_SLOT_2        8236
#define IDC_CONTEXT_RSVDO_SLOT_3        8237
#define IDC_CONTEXT_RSVDO_SLOT_4        8238
#define IDC_CONTEXT_RSVDO_SLOT_5        8239
#define IDC_CONTEXT_RSVDO_SLOT_6        8240
#define IDC_CONTEXT_RSVDO_SLOT_7        8241
#define IDC_CONTEXT_RSVDO_SLOT_8        8242
#define IDC_CONTEXT_RSVDO_SLOT_9        8243
#define IDC_CONTEXT_RSVDO_SLOT_10       8244
#define IDC_CONTEXT_RSVDO_SLOT_11       8245
#define IDC_CONTEXT_SPEED_STR           8246
#define IDC_CONTEXT_SLOT_STATE_STR      8247

#define IDC_CONTEXT_MAX_ESIT_HI         8260
#define IDC_CONTEXT_INTERVAL            8261
#define IDC_CONTEXT_LSA                 8262
#define IDC_CONTEXT_MAX_PSTREAMS        8263
#define IDC_CONTEXT_MULT                8264
#define IDC_CONTEXT_EP_STATE            8265
#define IDC_CONTEXT_MAX_PACKET_SIZE     8266
#define IDC_CONTEXT_MAX_BURST_SIZE      8267
#define IDC_CONTEXT_HID                 8268
#define IDC_CONTEXT_EP_TYPE             8269
#define IDC_CONTEXT_CERR                8270
#define IDC_CONTEXT_TR_DEQUEUE_PTR      8271
#define IDC_CONTEXT_DCS                 8272
#define IDC_CONTEXT_MAX_ESIT_LO         8273
#define IDC_CONTEXT_AVERAGE_LEN         8274
#define IDC_CONTEXT_RSVDO_EP_0          8275
#define IDC_CONTEXT_RSVDO_EP_1          8276
#define IDC_CONTEXT_RSVDO_EP_2          8277
#define IDC_CONTEXT_RSVDO_EP_3          8278
#define IDC_CONTEXT_RSVDO_EP_4          8279
#define IDC_CONTEXT_RSVDO_EP_5          8280
#define IDC_CONTEXT_RSVDO_EP_6          8281
#define IDC_CONTEXT_RSVDO_EP_7          8282
#define IDC_CONTEXT_RSVDO_EP_8          8283
#define IDC_CONTEXT_RSVDO_EP_9          8284
#define IDC_CONTEXT_RSVDO_EP_10         8285
#define IDC_CONTEXT_EP_STATE_STR        8286
#define IDC_CONTEXT_EP_TYPE_STR         8287
#define IDC_CONTEXT_MAXPS_STR           8288

// string context
#define IDC_STR_CONTEXT_DQPTR           8301
#define IDC_STR_CONTEXT_SCT             8302
#define IDC_STR_CONTEXT_DCS             8303
#define IDC_STR_CONTEXT_STOPPED         8304
#define IDC_STR_CONTEXT_RSVDO_0         8305
#define IDC_STR_CONTEXT_RSVDO_1         8306
#define IDC_CONTEXT_SCT_STR             8307

