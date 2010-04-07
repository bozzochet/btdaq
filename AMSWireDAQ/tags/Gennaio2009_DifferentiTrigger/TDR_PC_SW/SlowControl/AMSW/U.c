// file U.c
//
// 'Universal' "Pro" Controller for:
//  UPSFE, TPSFE, TBS, S9011AU, S9011AT, EPSFE, S9011AUG
//
// A.Lebedev Sep-2004...
//

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title;                                          // global variable
char *date  = {"25-Dec-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USE_CAN
#include "uscmlib.h"
int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x44;                                // global variable
int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable
int P = 0;                                         // global variable
#endif

#ifdef USE_AMSW
#include "daqlib.h"
int TX = 2;                 // global variable
int RX = 2;                 // global variable
int16 AMSW_PATH[5];         // global variable
int P = 0;                  // global variable
#endif

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

#define TIMER_PERIOD 2.0     // in seconds

#define status_NN -2
#define status_NA -1
#define status_KO  0
#define status_OK  1

bool extra_bus   = FALSE;
bool show_ADC    = FALSE;
bool show_ADC_VK = FALSE;

struct _map {
  int   brd;
  int   clk;
  int   dat;
  char *slt;
  char *hlf;
};

struct _map map[6][2];

struct _map S9011AU_old[1][2] = {
//
//~board--clk-dat-slot--half
 {{ 0x06,  6, 14,  "-", "A"},
  { 0x06,  7, 15,  "-", "B"}}
};

struct _map S9011AU_new[1][2] = {
//
//~board--clk-dat-slot--half
 {{ 0x06,  3,  3,  "-", "A"},
  { 0x06,  4,  4,  "-", "B"}}
};

struct _map S9011AU_VK[1][2] = {
//
//~board--clk-dat-slot--half
 {{ 0x03,  3,  3,  "-", "A"},
  { 0x03,  4,  4,  "-", "B"}}
};

struct _map S9011AT[2][2] = {
//
//~board--clk-dat--bus--half
 {{ 0x03,  4,  4,  "A", "A"},
  { 0x03,  5,  5,  "A", "B"}},
 {{ 0x03,  6,  6,  "B", "A"},
  { 0x03,  7,  7,  "B", "B"}}
};

struct _map UPSFE_old[6][2] = {
//
//~board--clk-dat-slot--half
 {{ 0x04,  4, 12,  "2", "A"},
  { 0x04,  5, 13,  "2", "B"}},
 {{ 0x06,  4, 12,  "3", "A"},
  { 0x06,  5, 13,  "3", "B"}},
 {{ 0x0C,  4, 12,  "6", "A"},
  { 0x0C,  5, 13,  "6", "B"}},
 {{ 0x0E,  4, 12,  "7", "A"},
  { 0x0E,  5, 13,  "7", "B"}},
 {{ 0x16,  4, 12, "11", "A"},
  { 0x16,  5, 13, "11", "B"}},
 {{ 0x18,  4, 12, "12", "A"},
  { 0x18,  5, 13, "12", "B"}}
};

struct _map UPSFE_new[3][2] = {
//
//~board--clk-dat-slot--half
 {{ 0x04,  2,  2,  "2", "A"},
  { 0x04,  7,  7,  "2", "B"}},
 {{ 0x0A,  2,  2,  "5", "A"},
  { 0x0A,  7,  7,  "5", "B"}},
 {{ 0x10,  2,  2,  "8", "A"},
  { 0x10,  7,  7,  "8", "B"}}
};

struct _map UPSFE_VK[3][2] = {
//
//~board--clk-dat-slot--half
 {{ 0x02,  2,  2,  "2", "A"},
  { 0x02,  7,  7,  "2", "B"}},
 {{ 0x05,  2,  2,  "5", "A"},
  { 0x05,  7,  7,  "5", "B"}},
 {{ 0x08,  2,  2,  "8", "A"},
  { 0x08,  7,  7,  "8", "B"}}
};

struct _map TPSFE[4][2] = {
//
//~board--clk-dat-slot--half
 {{ 0x04,  0,  0,  "4", "A"},
  { 0x04,  2,  2,  "4", "B"}},
 {{ 0x06,  0,  0,  "6", "A"},
  { 0x06,  2,  2,  "6", "B"}},
 {{ 0x0E,  1,  1, "14", "A"},
  { 0x0E,  3,  3, "14", "B"}},
 {{ 0x10,  1,  1, "16", "A"},
  { 0x10,  3,  3, "16", "B"}}
};

struct _map TBS[2][2] = {
//
//~board--clk-dat-slot--half
 {{ 0x05,  0,  0,  "5", "A"},
  { 0x05,  2,  2,  "5", "B"}},
 {{ 0x0F,  1,  1, "15", "A"},
  { 0x0F,  3,  3, "15", "B"}}
};

struct _map EPSFE[3][2] = {
//
//~board--clk-dat-slot--half
 {{ 0x02,  2,  2,  "2", "A"},
  { 0x02,  7,  7,  "2", "B"}},
 {{ 0x05,  2,  2,  "5", "A"},
  { 0x05,  7,  7,  "5", "B"}},
 {{ 0x08,  2,  2,  "9", "A"},
  { 0x08,  7,  7,  "9", "B"}}
};

struct _map S9011AUG[1][2] = {
//
//~board--clk-dat--bus--half
 {{ 0x03,  6,  6,  "-", "A"},
  { 0x03,  7,  7,  "-", "B"}}
};

//~--

FL_OBJECT *SLOT, *HALF;
FL_OBJECT *CLK, *DAT, *BRD[5];
int nslot, slot, half, Sandor_bus, brd, clk, dat;

int32 wri, rea;

FL_OBJECT *REA[16][16];
int32 rea_cod[16], rea_was[16];
int rea_sta[16];
int rea_first[16];

FL_OBJECT *WRI[16][16];
int32 wri_cod[16], wri_was[16];
int wri_sta[16];
int wri_shown[16];

FL_OBJECT *ADC[16];
int32 ADC_cod[16], ADC_was[16];
int ADC_sta[16];
int ADC_first[16];

char *hex[16] = {
  "0", "1", "2", "3", "4", "5", "6", "7", 
  "8", "9", "A", "B", "C", "D", "E", "F"};
  
//~--

#define NLAY 12

int ilay;

typedef struct {
  int16 reg;
  char *title;
  struct {
    char *title;
    int from;
    int to;
  } field[16];
} registers;

struct {
  char *tag;
  char *title;
  int nrea;
  registers rea[16];
  int nwri;
  registers wri[16];
} layout[NLAY] = {

 {"S9011AT",
  "S9011AT  Pro  Controller",
  10,
 {{ 0, "0000\nSEL",   {                      {  "S",0,0},{NULL}}},
  { 1, "0001\nActel", {{"t",8,8}, {"s",4,4}, {"cmd",0,1},{NULL}}},
  { 2, "0010\nDCDC L",{             {"last command",0,8},{NULL}}},
  { 3, "0011\nDCDC L",{                  {"tripped",0,8},{NULL}}},
  { 4, "0100\nDCDC H",{             {"last command",0,8},{NULL}}},
  { 5, "0101\nDCDC H",{                  {"tripped",0,8},{NULL}}},
  { 6, "0110\nDCDC L",{                {"feed back",0,8},{NULL}}},
  { 7, "0111\nDCDC H",{                {"feed back",0,8},{NULL}}},
  { 8, "1000\nDCDC L",{                   {"status",0,8},{NULL}}},
  { 9, "1001\nDCDC H",{                   {"status",0,8},{NULL}}}},
  5,
 {{ 0, "0000\nSEL",   {{  "C",0,0},{NULL}}},
  { 1, "0001\nActel", {{"off",0,1},{NULL}}},
  { 2, "0010\nDCDC L",{{"off",0,8},{NULL}}},
  { 3, "0011",        {            {NULL}}},
  { 4, "0100\nDCDC H",{{"off",0,8},{NULL}}}}},
  
 {"S9011AT_VK",
  "S9011AT_VK  Pro  Controller",
  5,
 {{ 0, "0000\nActel",      {{"P",15,15},{"S",14,14},{"T",13,13},    {"C",0,0},{NULL}}},
  { 1, "0001\nTPSFE",      {{"Status",8,15},            {"Local Control",0,7},{NULL}}},
  { 2, "0010\nTBS,TDR2",   {{"Status",8,15},            {"Local Control",0,7},{NULL}}},
  { 3, "0011\nGlobal Ctrl",{{"TBS",12,15},{"TDR2",8,11},{"TPSFE",0,7},        {NULL}}},
  { 4, "0100\nTrip",       {{"TBS",12,15},{"TDR2",8,11},{"TPSFE",0,7},        {NULL}}}},
  3,
 {{ 0, "0000\nActel",   {                                                                                  {"Act",0,1},{NULL}}},
  { 1, "0001\nTPSFE",   {{"3B",14,15},{"3A",12,13},{"2B",10,11},{"2A",8,9},{"1B",6,7},{"1A",4,5},{"0B",2,3},{"0A",0,1},{NULL}}},
  { 2, "0010\nTBS,TDR2",{{"1B",14,15},{"1A",12,13},{"0B",10,11},{"0A",8,9},{"B1",6,7},{"1A",4,5},{"B0",2,3},{"0A",0,1},{NULL}}}}},
  
 {"TPSFE",
  "TPSFE  Pro  Controller",
  14,
 {{ 0, "0000\nSEL",      {                                                         {"S",           0,0},{NULL}}},
  { 1, "0001\nActel",    {                    {"t",        8,8}, {"s",        4,4},{"cmd",         0,1},{NULL}}},
  { 2, "0010\nLRS",      {                    {"status",   8,13},                  {"last command",0,5},{NULL}}},
  { 3, "0011\nLRK",      {                    {"status",   8,13},                  {"last command",0,5},{NULL}}},
  { 4, "0100\nLR fb",    {                    {"LRK fb",   8,13},                  {"LRS fb",      0,5},{NULL}}},
  { 5, "0101\nTDR2",     {                    {"status",   8,13},                  {"last command",0,5},{NULL}}},
  { 6, "0110\nAuto",     {                                                         {"T",1,1}, {"L",0,0},{NULL}}},
  { 7, "0111\nCounter",  {                                                         {"Actel",0,3},       {NULL}}},
  { 8, "1000\nCounters", {{"LRS-3", 12,15},   {"LRS-2", 8,11},   {"LRS-1", 4,7},   {"LRS-0",       0,3},{NULL}}},
  { 9, "1001\nCounters", {                                       {"LRS-5", 4,7},   {"LRS-4",       0,3},{NULL}}},
  {10, "1010\nCounters", {{"LRK-3", 12,15},   {"LRK-2", 8,11},   {"LRK-1", 4,7},   {"LRK-0",       0,3},{NULL}}},
  {11, "1011\nCounters", {                                       {"LRK-5", 4,7},   {"LRK-4",       0,3},{NULL}}},
  {13, "1101\nCounters", {{"TDR2-3",   12,15},{"TDR2-2",   8,11},{"TDR2-1",   4,7},{"TDR2-0",      0,3},{NULL}}},
  {14, "1110\nCounters", {                                       {"TDR2-5",   4,7},{"TDR2-4",      0,3},{NULL}}}},
  9,
 {{ 0, "0000\nSEL",      {                      {"C",    0,0},{NULL}}},
  { 1, "0001\nActel",    {                      {"off",  0,1},{NULL}}},
  { 2, "0010\nLRS",      {                      {"off",  0,5},{NULL}}},
  { 3, "0011\nLRK",      {                      {"off",  0,5},{NULL}}},
  { 4, "0100",           {                                    {NULL}}},
  { 5, "0101\nTDR2",     {                      {"off",  0,5},{NULL}}},
  { 6, "0110",           {                                    {NULL}}},
  { 7, "0111\nAuto LR",  {{"E",10,10},{"R",9,9},              {NULL}}},
  { 8, "1000\nAutoTDR",  {{"E",10,10},{"R",9,9},              {NULL}}}}},

 {"TPSFE_VK",
  "TPSFE_VK  Pro  Controller",
  9,
 {{ 0, "0000\nTDR",        {{"P",15,15},{"S",14,14},{"Status",8,13},{"C",7,7},{"A",6,6},{"Local Control",0,5},{NULL}}},
  { 1, "0001\nVGS",        {                        {"Status",8,13},          {"A",6,6},{"Local Control",0,5},{NULL}}},
  { 2, "0010\nVGK",        {                        {"Status",8,13},          {"A",6,6},{"Local Control",0,5},{NULL}}},
  { 3, "0011\nGlobal Ctrl",{                        {"LRK",   8,13},                    {"LRS",          0,5},{NULL}}},
  { 4, "0100\nCounters",   {{"TDR3",12,15},{"TDR2",8,11},{"TDR1",4,7},{"TDR0",0,3},{NULL}}},
  { 5, "0101\nCounters",   {{"LRS1",12,15},{"LRS0",8,11},{"TDR5",4,7},{"TDR4",0,3},{NULL}}},
  { 6, "0110\nCounters",   {{"LRS5",12,15},{"LRS4",8,11},{"LRS3",4,7},{"LRS2",0,3},{NULL}}},
  { 7, "0111\nCounters",   {{"LRK3",12,15},{"LRK2",8,11},{"LRK1",4,7},{"LRK0",0,3},{NULL}}},
  { 8, "1000\nCounters",   {              {"ACTEL",8,11},{"LRK5",4,7},{"LRK4",0,3},{NULL}}}},
  3,
 {{ 0, "0000\nTDR",{{"ACT",14,15},{"A",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}},
  { 1, "0001\nLRS",{              {"A",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}},
  { 2, "0010\nLRK",{              {"A",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}}}},

 {"TBS",
  "TBS  Pro  Controller",
  8,
 {{ 0, "0000\nSEL",      {                            {"S",                      0,0},{NULL}}},
  { 1, "0001\nActel",    {{"t",  8, 8}, {"s",    4,4},{"cmd",                    0,1},{NULL}}},
  { 2, "0010\nlast cmd", {{"LR", 8, 9}, {"volt", 4,7},{"off",                    0,3},{NULL}}},
  { 3, "0011\nfeedback", {              {"volt", 4,7},{"off",                    0,3},{NULL}}},
  { 4, "0100",           {                                                            {NULL}}},
  { 5, "0101\nJINF",     {{"t",  8, 8}, {"s",    4,4},{"c",                      0,0},{NULL}}},
  { 6, "0110\nADCmod",   {                            {"U",3,3},{"S",2,2},{"PDS",0,1},{NULL}}},
  { 7, "0111\nADCdata",  {                            {"ADC Data",              0,12},{NULL}}}},
  7,
 {{ 0, "0000\nSEL",      {                                  {"C",  0,0},{NULL}}},
  { 1, "0001\nActel",    {                                  {"off",0,1},{NULL}}},
  { 2, "0010\ncontrol",  {{"volt", 4,7},{"off",                    0,3},{NULL}}},
  { 3, "0011",           {                                              {NULL}}},
  { 4, "0100",           {                                              {NULL}}},
  { 5, "0101\nJINF",     {                                  {"o",  0,0},{NULL}}},
  { 6, "0110\nADCmod",   {              {"U",3,3},{"S",2,2},{"PDS",0,1},{NULL}}}}},

 {"TBS_VK",
  "TBS_VK  Pro  Controller",
  2,
 {{ 0, "0000\nActel",   {{"P",15,15},{"S",14,14},{"T",13,13},{"LR",8,9}, {"C",0,0},{NULL}}},
  { 1, "0001\n60/80,LR",{{"Global Control",8,15},            {"Local Control",0,7},{NULL}}}},
  2,
 {{ 0, "0000\nActel",   {                                                                          {"ACT",0,1},{NULL}}},
  { 1, "0001\n60/80,LR",{{"3",14,15},{"2",12,13},{"1",10,11},{"0",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}}}},

 {"S9011AU",
  "S9011AU  Pro  Controller",
  6,
 {{ 0, "0000\nSEL",   {                      {  "S",0,0},{NULL}}},
  { 1, "0001\nActel", {{"t",8,8}, {"s",4,4}, {"cmd",0,1},{NULL}}},
  { 2, "0010\nDCDC L",{                   {"status",0,8},{NULL}}},
  { 3, "0011\nDCDC L",{                  {"tripped",0,8},{NULL}}},
  { 4, "0100\nDCDC H",{                   {"status",0,8},{NULL}}},
  { 5, "0101\nDCDC H",{                  {"tripped",0,8},{NULL}}}},
  5,
 {{ 0, "0000\nSEL",   {            {  "C",0,0},{NULL}}},
  { 1, "0001\nActel", {{"R",15,15},{"off",0,1},{NULL}}},
  { 2, "0010\nDCDC L",{{"R",15,15},{"off",0,8},{NULL}}},
  { 3, "0011",        {                        {NULL}}},
  { 4, "0100\nDCDC H",{{"R",15,15},{"off",0,8},{NULL}}}}},

 {"S9011AU_VK",
  "S9011AU_VK  Pro  Controller",
  4,
 {{ 0, "0000\nUHVG",       {{"P",15,15},{"A",14,14},{"Status",8,13},{"A",6,6},  {"Local Control",0,5},{NULL}}},
  { 1, "0001\nUPSFE",      {{"3.3",14,15},          {"Status",8,13},{"3.3",6,7},{"Local Control",0,5},{NULL}}},
  { 2, "0010\nGlobal Ctrl",{{"3.3",14,15},          {"UPSFE", 8,13},            {"UHVG",         0,5},{NULL}}},
  { 3, "0011\nTrip",       {{"3.3",14,15},          {"UPSFE", 8,13},{"A",6,6},  {"UHVG",         0,5},{NULL}}}},
  2,
 {{ 0, "0000\nUHVG",       {          {"ACT",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}},
  { 1, "0001\nUPSFE",      {{"1",14,15},{"0",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}}}},
  
#if stillnecessary
 {"UPSFEv1_v1",
  "UPSFEv1 (old)  Pro  Controller",
  8,
 {{ 0, "0000\nCounters", {{"UHVG-3",   12,15},{"UHVG-2",   8,11},{"UHVG-1",   4,7},{"UHVG-0",   0,3},{NULL}}},
  { 1, "0001\nCounters", {{"Analog-3", 12,15},{"Analog-2", 8,11},{"Analog-1", 4,7},{"Analog-0", 0,3},{NULL}}},
  { 2, "0010\nCounters", {{"JINF-0",   12,15},{"Analog-6", 8,11},{"Analog-5", 4,7},{"Analog-4", 0,3},{NULL}}},
  { 3, "0011\nCounters", {{"Digital-3",12,15},{"Digital-2",8,11},{"Digital-1",4,7},{"Digital-0",0,3},{NULL}}},
  { 4, "0100\nCounters", {{"JINF-1",   12,15},{"Digital-6",8,11},{"Digital-5",4,7},{"Digital-4",0,3},{NULL}}},
  { 5, "0101\nMonitors", {                    {"UHVG",     8,11},{"UDR2",     4,7},{"JINF",     0,3},{NULL}}},
  { 6, "0110\nStatus",   {{"S",        15,15},{"Analog",   7,13},                  {"Digital",  0,6},{NULL}}},
  { 7, "0111\nCounters", {{"UDR2-3",   12,15},{"UDR2-2",   8,11},{"UDR2-1",   4,7},{"UDR2-0",   0,3},{NULL}}}},
  5,
 {{ 0, "0000\nSEL",      {                                                            {"C",  0,0},  {NULL}}},
  { 1, "0001\nAuto LR",  {{"C",9,9},{"L",8,8},{"C",7,7},{"C",6,6},{"L",5,5},{"L",4,4},{"value",0,3},{NULL}}},
  { 2, "0010\nLR pairs", {                                                            {"off",0,6},  {NULL}}},
  { 3, "0011",           {                                                                          {NULL}}},
  { 4, "0100\nOFF/SWP",  {         {"UHVG",8,11},{"off",6,7},{"swp",4,5},{"JINF",2,3},{"Act",0,1},  {NULL}}}}},
#endif
  
#if stillnecessary
 {"UPSFEv1_v2",
  "UPSFEv1 (new)  Pro  Controller",
  14,
 {{ 0, "0000\nSEL",      {                                                         {"S",           0,0},{NULL}}},
  { 1, "0001\nActel",    {                    {"s",        8,8},                   {"cmd",         0,1},{NULL}}},
  { 2, "0010\nAnalog",   {                    {"status",   8,14},                  {"last command",0,6},{NULL}}},
  { 3, "0011\nDigital",  {                    {"status",   8,14},                  {"last command",0,6},{NULL}}},
  { 4, "0100\nUHVG",     {                    {"status",   8,11},                  {"last cmd",    0,3},{NULL}}},
  { 5, "0101\nUDR2",     {                    {"status",   8,11},                  {"last cmd",    0,3},{NULL}}},
  { 6, "0110\nJINF",     {                    {"sta",      8,9},                   {"cmd",         0,1},{NULL}}},
  { 7, "0111\nCounters", {{"JINF-1",   12,15},{"JINF-0",   8,11},                  {"Actel",0,3},       {NULL}}},
  { 8, "1000\nCounters", {{"Analog-3", 12,15},{"Analog-2", 8,11},{"Analog-1", 4,7},{"Analog-0",    0,3},{NULL}}},
  { 9, "1001\nCounters", {                    {"Analog-6", 8,11},{"Analog-5", 4,7},{"Analog-4",    0,3},{NULL}}},
  {10, "1010\nCounters", {{"Digital-3",12,15},{"Digital-2",8,11},{"Digital-1",4,7},{"Digital-0",   0,3},{NULL}}},
  {11, "1011\nCounters", {                    {"Digital-6",8,11},{"Digital-5",4,7},{"Digital-4",   0,3},{NULL}}},
  {12, "1100\nCounters", {{"UHVG-3",   12,15},{"UHVG-2",   8,11},{"UHVG-1",   4,7},{"UHVG-0",      0,3},{NULL}}},
  {13, "1101\nCounters", {{"UDR-3",    12,15},{"UDR-2",    8,11},{"UDR-1",    4,7},{"UDR-0",       0,3},{NULL}}}},
  9,
 {{ 0, "0000\nSEL",      {                                          {"C",    0,0},{NULL}}},
  { 1, "0001\nActel",    {                                          {"off",  0,1},{NULL}}},
  { 2, "0010\nLR pairs", {                                          {"off",  0,6},{NULL}}},
  { 3, "0011",           {                                                        {NULL}}},
  { 4, "0100\nUHVG",     {                                          {"off",  0,3},{NULL}}},
  { 5, "0101\nUDR2",     {            {"O",3,3},{"S",2,2},{"O",1,1},{"S",    0,0},{NULL}}},
  { 6, "0110\nJINF",     {                                          {"off",  0,1},{NULL}}},
  { 7, "0111\nAuto LR",  {{"E",10,10},{"R",9,9},{"L",8,8},          {"value",0,3},{NULL}}},
  { 8, "1000\nAuto Ext", {{"E",10,10},{"R",9,9},{"L",8,8},          {"value",0,3},{NULL}}}}},
#endif
  
 {"UPSFEv2",
  "UPSFEv2  Pro  Controller",
  14,
 {{ 0, "0000\nSEL",      {                                                         {"S",           0,0},{NULL}}},
  { 1, "0001\nActel",    {                    {"s",        8,8},                   {"cmd",         0,1},{NULL}}},
  { 2, "0010\nAnalog",   {                    {"status",   8,14},                  {"last command",0,6},{NULL}}},
  { 3, "0011\nDigital",  {                    {"status",   8,14},                  {"last command",0,6},{NULL}}},
  { 4, "0100\nUHVG",     {                    {"status",   8,11},                  {"last cmd",    0,3},{NULL}}},
  { 5, "0101\nUDR2",     {                    {"status",   8,11},                  {"last cmd",    0,3},{NULL}}},
  { 6, "0110\nJINF",     {                    {"sta",      8,9},                   {"cmd",         0,1},{NULL}}},
  { 7, "0111\nCounters", {{"JINF-1",   12,15},{"JINF-0",   8,11},                  {"Actel",0,3},       {NULL}}},
  { 8, "1000\nCounters", {{"Analog-3", 12,15},{"Analog-2", 8,11},{"Analog-1", 4,7},{"Analog-0",    0,3},{NULL}}},
  { 9, "1001\nCounters", {                    {"Analog-6", 8,11},{"Analog-5", 4,7},{"Analog-4",    0,3},{NULL}}},
  {10, "1010\nCounters", {{"Digital-3",12,15},{"Digital-2",8,11},{"Digital-1",4,7},{"Digital-0",   0,3},{NULL}}},
  {11, "1011\nCounters", {                    {"Digital-6",8,11},{"Digital-5",4,7},{"Digital-4",   0,3},{NULL}}},
  {12, "1100\nCounters", {{"UHVG-3",   12,15},{"UHVG-2",   8,11},{"UHVG-1",   4,7},{"UHVG-0",      0,3},{NULL}}},
  {13, "1101\nCounters", {{"UDR2-3",   12,15},{"UDR2-2",   8,11},{"UDR2-1",   4,7},{"UDR2-0",      0,3},{NULL}}}},
  9,
 {{ 0, "0000\nSEL",      {                                          {"C",    0,0},{NULL}}},
  { 1, "0001\nActel",    {                                          {"off",  0,1},{NULL}}},
  { 2, "0010\nAnalog",   {                                          {"off",  0,6},{NULL}}},
  { 3, "0011\nDigital",  {                                          {"off",  0,6},{NULL}}},
  { 4, "0100\nUHVG",     {                                          {"off",  0,3},{NULL}}},
  { 5, "0101\nUDR2",     {            {"O",3,3},{"S",2,2},{"O",1,1},{"S",    0,0},{NULL}}},
  { 6, "0110\nJINF",     {                                          {"off",  0,1},{NULL}}},
  { 7, "0111\nAuto LR",  {{"E",10,10},{"R",9,9},{"L",8,8},          {"value",0,3},{NULL}}},
  { 8, "1000\nAuto Ext", {{"E",10,10},{"R",9,9},{"L",8,8},          {"value",0,3},{NULL}}}}},

 {"UPSFE_VK",
  "UPSFE_VK  Pro  Controller",
  11,
 {{ 0, "0000\nUDR",        {{"P",13,13},{"S",12,12},{"Status",8,11},{"C",5,5},{"A",4,4},{"Loc.Ctrl",     0,3},{NULL}}},
  { 1, "0001\nUHVG",       {                        {"Status",8,11},          {"A",4,4},{"Loc.Ctrl",     0,3},{NULL}}},
  { 2, "0010\nLRA",        {                        {"Status",8,14},          {"A",7,7},{"Local Control",0,6},{NULL}}},
  { 3, "0011\nLRB",        {                        {"Status",8,14},          {"A",7,7},{"Local Control",0,6},{NULL}}},
  { 4, "0100\nGlobal Ctrl",{                        {"LRB",   8,14},                    {"LRA",          0,6},{NULL}}},
  { 5, "0101\nCounters",   {{"UDR3", 12,15}, {"UDR2", 8,11}, {"UDR1", 4,7}, {"UDR0", 0,3}, {NULL}}},
  { 6, "0110\nCounters",   {{"UHVG3",12,15}, {"UHVG2",8,11}, {"UHVG1",4,7}, {"UHVG0",0,3}, {NULL}}},
  { 7, "0111\nCounters",   {{"LRA3", 12,15}, {"LRA2", 8,11}, {"LRA1", 4,7}, {"LRA0", 0,3}, {NULL}}},
  { 8, "1000\nCounters",   {{"ACTEL", 12,15},{"LRA6", 8,11}, {"LRA5", 4,7}, {"LRA4", 0,3}, {NULL}}},
  { 9, "1001\nCounters",   {{"LRB3", 12,15}, {"LRB2", 8,11}, {"LRB1", 4,7}, {"LRB0", 0,3}, {NULL}}},
  {10, "1010\nCounters",   {                 {"LRB6", 8,11}, {"LRB5", 4,7}, {"LRB4", 0,3}, {NULL}}}},
  4,
 {{ 0, "0000\nUDR", {                      {"ACT",10,11},{"A",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}},
  { 1, "0001\nUHVG",{                                    {"A",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}},
  { 2, "0010\nLRA", {{"A",14,15},{"6",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}},
  { 3, "0011\nLRB", {{"A",14,15},{"6",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}}}},

 {"EPSFE",
  "EPSFE  Pro  Controller",
  9,
 {{ 0, "0000\nEDR",        {{"P",15,15},{"S",14,14},{"Status",8,13},{"C",7,7},{"A",6,6},{"Local Control",0,5},{NULL}}},
  { 1, "0001\nVGA",        {                        {"Status",8,13},          {"A",6,6},{"Local Control",0,5},{NULL}}},
  { 2, "0010\nVGB",        {                        {"Status",8,13},          {"A",6,6},{"Local Control",0,5},{NULL}}},
  { 3, "0011\nGlobal Ctrl",{                        {"VGB",   8,13},                    {"VGA",          0,5},{NULL}}},
  { 4, "0100\nCounters",   {{"EDR3",12,15}, {"EDR2",8,11},  {"EDR1",4,7}, {"EDR0",0,3}, {NULL}}},
  { 5, "0101\nCounters",   {{"VGA1",12,15}, {"VGA0",8,11},  {"EDR5",4,7}, {"EDR4",0,3}, {NULL}}},
  { 6, "0110\nCounters",   {{"VGA5",12,15}, {"VGA4",8,11},  {"VGA3",4,7}, {"VGA2",0,3}, {NULL}}},
  { 7, "0110\nCounters",   {{"VGB3",12,15}, {"VGB2",8,11},  {"VGB1",4,7}, {"VGB0",0,3}, {NULL}}},
  { 8, "0111\nCounters",   {                {"ACTEL",8,11}, {"VGB5",4,7}, {"VGB4",0,3}, {NULL}}}},
  3,
 {{ 0, "0000\nEDR",{{"ACT",14,15},{"A",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}},
  { 1, "0001\nVGA",{              {"A",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}},
  { 2, "0010\nVGB",{              {"A",12,13},{"5",10,11},{"4",8,9},{"3",6,7},{"2",4,5},{"1",2,3},{"0",0,1},{NULL}}}}},

 {"S9011AUG",
  "S9011AUG  Pro  Controller",
  2,
 {{ 0, "0000\nSta,Local",  {{"P",15,15},{"B",12,14},{"S",11,11},{"A",8,10},{"T",7,7},{"B",4,6},{"C",3,3},{"A",0,2},{NULL}}},
  { 1, "0001\nTrip,Glbal", {            {"B",12,14},            {"A",8,10},          {"B",4,6},          {"A",0,2},{NULL}}}},
  1,
 {{ 0, "0000\nControl", {{"ACT",14,15},{"29B",12,13},{"12B",10,11},{"5B",8,9}, {"29A",4,5},{"12A",2,3},{"5A",0,1},{NULL}}}}}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);
void change_layout_callback_serial(FL_OBJECT *obj, long data);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_change_layout_panel(int x0, int y0);
void change_layout_callback_direct(FL_OBJECT *obj, long data);

void create_hi_lvl_adr_panel(int x0, int y0);
void slot_callback(FL_OBJECT *obj, long data);
void half_callback(FL_OBJECT *obj, long data);
void bus__callback(FL_OBJECT *obj, long data);
void update_hi_lvl_adr_panel(void);

void create_lo_lvl_adr_panel(int x0, int y0);
void brd_callback(FL_OBJECT *obj, long data);
void chp_callback(FL_OBJECT *obj, long data);
void clk_callback(FL_OBJECT *obj, long data);
void dat_callback(FL_OBJECT *obj, long data);
void update_lo_lvl_adr_panel(void);
void INIT_callback(FL_OBJECT *obj, long data);

void create_READ_panel(int x0, int y0, int *W, int *H);
void GET_callback(FL_OBJECT *obj, long data);
void update_READ_panel(void);

void create_WRITE_panel(int x0, int y0, int *W, int *H);
void WRI_callback(FL_OBJECT *obj, long data);
void SET_callback(FL_OBJECT *obj, long data);
void update_WRITE_panel(void);

void create_ADC_panel(int x0, int y0, int ind, int *W, int *H);
void ADC_callback(FL_OBJECT *obj, long data);
void ADC_ADC_callback(FL_OBJECT *obj, long data);
void update_ADC_panels(void);

void create_ADC_VK_panel(int x0, int y0, int ind, int *W, int *H);
void ADC_VK_callback(FL_OBJECT *obj, long data);
void ADC_ADC_VK_callback(FL_OBJECT *obj, long data);
void update_ADC_VK_panels(void);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  FL_OBJECT *obj;
  int X, Y, W, H, W1, H1, W2, H2, WW, HH;
  int twice;

  invalidate_data();
  ilay = 0;
  read_dat_config_file(dat_config_file_name);
  ilay = MIN(ilay, NLAY-1);

  X = Y = 0;
  WW = HH = 1;
  
  for (twice=0; twice<2; twice++) {

    title = layout[ilay].title;
    sprintf(progname, "U:%s", layout[ilay].tag);
#ifdef USE_CAN
    bgn_form(&MAIN, 0, 0, WW, HH);
#endif
#ifdef USE_AMSW
    bgn_form(&MAIN, 0, 0, WW, HH, "JINF Path");
#endif
    AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
    AFTER_RUN_FLASH_CALL       = refresh_objects;
    SW_BTN_CALLBACK            = switch_btn_callback;
  

    obj = fl_add_button(FL_HIDDEN_BUTTON, WW/2, 5, WW-125-5, 45, "");
      fl_set_object_callback(obj, change_layout_callback_serial, 0);

    invalidate_data();

    create_change_layout_panel( 0, 45);
    create_hi_lvl_adr_panel(    0, 95);
    create_lo_lvl_adr_panel(   70, 95);

    HH = 140;
    create_READ_panel(0, HH, &W1, &H1);
    WW = W1;

    create_WRITE_panel(WW, HH, &W2, &H2);
    WW = WW + W2;
    HH = HH + MAX(H1, H2);

    if (show_ADC) {
      if (show_ADC_VK) {
        create_ADC_VK_panel(0, HH, 0, &W, &H);
        create_ADC_VK_panel(W, HH, 1, &W, &H);
      }
      else {
        create_ADC_panel(0, HH, 0, &W, &H);
        create_ADC_panel(W, HH, 1, &W, &H);
      }
      HH = HH + H;
    }

    HH = HH + 30;

    end_form(MAIN);
  }
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void change_layout_callback_serial(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  ilay = MOD(ilay+1, NLAY);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ilay = MOD(ilay-1, NLAY);

  write_dat_config_file(dat_config_file_name);

  create_main_form();
}

//~-------------------------------------------------------------------------------

void invalidate_data(void) {

  int i;

  extra_bus    = FALSE;
  show_ADC     = FALSE;
  show_ADC_VK  = FALSE;
    
  memcpy(map, S9011AU_old, sizeof(S9011AU_old));
  nslot = sizeof(S9011AU_old) / sizeof(struct _map) / 2;

  if (strncmp(layout[ilay].tag, "S9011AU", 7) == 0) {
#ifdef USE_CAN
    memcpy(map, S9011AU_old, sizeof(S9011AU_old));
    nslot = sizeof(S9011AU_old) / sizeof(struct _map) / 2;
#endif
#ifdef USE_AMSW
    if (strcmp(layout[ilay].tag, "S9011AU_VK") == 0) {
      memcpy(map, S9011AU_VK, sizeof(S9011AU_VK));
      nslot = sizeof(S9011AU_VK) / sizeof(struct _map) / 2;
    }
    else {
      memcpy(map, S9011AU_new, sizeof(S9011AU_new));
      nslot = sizeof(S9011AU_new) / sizeof(struct _map) / 2;
    }
#endif
  }

  if (strncmp(layout[ilay].tag, "S9011AT", 7) == 0) {
    memcpy(map, S9011AT, sizeof(S9011AT));
    nslot = sizeof(S9011AT) / sizeof(struct _map) / 2;
    extra_bus = TRUE;;
  }

  if (strncmp(layout[ilay].tag, "UPSFE", 5) == 0) {
#ifdef USE_CAN
    memcpy(map, UPSFE_old, sizeof(UPSFE_old));
    nslot = sizeof(UPSFE_old) / sizeof(struct _map) / 2;
#endif
#ifdef USE_AMSW
    if (strcmp(layout[ilay].tag, "UPSFE_VK") == 0) {
      memcpy(map, UPSFE_VK, sizeof(UPSFE_VK));
      nslot = sizeof(UPSFE_VK) / sizeof(struct _map) / 2;
    }
    else {
      memcpy(map, UPSFE_new, sizeof(UPSFE_new));
      nslot = sizeof(UPSFE_new) / sizeof(struct _map) / 2;
    }
#endif
  }

  if (strncmp(layout[ilay].tag, "TPSFE", 5) == 0) {
    memcpy(map, TPSFE, sizeof(TPSFE));
    nslot = sizeof(TPSFE) / sizeof(struct _map) / 2;
  }

  if (strncmp(layout[ilay].tag, "TBS", 3) == 0) {
    memcpy(map, TBS, sizeof(TBS));
    nslot = sizeof(TBS) / sizeof(struct _map) / 2;
    show_ADC = TRUE;
    if (strcmp(layout[ilay].tag, "TBS_VK") == 0) show_ADC_VK = TRUE;
  }

  if (strncmp(layout[ilay].tag, "EPSFE", 5) == 0) {
    memcpy(map, EPSFE, sizeof(EPSFE));
    nslot = sizeof(EPSFE) / sizeof(struct _map) / 2;
  }

  if (strncmp(layout[ilay].tag, "S9011AUG", 8) == 0) {
    memcpy(map, S9011AUG, sizeof(S9011AUG));
    nslot = sizeof(S9011AUG) / sizeof(struct _map) / 2;
  }

  for (i=0; i<layout[ilay].nwri; i++) {
    wri_sta[i] = status_NN;
    wri_cod[i] = 0;
    wri_was[i] = 0;
    wri_shown[i] = FALSE;
  }

  for (i=0; i<layout[ilay].nrea; i++) {
    rea_sta[i] = status_NN;
    rea_cod[i] = 0;
    rea_was[i] = 0;
  }

  for (i=0; i<16; i++) {
    ADC_sta[i] = status_NN;
    ADC_cod[i] = 0;
    ADC_was[i] = 0;
  }

  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

void refresh_objects(void) {

  invalidate_data();
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void switch_btn_callback(int32 data) {

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(&ilay, sizeof(ilay), 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(&ilay, sizeof(ilay), 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//
//   Change Layout Panel (Direct Access)
//
//
//~============================================================================

void create_change_layout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h, w1 = 80, h1 = 15, h2 = 25;
  struct {
    char *txt;
    int   mod;
    int   old;
    int   new;
  } lay [7] = {{"S9011AT",  1, 0, 1},
               {"TPSFE",    1, 2, 3},
               {"TBS",      1, 4, 5},
               {"S9011AU",  1, 6, 7},
               {"UPSFE",    1, 8, 9},
               {"EPSFE",    0,-1,10},
               {"S9011AUG", 0,-1,11}};

  w = 5 + 7 * (w1 + 5);
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  x = x0 + 5;
  y = y0 + 5;
  
  for (i=0; i<7; i++) {
    if (lay[i].mod) {
      add_frame_box(x, y, w1, h1, lay[i].txt, 'S');
      obj = add_normal_button(x,      y+h1, w1/2, h2, "OLD", 'M');
        fl_set_object_callback(obj, change_layout_callback_direct, lay[i].old);
      obj = add_normal_button(x+w1/2, y+h1, w1/2, h2, "VK", 'M');
        fl_set_object_callback(obj, change_layout_callback_direct, lay[i].new);
    }
    else {
      obj = add_normal_button(x, y, w1, h1+h2, lay[i].txt, 'M');
        fl_set_object_callback(obj, change_layout_callback_direct, lay[i].new);
    }
    x = x + w1 + 5;
  }
}

//~----------------------------------------------------------------------------

void change_layout_callback_direct(FL_OBJECT *obj, long data) {

  ilay = data;

  write_dat_config_file(dat_config_file_name);

  create_main_form();
}

//~============================================================================
//
//
//   High Level Address Panel
//
//
//~============================================================================

void create_hi_lvl_adr_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, size, w, h, w1 = 30, h1 = 15, h2 = 20;

  slot = 0;
  half = 0;
  Sandor_bus  = 0;

  brd = map[slot][half].brd;
  clk = map[slot][half].clk;
  dat = map[slot][half].dat;

  w = 5 + 2 * w1 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  size = FL_SMALL_SIZE;
  x = x0 + 5;
  y = y0 + 5;
  if (extra_bus) obj = add_frame_box(x, y, w1, h1, "Bus", size);
  else           obj = add_frame_box(x, y, w1, h1, "Slot", size);

  x = x + w1;
  obj = add_frame_box(x, y, w1, h1, "Half", size);

  size = FL_MEDIUM_SIZE;
  x = x0 + 5;
  y = y + h1;
  SLOT =
  obj = add_switch_button(x, y, w1, h2, " ", size);
    fl_set_object_callback(obj, slot_callback, 0);

  x = x + w1;
  HALF =
  obj = add_switch_button(x, y, w1, h2, " ", size);
    fl_set_object_callback(obj, half_callback, 0);
}

//~----------------------------------------------------------------------------

void slot_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  slot = MOD(slot+1, nslot);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) slot = MOD(slot-1, nslot);

  if (slot != -1 && half != -1) {
    brd = map[slot][half].brd;
    clk = map[slot][half].clk;
    dat = map[slot][half].dat;
  }

  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void half_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  half = MOD(half+1, 2);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) half = MOD(half-1, 2);

  if (slot != -1 && half != -1) {
    brd = map[slot][half].brd;
    clk = map[slot][half].clk;
    dat = map[slot][half].dat;
  }

  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void bus__callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  Sandor_bus = MOD(Sandor_bus+1, 2);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) Sandor_bus = MOD(Sandor_bus-1, 2);

  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void update_hi_lvl_adr_panel(void) {

  FL_OBJECT *obj;

  obj = SLOT;
  if (slot != -1) {
    fl_set_object_label(obj, map[slot][0].slt);
    fl_set_object_color(obj, FL_WHEAT, FL_RED);
  }
  else {
    fl_set_object_label(obj, " ");
    fl_set_object_color(obj, FL_RED, FL_RED);
  }

  obj = HALF;
  if (half != -1) {
    fl_set_object_label(obj, map[0][half].hlf);
    fl_set_object_color(obj, FL_WHEAT, FL_RED);
  }
  else {
    fl_set_object_label(obj, " ");
    fl_set_object_color(obj, FL_RED, FL_RED);
  }
}

//~============================================================================
//
//
//   Low Level Address Panel & LeCroy Bus Initialization
//
//
//~============================================================================

void create_lo_lvl_adr_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int size, x, y, w, h, w1 = 20, w2 = 30, w3 = 55, h1 = 15, h2 = 20;
  
  w = 5 + 5 * w1 + 5 + 2 * w2 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  size = FL_SMALL_SIZE;
  x = x0 + 5;
  y = y0 + 5;
  
  w = 5 * w1;
  obj = add_frame_box(x, y, w, h1, "Board Address", size);
  
  x = x + 5 + w;
  w = w2;
  obj = add_frame_box(x, y, w, h1, "Clk", size);
  
  x = x + w;
  w = w2;
  obj = add_frame_box(x, y, w, h1, "Dat", size);
  
  x = x + w + 5;
  obj = add_normal_button(x, y0, w3, 5+h1+h2+5, "INIT\nLeCroy", 'M');
    fl_set_object_callback(obj, INIT_callback, 0);

  size = FL_MEDIUM_SIZE;
  x = x0 + 5;
  y = y + h1;
  for (i=0; i<5; i++) {
    int j = 4 - i;
    BRD[j] = 
    obj = add_switch_button(x, y, w1, h2, " ", size);
      fl_set_object_callback(obj, brd_callback, j);
    x = x + w1;
  }
  
  x = x + 5;
  CLK = 
  obj = add_switch_button(x, y, w2, h2, " ", size);
    fl_set_object_callback(obj, clk_callback, 0);

  x = x + w2;
  DAT = 
  obj = add_switch_button(x, y, w2, h2, " ", size);
    fl_set_object_callback(obj, dat_callback, 0);
}

//~----------------------------------------------------------------------------

void brd_callback(FL_OBJECT *obj, long data) {

  int bit = data;
  int i, j;

  slot = -1;
  half = -1;
  
  brd ^= 1 << bit;

  for (i=0; i<6; i++) {
    for (j=0; j<2; j++) {
      if (clk == map[i][j].clk && 
          dat == map[i][j].dat &&
          brd == map[i][j].brd) {
        slot = i;
        half = j;
      }
    }
  }
  
  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void clk_callback(FL_OBJECT *obj, long data) {

  int i, j;

  slot = -1;
  half = -1;
  
  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  clk = MOD(clk+1, 8);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) clk = MOD(clk-1, 8);

#ifdef USE_AMSW
  dat = clk;
#endif

  for (i=0; i<6; i++) {
    for (j=0; j<2; j++) {
      if (clk==map[i][j].clk && dat==map[i][j].dat && brd==map[i][j].brd) {
        slot = i;
        half = j;
      }
    }
  }
    
  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void dat_callback(FL_OBJECT *obj, long data) {

  int i, j;

  slot = -1;
  half = -1;
  
#ifdef USE_CAN
  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  dat = MOD(dat+1, 16);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) dat = MOD(dat-1, 16);
#endif

#ifdef USE_AMSW
  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  dat = MOD(dat+1, 8);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) dat = MOD(dat-1, 8);
  clk = dat;
#endif

  for (i=0; i<6; i++) {
    for (j=0; j<2; j++) {
      if (clk==map[i][j].clk && dat==map[i][j].dat && brd==map[i][j].brd) {
        slot = i;
        half = j;
      }
    }
  }
    
  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void update_lo_lvl_adr_panel(void) {

  FL_OBJECT *obj;
  int i;
  char b[80];

  for (i=0; i<5; i++) {
    obj = BRD[i];
    if (brd & (1<<i)) fl_set_object_label(obj, "1");
    else              fl_set_object_label(obj, "0");
  }
  
  sprintf(b, "%d", clk);
  fl_set_object_label(CLK, b);

  sprintf(b, "%d", dat);
  fl_set_object_label(DAT, b);
}

//~----------------------------------------------------------------------------

void INIT_callback(FL_OBJECT *obj, long data) {

  int8 bus;
  
  msg = -1;
  update_objects();

  chk = 0;
  
#ifdef USE_CAN
  bus = (clk << 4) | dat;
#endif
#ifdef USE_AMSW
  bus = dat;
#endif
  init_LeCroy_bus(bus, &err);

  msg = 1;
  update_objects();
}

//~============================================================================
//
//
//   Read Panel
//
//
//~============================================================================

void create_READ_panel(int x0, int y0, int *W, int *H) {

  FL_OBJECT *obj;
  int i, j, from, to;
  int x, y, x1, x2, w, h, size;
  int w1 = 65, w2 = 15, w3 = 40, h1 = 20;

  w = 5 + w1 + 16 * w2 + 3 * 5 + w3 + 5;
  h = 5 + h1 + layout[ilay].nrea * 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h,"");
  
  *W = w;
  *H = h;

  size = FL_NORMAL_SIZE;

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Reg.", size);
  x = x + w1;
  w = 16 * w2 + 3 * 5;
  obj = add_frame_box(x, y, w, h1, "Reading", size);
  x = x + w;
  obj = add_frame_box(x, y, w3, h1, "Cmd.", size);
  
  for (j=0; j<layout[ilay].nrea; j++) {
  
    x = x0 + 5;
    y = y + h1;
    obj = add_frame_box(x, y, w1, 2*h1, layout[ilay].rea[j].title, size); 
    x1 = x0 + 5 + w1;

    for (i=0; i<16; i++) {
      if (!layout[ilay].rea[j].field[i].title) break;
      from = 15 - MAX(layout[ilay].rea[j].field[i].from,
                      layout[ilay].rea[j].field[i].to);
      to   = 15 - MIN(layout[ilay].rea[j].field[i].from,
                      layout[ilay].rea[j].field[i].to);
      x1 = x0 + 5 + w1 + from * w2 + (from / 4) * 5;
      x2 = x0 + 5 + w1 + to   * w2 + (to   / 4) * 5;
      w = x2 - x1 + w2;
      add_frame_box(x1, y, w, h1, layout[ilay].rea[j].field[i].title, size);
    }

    x = x0 + 5 + w1 + 16 * w2 + 3 * 5;
    obj = add_normal_button(x, y, w3, 2*h1, "GET", 'M');
      fl_set_object_callback(obj, GET_callback, j);
    y = y + h1;
    for (i=0; i<16; i++) {
      x = x0 + 5 + w1 + i * w2 + (i / 4) * 5;
      REA[j][i] =
      obj = add_data_box(x, y, w2, h1, "r", 'M');
    }
  }
}

//~----------------------------------------------------------------------------

void GET_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 reg = layout[ilay].rea[ind].reg;
  int32 m;
  int p;
  int8 bus;

  msg = -1;
  update_objects();

  wri = 0xA0000000;
  pack32(&wri, 0x0F800000, brd);
  pack32(&wri, 0x00070000, unpack16(reg, 0x0007));
  pack32(&wri, 0x00700000, unpack16(reg, 0x0038));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
  
  chk = 1;

#ifdef USE_CAN
  bus = (clk << 4) | dat;
#endif
#ifdef USE_AMSW
  bus = dat;
#endif
  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);

  if (!err && !chk) {
    if (rea_first[ind]) rea_first[ind] = FALSE;
    if (rea_sta[ind] == status_NN) rea_first[ind] = TRUE;
    rea_was[ind] = rea_cod[ind];
    rea_cod[ind] = unpack32(rea, 0x0000FFFF);
    rea_sta[ind] = status_OK;
  }
  else {
    if (rea_sta[ind] != status_NN) rea_sta[ind] = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_READ_panel(void) {

  int i, j;
  
  for (j=0; j<layout[ilay].nrea; j++) {
    int16 d = rea_was[j] ^ rea_cod[j];
    if (rea_first[j]) d = 0xFFFF;
    for (i=0; i<16; i++) {
      FL_OBJECT *obj = REA[j][i];
      int color;
      if (rea_sta[j] == status_NN) {
        fl_set_object_label(obj, "");
        color = FL_YELLOW;
      }
      else {
        if (rea_cod[j] & 0x8000>>i) fl_set_object_label(obj, "1");
        else                        fl_set_object_label(obj, "0");
        if (rea_sta[j] == status_KO) {
          color = FL_BLACK;
        }
        else {
          if (d & 0x8000>>i) color = FL_YELLOW;
          else               color = FL_GREEN;
        }
      }
      fl_set_object_color(obj, color, 0);
      if (color == FL_BLACK) fl_set_object_lcolor(obj, FL_WHITE);
      else                   fl_set_object_lcolor(obj, FL_BLACK);
    }
  }
}

//~============================================================================
//
//
//   Write Panel
//
//
//~============================================================================

void create_WRITE_panel(int x0, int y0, int *W, int *H) {

  FL_OBJECT *obj;
  int i, j, from, to;
  int x, y, x1, x2, w, h, size;
  int w1 = 65, w2 = 15, w3 = 40, h1 = 20;

  w = 5 + w1 + 16 * w2 + 3 * 5 + w3 + 5;
  h = 5 + h1 + layout[ilay].nwri * 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h,"");
  
  *W = w;
  *H = h;

  size = FL_NORMAL_SIZE;

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Reg.", size);
  x = x + w1;
  w = 16 * w2 + 3 * 5;
  obj = add_frame_box(x, y, w, h1, "Writing", size);
  x = x + w;
  obj = add_frame_box(x, y, w3, h1, "Cmd.", size);
  
  for (j=0; j<layout[ilay].nwri; j++) {
  
    x = x0 + 5;
    y = y + h1;
    obj = add_frame_box(x, y, w1, 2*h1, layout[ilay].wri[j].title, size); 
    x1 = x0 + 5 + w1;

    for (i=0; i<16; i++) {
      if (!layout[ilay].wri[j].field[i].title) break;
      from = 15 - MAX(layout[ilay].wri[j].field[i].from,
                      layout[ilay].wri[j].field[i].to);
      to   = 15 - MIN(layout[ilay].wri[j].field[i].from,
                      layout[ilay].wri[j].field[i].to);
      x1 = x0 + 5 + w1 + from * w2 + (from / 4) * 5;
      x2 = x0 + 5 + w1 + to   * w2 + (to   / 4) * 5;
      w = x2 - x1 + w2;
      add_frame_box(x1, y, w, h1, layout[ilay].wri[j].field[i].title, size);
    }

    x = x0 + 5 + w1 + 16 * w2 + 3 * 5;
    obj = add_normal_button(x, y, w3, 2*h1, "SET", 'M');
      fl_set_object_callback(obj, SET_callback, j);
    y = y + h1;
    for (i=0; i<16; i++) {
      x = x0 + 5 + w1 + i * w2 + (i / 4) * 5;
      WRI[j][i] =
      obj = add_switch_button(x, y, w2, h1, "r", 'M');
        fl_set_object_callback(obj, WRI_callback, 16*j+i);
    }
  }
}

//~----------------------------------------------------------------------------

void WRI_callback(FL_OBJECT *obj, long data) {

  int ind = data / 16;
  int bit = data % 16;
  
  wri_cod[ind] ^= 0x8000 >> bit;
  
  update_objects();
}

//~----------------------------------------------------------------------------

void SET_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 reg = layout[ilay].wri[ind].reg;
  int32 m;
  int p;
  int8 bus;

  msg = -1;
  update_objects();

  wri = 0xA0080000;
  pack32(&wri, 0x0F800000, brd);
  pack32(&wri, 0x00070000, unpack16(reg, 0x0007));
  pack32(&wri, 0x00700000, unpack16(reg, 0x0038));
  pack32(&wri, 0x0000FFFF, wri_cod[ind]);
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
  
  chk = 1;

#ifdef USE_CAN
  bus = (clk << 4) | dat;
#endif

#ifdef USE_AMSW
  bus = dat;
#endif

  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);

  wri_sta[ind] = status_OK;
  wri_was[ind] = wri_cod[ind];
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_WRITE_panel(void) {

  int i, j;
  
  for (j=0; j<layout[ilay].nwri; j++) {
    int16 d = wri_was[j] ^ wri_cod[j];
    for (i=0; i<16; i++) {
      FL_OBJECT *obj = WRI[j][i];
      int color;
      if (wri_cod[j] & 0x8000>>i) fl_set_object_label(obj, "1");
      else                        fl_set_object_label(obj, "0");
      if (wri_sta[j] == status_NN) {
        color = FL_YELLOW;
      }
      else {
        if (d & 0x8000>>i) color = FL_YELLOW;
        else               color = FL_GREEN;
      }
      fl_set_object_color(obj, color, FL_RED);
    }
  }
}

//~============================================================================
//
//
//   ADC for TBS (Sandor and Co)
//
//
//~============================================================================

void create_ADC_panel(int x0, int y0, int ind, int *W, int *H) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 15, h1 = 20;

  w = 5 + 8 * (3 * w1) + 5;
  h = 5 + 5 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  *W = w;
  *H = h;

  x = x0 + 5;
  y = y0 + 5;
  w = 8 * (3 * w1);
  if (ind == 0) obj = add_frame_box(x, y, w, h1, "ADC 0", 'S');
  else          obj = add_frame_box(x, y, w, h1, "ADC 1", 'S');

  x = x0 + 5;
  y = y + h1;
  w = 3 * w1;
  for (i=8*ind; i<8*ind+8; i++) {
    char b[80];
    int reg = 0x10 + i;
    sprintf(b, "0x%02X", reg);
    obj = add_frame_box(     x, y,      w, h1, b,      'S');
    ADC[i] = 
    obj = add_data_box(      x, y+h1,   w, h1, "1234", 'M');
    obj = add_normal_button( x, y+2*h1, w, h1, "GET",  'N');
      fl_set_object_callback(obj, ADC_callback, i);
    x = x + w;
  }

  x = x0 + 5;
  y = y + 3 * h1;
  w = 8 * (3 * w1);
  obj = add_normal_button( x, y, w, h1, "GET",  'N');
    fl_set_object_callback(obj, ADC_ADC_callback, ind);
}

//~----------------------------------------------------------------------------

void ADC_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 reg = 0x10 + ind;
  int32 m;
  int p;
  int8 bus;

  msg = -1;
  update_objects();

  wri = 0xA0000000;
  pack32(&wri, 0x0F800000, brd);
  pack32(&wri, 0x00070000, unpack16(reg, 0x0007));
  pack32(&wri, 0x00700000, unpack16(reg, 0x0038));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
  
  chk = 1;

#ifdef USE_CAN
  bus = (clk << 4) | dat;
#endif

#ifdef USE_AMSW
  bus = dat;
#endif

  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);

  if (!err && !chk) {
    if (ADC_first[ind]) ADC_first[ind] = FALSE;
    if (ADC_sta[ind] == status_NN) ADC_first[ind] = TRUE;
    ADC_was[ind] = ADC_cod[ind];
    ADC_cod[ind] = unpack32(rea, 0x0000FFFF);
    ADC_sta[ind] = status_OK;
  }
  else {
    if (ADC_sta[ind] != status_NN) ADC_sta[ind] = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void ADC_ADC_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;

  msg = -1;
  update_objects();

  for (i=8*ind; i<8*ind+8; i++) ADC_callback(NULL, i);
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_ADC_panels(void) {

  int i;
  
  for (i=0; i<16; i++) {
    FL_OBJECT *obj = ADC[i];
    int color;
    if (ADC_sta[i] == status_NN) {
      fl_set_object_label(obj, "");
      color = FL_YELLOW;
    }
    else {
      char b[80];
      sprintf(b, "%04X", ADC_cod[i]);
      fl_set_object_label( obj, b);
      if (ADC_sta[i] == status_KO) {
        color = FL_BLACK;
      }
      else {
        if (ADC_was[i] != ADC_cod[i]) color = FL_YELLOW;
        else                          color = FL_GREEN;
      }
    }
    fl_set_object_color(obj, color, 0);
    if (color == FL_BLACK) fl_set_object_lcolor(obj, FL_WHITE);
    else                   fl_set_object_lcolor(obj, FL_BLACK);
  }
}

//~============================================================================
//
//
//   ADC for TBS (VK)
//
//
//~============================================================================

void create_ADC_VK_panel(int x0, int y0, int ind, int *W, int *H) {

  FL_OBJECT *obj;
  int i, n;
  int x, y, w, h;
  int w1 = 45, h1 = 20, h2 = 15;

  n = ind ? 4 : 12;

  w = 5 + n * w1 + 5;
  h = 5 + 3 * h1 + h2 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  *W = w;
  *H = h;

  x = x0 + 5;
  y = y0 + 5;
  if (ind == 0) add_frame_box(x, y, n*w1, h1, "ADC: Bias Current", 'N');
  else          add_frame_box(x, y, n*w1, h1, "ADC: Bias Voltage", 'N');

  x = x0 + 5;
  y = y + h1;
  for (i=0; i<n; i++) {
    char b[80];
    int j = 15 - (12 * ind + i);
    int reg = j + 0x10;
    int cha = n - 1 - i;
    sprintf(b, "%d", cha);
    add_frame_box(x, y, w1, h1, b, 'N');
    ADC[j] = 
    add_data_box(x, y+h1, w1, h1, "1234", 'M');
    sprintf(b, "0x%02X", reg);
    add_frame_box(x, y+2*h1, w1, h2, b, 'S');
    obj = add_normal_button( x, y+2*h1+h2, w1, h1, "GET",  'N');
      fl_set_object_callback(obj, ADC_VK_callback, j);
    x = x + w1;
  }

  x = x0 + 5;
  y = y + 3 * h1 + h2;
  obj = add_normal_button( x, y, n*w1, h1, "GET",  'N');
    fl_set_object_callback(obj, ADC_ADC_VK_callback, ind);
}

//~----------------------------------------------------------------------------

void ADC_VK_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 reg = 0x10 + ind;
  int32 m;
  int p;
  int8 bus;

  msg = -1;
  update_objects();

  wri = 0xA0000000;
  pack32(&wri, 0x0F800000, brd);
  pack32(&wri, 0x00070000, unpack16(reg, 0x0007));
  pack32(&wri, 0x00700000, unpack16(reg, 0x0038));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
  
  chk = 1;

#ifdef USE_CAN
  bus = (clk << 4) | dat;
#endif

#ifdef USE_AMSW
  bus = dat;
#endif

  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);

  if (!err && !chk) {
    if (ADC_first[ind]) ADC_first[ind] = FALSE;
    if (ADC_sta[ind] == status_NN) ADC_first[ind] = TRUE;
    ADC_was[ind] = ADC_cod[ind];
    ADC_cod[ind] = unpack32(rea, 0x0000FFFF);
    ADC_sta[ind] = status_OK;
  }
  else {
    if (ADC_sta[ind] != status_NN) ADC_sta[ind] = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void ADC_ADC_VK_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i, i1, i2;

  msg = -1;
  update_objects();

  if (ind == 0) {
    i1 = 4;
    i2 = 16;
  }
  else {
    i1 = 0;
    i2 = 4;
  }
  
  for (i=i1; i<i2; i++) ADC_VK_callback(NULL, i);
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_ADC_VK_panels(void) {

  int i;
  
  for (i=0; i<16; i++) {
    FL_OBJECT *obj = ADC[i];
    int color;
    if (ADC_sta[i] == status_NN) {
      fl_set_object_label(obj, "");
      color = FL_YELLOW;
    }
    else {
      char b[80];
      sprintf(b, "%04X", ADC_cod[i]);
      fl_set_object_label( obj, b);
      if (ADC_sta[i] == status_KO) {
        color = FL_BLACK;
      }
      else {
        if (ADC_was[i] != ADC_cod[i]) color = FL_YELLOW;
        else                          color = FL_GREEN;
      }
    }
    fl_set_object_color(obj, color, 0);
    if (color == FL_BLACK) fl_set_object_lcolor(obj, FL_WHITE);
    else                   fl_set_object_lcolor(obj, FL_BLACK);
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);
  update_hi_lvl_adr_panel();
  update_lo_lvl_adr_panel();
  update_WRITE_panel();
  update_READ_panel();
  if (show_ADC) {
    if (show_ADC_VK) update_ADC_VK_panels();
    else             update_ADC_panels();
  }
  
//write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  return 0;
}

//~============================================================================
