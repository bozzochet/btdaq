// this is file cnt.c
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
// - functions for "COUNTERS" (TOF,ACC,ATC) HV & TEMP data monitoring
//

#include "pgsc.h"               // it has some global data definitions

//------------------------------------------------------------------------------

static int CNT_node_number[8] = {  0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7};

char *SMC_name[8] = { 
"SMC01", "SMC31", "SMC41", "SMC71", 
"SMC03", "SMC33", "SMC43", "SMC73"};

// - - - - - - - - - - - - - - - - - TOF - - - - - - - - - - - - - - - - - - - -

static int TOF_index[8]       = {    0,    1,    2,    3,    4,    5,    6,    7};

struct {
  char name[7];
  struct {
    char name[8];
    struct {
      int HV_index;
      float A, B;
      int HV_default;
      int HV_there;
      int HV_here;
    } PMT[3];
  } channel[14];
} TOF_crate[8] =

// what follows was prepared by TOF_commander_preparation.c program with Luca's input

{
"01", {"TF1.01P", {{20, -2.5800, 950.8, 0x44}, {19, -2.6714, 942.4, 0x2A}, {18, -2.6934, 947.7, 0x3A}},
       "TF1.02P", {{17, -2.6074, 950.6, 0x31}, {16, -2.6390, 948.6, 0x30}, {15, -2.6214, 949.9, 0x2E}},
       "TF1.03P", {{14, -2.7195, 946.0, 0x44}, {13, -2.7195, 948.1, 0x32}, {12, -2.6714, 948.3, 0x3E}},
       "TF1.04P", {{11, -2.7083, 948.3, 0x2C}, {10, -2.7385, 948.3, 0x2C}, { 9, -2.6569, 949.9, 0x26}},
       "TF1.05P", {{ 8, -2.6750, 948.8, 0x33}, { 7, -2.6823, 947.0, 0x37}, { 6, -2.6714, 948.3, 0x06}},
       "TF1.06P", {{ 5, -2.7083, 947.3, 0x2B}, { 4, -2.6497, 950.2, 0x35}, { 3, -2.7309, 948.0, 0x2B}},
       "TF1.07P", {{ 2, -2.7158, 948.1, 0x28}, { 1, -2.6714, 948.8, 0x31}, { 0, -2.6569, 949.4, 0x2F}},
       "TF2.01P", {{39, -2.7271, 836.8, 0x23}, {40, -2.7424, 843.4, 0x2A}, {41, -2.7385, 849.3, 0x24}},
       "TF2.02P", {{36, -2.7424, 833.6, 0x0D}, {37, -2.7083, 841.6, 0x21}, {38, -2.7271, 846.6, 0x19}},
       "TF2.03P", {{33, -2.7424, 836.9, 0x23}, {34, -2.7385, 842.7, 0x1A}, {35, -2.7195, 827.3, 0x15}},
       "TF2.04P", {{30, -2.7120, 841.8, 0x1C}, {31, -2.7578, 851.7, 0x1E}, {32, -2.7008, 855.0, 0x20}},
       "TF2.05P", {{27, -2.7271, 845.0, 0x18}, {28, -2.7695, 841.4, 0x15}, {29, -2.7008, 835.4, 0x1F}},
       "TF2.06P", {{24, -2.7500, 847.0, 0x20}, {25, -2.7309, 838.1, 0x15}, {26, -2.7271, 848.3, 0x27}},
       "TF2.07P", {{21, -2.7578, 843.0, 0x1C}, {22, -2.7695, 856.3, 0x25}, {23, -2.7424, 841.9, 0x20}}},
"31", {"TF1.01N", {{20, -2.7462, 898.7, 0x24}, {19, -2.7083, 905.9, 0x27}, {18, -2.6897, 906.3, 0x22}},
       "TF1.02N", {{17, -2.7734, 905.7, 0x21}, {16, -2.7578, 905.2, 0x28}, {15, -2.5902, 907.6, 0x2A}},
       "TF1.03N", {{14, -2.7008, 906.6, 0x26}, {13, -2.6569, 905.4, 0x28}, {12, -2.7462, 900.9, 0x1C}},
       "TF1.04N", {{11, -2.7195, 906.2, 0x1B}, {10, -2.7617, 905.2, 0x25}, { 9, -2.6860, 892.3, 0x1A}},
       "TF1.05N", {{ 8, -2.6934, 906.3, 0x22}, { 7, -2.7462, 904.1, 0x26}, { 6, -2.6677, 906.1, 0x28}},
       "TF1.06N", {{ 5, -2.6934, 905.8, 0x24}, { 4, -2.8011, 891.4, 0x1F}, { 3, -2.6934, 907.3, 0x12}},
       "TF1.07N", {{ 2, -2.6677, 905.4, 0x20}, { 1, -2.7309, 900.2, 0x26}, { 0, -2.6823, 906.1, 0x28}},
       "TF2.08P", {{23, -2.7695, 852.8, 0x1F}, {22, -2.7271, 837.4, 0x13}, {21, -2.7385, 841.6, 0x1C}},
       "TF2.09P", {{26, -2.7617, 840.4, 0x16}, {25, -2.7195, 844.7, 0x1C}, {24, -2.7309, 843.5, 0x17}},
       "TF2.10P", {{29, -2.7120, 833.1, 0x1B}, {28, -2.7120, 837.9, 0x20}, {27, -2.7813, 846.2, 0x14}},
       "TF2.11P", {{32, -2.7385, 844.9, 0x1B}, {31, -2.7500, 849.2, 0x24}, {30, -2.6714, 834.6, 0x1C}},
       "TF2.12P", {{35, -2.7271, 840.6, 0x1E}, {34, -2.6971, 840.7, 0x1F}, {33, -2.7424, 840.2, 0x1E}},
       "TF2.13P", {{38, -2.7424, 850.6, 0x2A}, {37, -2.7424, 836.9, 0x1A}, {36, -2.7385, 830.7, 0x1B}},
       "TF2.14P", {{41, -2.7195, 833.8, 0x19}, {40, -2.7309, 842.4, 0x27}, {39, -2.7309, 834.8, 0x21}}},
"41", {"TF1.08N", {{ 0, -2.7734, 872.0, 0x11}, { 1, -2.7008, 880.0, 0x17}, { 2, -2.7271, 888.5, 0x1A}},
       "TF1.09N", {{ 3, -2.7120, 878.8, 0x15}, { 4, -2.7734, 880.2, 0x20}, { 5, -2.7120, 874.4, 0x0C}},
       "TF1.10N", {{ 6, -2.6897, 872.0, 0x23}, { 7, -2.7931, 892.3, 0x22}, { 8, -2.6934, 868.3, 0x21}},
       "TF1.11N", {{ 9, -2.7578, 881.1, 0x26}, {10, -2.7617, 881.8, 0x20}, {11, -2.7539, 874.9, 0x14}},
       "TF1.12N", {{12, -2.7120, 869.5, 0x1C}, {13, -2.7500, 885.1, 0x2C}, {14, -2.7309, 873.5, 0x1A}},
       "TF1.13N", {{15, -2.7539, 885.8, 0x1C}, {16, -2.6897, 877.0, 0x1E}, {17, -2.7385, 876.9, 0x20}},
       "TF1.14N", {{18, -2.6971, 873.4, 0x23}, {19, -2.7195, 877.4, 0x1D}, {20, -2.7195, 869.7, 0x1A}},
       "TF2.08N", {{23, -2.7500, 854.6, 0x20}, {22, -2.7695, 842.3, 0x17}, {21, -2.7158, 841.3, 0x1C}},
       "TF2.09N", {{26, -2.7271, 841.7, 0x13}, {25, -2.7617, 863.3, 0x30}, {24, -2.7617, 847.8, 0x24}},
       "TF2.10N", {{29, -2.7695, 846.2, 0x14}, {28, -2.7931, 857.2, 0x14}, {27, -2.7813, 857.1, 0x23}},
       "TF2.11N", {{32, -2.7500, 847.0, 0x10}, {31, -2.7120, 844.0, 0x21}, {30, -2.7500, 856.8, 0x20}},
       "TF2.12N", {{35, -2.7158, 848.7, 0x1D}, {34, -2.7309, 834.8, 0x18}, {33, -2.7734, 855.7, 0x1B}},
       "TF2.13N", {{38, -2.7385, 851.4, 0x22}, {37, -2.7539, 861.8, 0x29}, {36, -2.7347, 854.3, 0x26}},
       "TF2.14N", {{41, -2.7695, 838.6, 0x23}, {40, -2.7195, 848.0, 0x27}, {39, -2.6787, 846.1, 0x13}}},
"71", {"TF1.08P", {{ 0, -2.7195, 881.9, 0x1A}, { 1, -2.7695, 887.7, 0x1F}, { 2, -2.7462, 880.2, 0x15}},
       "TF1.09P", {{ 3, -2.7385, 884.2, 0x19}, { 4, -2.7385, 881.0, 0x1B}, { 5, -2.7617, 896.0, 0x24}},
       "TF1.10P", {{ 6, -2.7617, 883.6, 0x1D}, { 7, -2.7500, 889.6, 0x22}, { 8, -2.7500, 874.3, 0x19}},
       "TF1.11P", {{ 9, -2.7158, 878.6, 0x0E}, {10, -2.7271, 876.7, 0x1B}, {11, -2.7500, 895.0, 0x2A}},
       "TF1.12P", {{12, -2.8253, 879.7, 0x1C}, {13, -2.7008, 875.2, 0x1C}, {14, -2.7462, 878.0, 0x18}},
       "TF1.13P", {{15, -2.7195, 896.0, 0x2D}, {16, -2.7158, 874.7, 0x1E}, {17, -2.7385, 866.8, 0x22}},
       "TF1.14P", {{18, -2.7462, 864.3, 0x19}, {19, -2.7083, 876.6, 0x21}, {20, -2.7120, 876.9, 0x24}},
       "TF2.01N", {{39, -2.7424, 844.0, 0x23}, {40, -2.7271, 840.9, 0x20}, {41, -2.6971, 846.1, 0x22}},
       "TF2.02N", {{36, -2.6971, 842.9, 0x1C}, {37, -2.7083, 841.6, 0x1A}, {38, -2.7271, 846.1, 0x18}},
       "TF2.03N", {{33, -2.6787, 839.6, 0x24}, {34, -2.7271, 848.7, 0x17}, {35, -2.7539, 850.1, 0x1F}},
       "TF2.04N", {{30, -2.7195, 841.4, 0x20}, {31, -2.7309, 851.7, 0x20}, {32, -2.7271, 842.2, 0x1D}},
       "TF2.05N", {{27, -2.7158, 843.2, 0x19}, {28, -2.7008, 850.0, 0x24}, {29, -2.7656, 852.0, 0x19}},
       "TF2.06N", {{24, -2.7424, 848.4, 0x18}, {25, -2.7271, 846.9, 0x23}, {26, -2.7695, 841.9, 0x20}},
       "TF2.07N", {{21, -2.7195, 843.6, 0x1E}, {22, -2.7578, 851.4, 0x1D}, {23, -2.6823, 842.7, 0x21}}},
"03", {"TF3.01P", {{18, -2.7500, 875.3, 0x20}, {19, -2.7462, 887.9, 0x2B}, {20, -2.7120, 881.2, 0x23}},
       "TF3.02P", {{15, -2.7617, 885.0, 0x23}, {16, -2.7271, 885.3, 0x23}, {17, -2.7695, 886.5, 0x22}},
       "TF3.03P", {{12, -2.7500, 872.3, 0x1A}, {13, -2.7500, 884.0, 0x21}, {14, -2.6355, 869.7, 0x1D}},
       "TF3.04P", {{ 9, -2.7462, 891.2, 0x21}, {10, -2.7578, 883.3, 0x27}, {11, -2.7309, 880.0, 0x1D}},
       "TF3.05P", {{ 6, -2.7195, 872.8, 0x19}, { 7, -2.6971, 867.1, 0x24}, { 8, -2.7734, 876.3, 0x1A}},
       "TF3.06P", {{ 3, -2.7008, 881.6, 0x26}, { 4, -2.7083, 884.0, 0x19}, { 5, -2.8213, 875.0, 0x1D}},
       "TF3.07P", {{ 0, -2.7734, 878.5, 0x22}, { 1, -2.7813, 882.2, 0x1A}, { 2, -2.7500, 876.4, 0x1B}},
       "TF4.01P", {{39, -2.6971, 811.5, 0x15}, {40, -2.7309, 815.9, 0x13}, {41, -2.7233, 822.4, 0x1E}},
       "TF4.02P", {{36, -2.7083, 821.0, 0x1E}, {37, -2.7578, 825.0, 0x19}, {38, -2.7120, 833.7, 0x1C}},
       "TF4.03P", {{33, -2.7813, 823.1, 0x1C}, {34, -2.5465, 812.8, 0x18}, {35, -2.7424, 808.5, 0x03}},
       "TF4.04P", {{30, -2.7462, 817.3, 0x1C}, {31, -2.7195, 820.7, 0x20}, {32, -2.7309, 811.9, 0x16}},
       "TF4.05P", {{27, -2.7195, 818.7, 0x16}, {28, -2.7120, 814.3, 0x17}, {29, -2.7813, 824.1, 0x1C}},
       "TF4.06P", {{24, -2.7347, 821.2, 0x17}, {25, -2.7195, 816.3, 0x23}, {26, -2.7734, 834.1, 0x10}},
       "TF4.07P", {{21, -2.7083, 822.1, 0x17}, {22, -2.7462, 835.3, 0x21}, {23, -2.7424, 815.6, 0x11}}},
"33", {"TF3.08P", {{ 2, -2.7539, 877.1, 0x25}, { 1, -2.7008, 875.1, 0x25}, { 0, -2.7271, 870.0, 0x16}},
       "TF3.09P", {{ 5, -2.7385, 876.5, 0x21}, { 4, -2.7309, 871.8, 0x27}, { 3, -2.6823, 871.3, 0x22}},
       "TF3.10P", {{ 8, -2.7309, 876.7, 0x26}, { 7, -2.7385, 867.8, 0x1C}, { 6, -2.6897, 875.3, 0x26}},
       "TF3.11P", {{11, -2.7500, 879.6, 0x26}, {10, -2.7309, 881.6, 0x2E}, { 9, -2.7500, 885.1, 0x2F}},
       "TF3.12P", {{14, -2.7500, 890.5, 0x2E}, {13, -2.7271, 874.7, 0x2A}, {12, -2.7734, 869.3, 0x29}},
       "TF3.13P", {{17, -2.6787, 859.2, 0x25}, {16, -2.7309, 878.3, 0x1F}, {15, -2.6787, 888.7, 0x2E}},
       "TF3.14P", {{20, -2.7617, 883.5, 0x2D}, {19, -2.7008, 882.7, 0x35}, {18, -2.7271, 888.5, 0x33}},
       "TF4.01N", {{39, -2.7424, 812.6, 0x11}, {40, -2.6641, 804.8, 0x1C}, {41, -2.6971, 806.5, 0x20}},
       "TF4.02N", {{36, -2.7695, 823.8, 0x17}, {37, -2.7309, 809.9, 0x14}, {38, -2.7309, 804.9, 0x11}},
       "TF4.03N", {{33, -2.7578, 819.6, 0x16}, {34, -2.7424, 823.0, 0x20}, {35, -2.7539, 816.2, 0x12}},
       "TF4.04N", {{30, -2.7195, 815.1, 0x1F}, {31, -2.7309, 817.9, 0x1E}, {32, -2.7462, 818.8, 0x15}},
       "TF4.05N", {{27, -2.7424, 815.6, 0x14}, {28, -2.6355, 802.1, 0x1A}, {29, -2.6971, 817.5, 0x18}},
       "TF4.06N", {{24, -2.7424, 810.0, 0x1D}, {25, -2.7309, 821.4, 0x17}, {26, -2.6897, 813.1, 0x16}},
       "TF4.07N", {{21, -2.7578, 813.1, 0x20}, {22, -2.7578, 824.6, 0x1D}, {23, -2.7233, 814.0, 0x15}}},
"43", {"TF3.08N", {{ 2, -2.7813, 878.9, 0x27}, { 1, -2.7309, 871.8, 0x25}, { 0, -2.7385, 875.4, 0x1A}},
       "TF3.09N", {{ 5, -2.7385, 886.3, 0x21}, { 4, -2.7695, 876.2, 0x27}, { 3, -2.6971, 870.1, 0x22}},
       "TF3.10N", {{ 8, -2.7195, 875.8, 0x1C}, { 7, -2.7617, 872.6, 0x25}, { 6, -3.0073, 924.2, 0x26}},
       "TF3.11N", {{11, -2.7424, 881.0, 0x28}, {10, -2.7385, 878.6, 0x21}, { 9, -2.8213, 868.5, 0x1F}},
       "TF3.12N", {{14, -2.7500, 877.5, 0x24}, {13, -2.7271, 868.9, 0x16}, {12, -2.6787, 865.7, 0x1E}},
       "TF3.13N", {{17, -2.7385, 887.3, 0x20}, {16, -2.7500, 881.8, 0x25}, {15, -2.7424, 883.7, 0x19}},
       "TF3.14N", {{20, -2.7813, 881.1, 0x25}, {19, -2.7578, 866.9, 0x2A}, {18, -2.6641, 883.3, 0x32}},
       "TF4.08N", {{23, -2.7695, 818.8, 0x1B}, {22, -2.7578, 811.1, 0x16}, {21, -2.7931, 814.9, 0x1D}},
       "TF4.09N", {{26, -2.7852, 821.4, 0x1C}, {25, -2.7656, 811.0, 0x1B}, {24, -2.7424, 813.0, 0x1A}},
       "TF4.10N", {{29, -2.7734, 819.6, 0x1C}, {28, -2.7539, 818.2, 0x15}, {27, -2.7656, 821.0, 0x12}},
       "TF4.11N", {{32, -2.7424, 819.0, 0x19}, {31, -2.7578, 816.6, 0x1E}, {30, -2.7120, 813.3, 0x10}},
       "TF4.12N", {{35, -2.7462, 822.3, 0x15}, {34, -2.7195, 811.7, 0x1E}, {33, -2.7309, 812.9, 0x0C}},
       "TF4.13N", {{38, -2.7578, 816.6, 0x19}, {37, -2.7233, 808.0, 0x1F}, {36, -2.7578, 809.6, 0x19}},
       "TF4.14N", {{41, -2.7695, 826.8, 0x0D}, {40, -2.7120, 798.3, 0x19}, {39, -2.7462, 816.3, 0x18}}},
"73", {"TF3.01N", {{18, -2.7309, 886.2, 0x24}, {19, -2.7500, 885.8, 0x23}, {20, -2.6860, 875.7, 0x2B}},
       "TF3.02N", {{15, -2.7385, 889.8, 0x29}, {16, -2.6249, 881.3, 0x1A}, {17, -2.7309, 878.6, 0x23}},
       "TF3.03N", {{12, -2.7734, 898.3, 0x2B}, {13, -2.7695, 893.8, 0x30}, {14, -2.7120, 878.5, 0x19}},
       "TF3.04N", {{ 9, -2.7385, 888.1, 0x23}, {10, -2.7309, 898.2, 0x26}, {11, -2.7852, 895.0, 0x25}},
       "TF3.05N", {{ 6, -2.6533, 874.7, 0x24}, { 7, -2.7695, 895.9, 0x31}, { 8, -2.7500, 878.8, 0x19}},
       "TF3.06N", {{ 3, -2.8051, 896.6, 0x2A}, { 4, -2.7195, 889.6, 0x26}, { 5, -2.7120, 882.8, 0x2A}},
       "TF3.07N", {{ 0, -2.7500, 886.4, 0x22}, { 1, -2.7617, 894.0, 0x2E}, { 2, -2.6823, 878.6, 0x2A}},
       "TF4.08P", {{23, -2.7539, 825.2, 0x26}, {22, -2.7424, 835.0, 0x1F}, {21, -2.7971, 831.7, 0x26}},
       "TF4.09P", {{26, -2.7617, 816.3, 0x1C}, {25, -2.7347, 819.2, 0x1B}, {24, -2.7813, 815.6, 0x20}},
       "TF4.10P", {{29, -2.7309, 816.9, 0x1C}, {28, -2.6533, 812.3, 0x23}, {27, -2.7424, 827.0, 0x22}},
       "TF4.11P", {{32, -2.7309, 835.9, 0x27}, {31, -2.7813, 825.6, 0x2A}, {30, -2.7617, 831.3, 0x2A}},
       "TF4.12P", {{35, -2.7309, 823.9, 0x1F}, {34, -2.7539, 827.8, 0x20}, {33, -2.6569, 827.5, 0x1F}},
       "TF4.13P", {{38, -2.7578, 813.6, 0x1A}, {37, -2.7083, 821.6, 0x26}, {36, -2.7233, 835.4, 0x27}},
       "TF4.14P", {{41, -2.7539, 818.2, 0x28}, {40, -2.7347, 813.7, 0x2F}, {39, -2.7813, 822.6, 0x0C}}}
};
// - - - - - - - - - - - - - - - - - ACC - - - - - - - - - - - - - - - - - - - -

//   crate number       =     01    31    41    71    03    33    43    73
//   node number        =      0     1     2     3     4     5     6     7
static int ACC_index[8] = {   -1,    1,   -1,    0,   -1,   -1,   -1,   -1};

int ACC_adr[2][4] = {
  0x78, 0x7C, 0x7A, 0x7E,	// crate 71, node_number 3
  0x7A, 0x7E, 0x78, 0x7C	// crate 31, node_number 1
};

char *ACC_name[2][2] = {
  "01P 02P 03P 04P 05P 06P 07P 08P", 
  "09P 10P 11P 12P 13P 14P 15P 16P",
  "01N 02N 03N 04N 05N 06N 07N 08N",
  "09N 10N 11N 12N 13N 14N 15N 16N"
};

struct {
  int DAC_1;
  int HV_1[8];
  int DAC_2;
  int HV_2[8];
} ACC_db[2][4] = { 
     0xC2 , {  3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3},
     0x00 , {902 , 902 , 884 , 869 , 902 , 884 , 902 , 869},
     0xC2 , {  3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3},
     0x00 , {903 , 903 , 885 , 870 , 903 , 885 , 903 , 870},
     0xC2 , {  3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3},
     0x00 , {867 , 882 , 900 , 900 , 900 , 867 , 882 , 900},
     0xC2 , {  3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3},
     0x00 , {866 , 881 , 899 , 899 , 899 , 866 , 881 , 899},

     0xC2 , {  3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3},
     0x00 , {869 , 884 , 902 , 902 , 902 , 902 , 869 , 884},
     0xC2 , {  3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3},
     0x00 , {859 , 874 , 892 , 892 , 892 , 892 , 859 , 874},
     0xC2 , {  3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3},
     0x00 , {897 , 879 , 864 , 897 , 897 , 897 , 879 , 864},
     0xC2 , {  3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3 ,   3},
     0x00 , {898 , 880 , 865 , 898 , 898 , 898 , 880 , 865}};
     
float C[2][4][8];
  
// - - - - - - - - - - - - - - - - - ATC - - - - - - - - - - - - - - - - - - - -

static int ATC_index[8]       = {    0,   -1,    2,   -1,    4,    5,    6,    7};

int nb_of_ATC_mods[2] = { 6, 5};

struct {
  char *name;
  int SMC_node_nb_0;
  int HV_adr_0;
  int SMC_node_nb_1;
  int HV_adr_1;
} ATC[2][6] = {

   "L1", 6, 0x9E, 5, 0x9E,
   "L2", 6, 0x9C, 5, 0x9C,
   "L3", 6, 0x7E, 4, 0x7E,
   "L4", 6, 0x7A, 4, 0x7A,
   "L5", 7, 0x9E, 4, 0x9E,
   "L6", 7, 0x9C, 4, 0x9C,

   "U1", 2, 0x9E, 5, 0x7E,
   "U2", 2, 0x9C, 5, 0x7A,
   "U3", 2, 0x7A, 0, 0x7A,
   "U4", 7, 0x7E, 0, 0x9E,
   "U5", 7, 0x7A, 0, 0x9C};

//------------------------------------------------------------------------------


void simulate_CNT_block( void) {

  int ii, j, k, block_index;   static long q = 0;
  int i, temp;
  
  ii = rand() % 8;                          /* CNT crate number */
  q++;  if (q > 20) q = 0; if (q > 10 && ii == 2) return;
//AMS_block_ID = block_type<<13 | node_type<<9 | node_number<<6 | data_type;
  AMS_block_ID =        0x4<<13 |       0xA<<9 |          ii<<6 |         0;
  block_index = 0;

/*********************************************/
/*                                           */
/*          simulate CNT state data          */
/*                                           */
/*********************************************/

  for (j=0; j<7; j++) {
    AMS_block_body[block_index++] = (rand() % 2) ?  0 : 1;    // some status
    AMS_block_body[block_index++] = (rand() % 3) ? 22 : 5;    // some parameter
  }

/*********************************************/
/*                                           */
/*           simulate temperature data       */
/*                                           */
/*********************************************/
                                       
  for (j=0; j<7; j++) {
    for (k=0; k<2; k++) {
      AMS_block_body[block_index++] = 0x7f + (rand() % 0x7f); // temperature
    }
  }

  AMS_block_length = block_index + 1;
  
} /* end of simulate_CNT_block() */

//------------------------------------------------------------------------------

void initialize_CNT_page( void) {

  static short first = TRUE;
  int Cr, Ch, PM;
     
  if (first) {
    for (Cr=0; Cr<2; Cr++) {
      for (Ch=0; Ch<4; Ch++) {
        for (PM=0; PM<8; PM++) {
          C[Cr][Ch][PM] = 
                (float) (ACC_db[Cr][Ch].HV_2[PM]  - ACC_db[Cr][Ch].HV_1[PM]) /
                (float) (ACC_db[Cr][Ch].DAC_2     - ACC_db[Cr][Ch].DAC_1   );
        }
      }
    }
    first = FALSE;
  }
}

//------------------------------------------------------------------------------

void process_CNT_block( void) {
  int ii, j, k, l, m, plane, mod, ind;
  int adr, code, HV;
  float value;
  double t0, t1;	// 'float' does not word on PGSC computer

//********************************
//*                              *
//*  CNT block structure checks  *
//*                              *
//********************************

  if ((block_type != GET_REPLY_block_type) &&
      (block_type != EXE_REPLY_block_type) &&
      (block_type != HK_DATA_block_type  )) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "CNT block with wrong block type %X(16)", block_type);
    write_log_pipe( b);
    return;
  }
  
  if (node_type != CNT_node_type) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "CNT block with wrong node type %X(16)", node_type);
    write_log_pipe( b);
    return;
  }  

//****************************************
//*                                      *
//*          process TOF HV data         *
//*                                      *
//****************************************
  
  if ((data_type & 0x1F) == TOF_HV_data_type) {
  
    ii = TOF_index[node_number];

    if (ii < 0) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "CNT TOF_HV_data_type block w/wrong node number %X(16)", node_number);
      write_log_pipe( b);
      return;
    }
      
    if (AMS_block_length != (1+3+42/2)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "CNT TOF_HV_data_type block w/wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
    
    if (SIMULATION) {
//    if (time(NULL)%60<40 && ii==1) return; // simulate old data
    }

    TOF_HV_time[ii] = time( NULL);

    for (j=0; j<14; j++) {
      for (k=0; k<3; k++) {
        ind = TOF_crate[ii].channel[j].PMT[k].HV_index;
        if (ind % 2 == 0) code = AMS_block_body[3+ind/2] >> 8;
        else              code = AMS_block_body[3+ind/2] & 0xFF;
        HV = (int) (TOF_crate[ii].channel[j].PMT[k].A * (float)code +
                    TOF_crate[ii].channel[j].PMT[k].B);
        if (HV >= 0) TOF_HV[ii][j][k] = HV;
        else         TOF_HV[ii][j][k] = 0;
      }
    }
  }

//****************************************
//*                                      *
//*         process TOF temp data        *
//*                                      *
//****************************************

  if ((data_type & 0x1F) == TOF_temp_data_type) {
  
    ii = TOF_index[node_number];

    if (ii < 0) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "CNT TOF_temp_data_type block w/wrong node number %X(16)", node_number);
      write_log_pipe( b);
      return;
    }
      
    if (AMS_block_length != (1+3+12)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "CNT TOF_temp_data_type block w/wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
    
    if (SIMULATION) {
//    if (time(NULL)%60<40 && ii==1) return; // simulate old data
    }

    TOF_temp_time[ii] = time( NULL);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// how often are we getting TOF temperature - ?
/*
{
    static time_t t0, t1;
    t0 = t1;
    t1 = (AMS_block_data[2] << 16) | AMS_block_data[3];
    if (t1 > t0) printf( "delta T for TOF temperature = %d sec\n", t1-t0);
}
*/
/*
// write anti-Suter file
{
  static FILE *file[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  char *fn[8] = { "tof01.temp", "tof03.temp", "tof31.temp", "tof33.temp", 
                  "tof41.temp", "tof43.temp", "tof71.temp", "tof73.temp"};
  if (file[ii] == NULL) file[ii] = fopen( fn[ii], "wb");
  if (file[ii] != NULL) {
    fwrite( AMS_block, 2*(1+AMS_block_length), 1, file[ii]);
  }
}
*/
    for (j=0, l=6; j<4; j++, l++) {
      t0 = AMS_block_body[l];
      t1 = AMS_block_body[l+4];
      if (t0 != 0.0 && t1 != 0.0) {
        TOF_temp[ii][j] = 235.0 - 400.0 * t0 / (t1 - t0);
        TOF_temp_status[ii][j] = NORMAL;
        if (TOF_temp[ii][j] <   0.0) TOF_temp_status[ii][j] = WARNING;
        if (TOF_temp[ii][j] >  50.0) TOF_temp_status[ii][j] = WARNING;
        if (TOF_temp[ii][j] < -10.0) TOF_temp_status[ii][j] = ALARM;
        if (TOF_temp[ii][j] >  60.0) TOF_temp_status[ii][j] = ALARM;
      }
      else {
        TOF_temp[ii][j] = 0.0;
        TOF_temp_status[ii][j] = NO_DATA;
      }
//    printf( "%d  %d  %d \n", ii, j, TOF_temp[ii][j]);
    }
///*
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
if (write_N_tuples) {
  int i, j;
  struct tm *tm_ptr;
  static FILE *file = NULL;
  if (file == NULL) file = fopen( "Luca", "w");
  if (file != NULL) {
    tm_ptr = localtime( &block_time);
    fprintf( file, "%3d %6.3f %12d",
    tm_ptr->tm_yday+1,
    (float) tm_ptr->tm_hour +
    ((float) tm_ptr->tm_min + 
    (float) tm_ptr->tm_sec / 60.0) / 60.0,
    run_number);
    for (i=0; i<8; i++) for (j=0; j<4; j++) 
        fprintf( file, "%5.1f", TOF_temp[i][j]);
    fprintf( file, "\n");
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//*/
  }

//****************************************
//*                                      *
//*          process ACC HV data         *
//*                                      *
//****************************************
  

  if ((data_type & 0x1F) == ACC_HV_data_type) {
  
    ii = ACC_index[node_number];

    if (ii < 0) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "CNT ACC_HV_data_type block w/wrong node number %X(16)", node_number);
      write_log_pipe( b);
      return;
    }  
  
    if (AMS_block_length != (1+3+4)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "CNT ACC_HV_data_type block w/wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
    
    if (SIMULATION) {
//    if (time(NULL)%60<40 && ii==1) return; // simulate old data
    }

    ACC_HV_time[ii] = time( NULL);
    
    for (l=3; l<3+4; l++) {
      adr  = AMS_block_body[l] >> 8;
      code = AMS_block_body[l] & 0xFF;
      for (m=0; m<4; m++) {
        if (adr == ACC_adr[ii][m]) {
          ACC_code[ii][m] = adr;
          for (k=0; k<8; k++) {
            value = C[ii][m][k] * (float) (code - ACC_db[ii][m].DAC_1)
                  + (float) ACC_db[ii][m].HV_1[k];
            HV = value;
            if (HV < 0) HV = 0;
            ACC_HV[ii][m][k] = HV;
          }
        }
      }
    }
  }

//****************************************
//*                                      *
//*          process ATC HV data         *
//*                                      *
//****************************************
  

  if ((data_type & 0x1F) == ATC_HV_data_type) {
  
    ii = ATC_index[node_number];

    if (ii < 0) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "CNT ATC_HV_data_type block w/wrong node number %X(16)", node_number);
      write_log_pipe( b);
      return;
    }  
  
    if (AMS_block_length != (1+3+4)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "CNT ATC_HV_data_type block w/wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
    
    if (SIMULATION) {
//    if (time(NULL)%60<40 && ii==1) return; // simulate old data
    }

    ATC_HV_time[ii] = time( NULL);
    
    for (l=3; l<3+8; l++) {
      adr  = AMS_block_body[l] >> 8;
      code = AMS_block_body[l] & 0xFF;
      for (plane=0; plane<2; plane++) {
        for (mod=0; mod<nb_of_ATC_mods[plane]; mod++) {
          if ((node_number == ATC[plane][mod].SMC_node_nb_0) &&
              (adr         == ATC[plane][mod].HV_adr_0)) {
            value = (-370.4 * (2.43 - 0.01921 * (float) code));
            if (value > 0.0) ATC_HV[plane][mod][0] = 0;
            else             ATC_HV[plane][mod][0] = abs( (int) value);
          }
          if ((node_number == ATC[plane][mod].SMC_node_nb_1) &&
              (adr         == ATC[plane][mod].HV_adr_1)) {
            value = (-370.4 * (2.43 - 0.01921 * (float) code));
            if (value > 0.0) ATC_HV[plane][mod][1] = 0;
            else             ATC_HV[plane][mod][1] = abs( (int) value);
          }
        }
      }
    }
/*  
    printf( "%d ==========================\n", ii);  
    for (plane=0; plane<2; plane++) {
      for (mod=0; mod<nb_of_ATC_mods[plane]; mod++) {
        printf( "  %d %d   ", ATC_HV[plane][mod][0], ATC_HV[plane][mod][1]);
      }
      printf( "\n");
    }
*/
  }

}

//------------------------------------------------------------------------------

void display_CNT_right_panel( short row, short col) {

  int i;
  
  set_colors( headline); 
  
  for (i=row; i<23; i++) display_line( i, col, 80, " ");

  _settextposition( ++row, col+1);
  _outtext("Color");
  _settextposition( ++row, col+1);
  _outtext("coding:");
  
  row++;
  
  _settextposition( ++row, col+1);
  set_colors( frame); 
  _outtext("7 x");
  set_colors( headline); 
  _outtext(" - ");
  _settextposition( ++row, col+1);
  _outtext("const.");

  row++;
  
  _settextposition( ++row, col+1);
  set_colors( no_data); 
  _outtext("   ");  
  set_colors( headline); 
  _outtext(" - ");  
  _settextposition( ++row, col+1);
  _outtext("no data");  
  
  row++;
  
  _settextposition( ++row, col+1);
  set_colors( normal); 
  _outtext("0.1");  
  set_colors( headline); 
  _outtext(" - ");    
  _settextposition( ++row, col+1);
  _outtext("normal");    

  row++;
  
  _settextposition( ++row, col+1);
  set_colors( warning); 
  _outtext("0.5");  
  set_colors( headline); 
  _outtext(" - ");
  _settextposition( ++row, col+1);
  _outtext("warning");

  row++;
  
  _settextposition( ++row, col+1);
  set_colors( alarm); 
  _outtext("0.9");  
  set_colors( headline); 
  _outtext(" - ");  
  _settextposition( ++row, col+1);
  _outtext("alarm");  

  row++;
  
  _settextposition( ++row, col+1);
  set_colors( old_data); 
  _outtext("1.5");  
  set_colors( headline); 
  _outtext(" -");  
  _settextposition( ++row, col+1);
  _outtext("old/bad");  
  
}

//------------------------------------------------------------------------------
    
void display_CNT_data_frame( int page) {     

  int i, plane, mod, Cr, Ch;
  short row, col;
  
  set_colors( frame);
  
  if (page == 1 || page == 2) {
    for (Cr=4*(page-1); Cr<4*page; Cr++) {
      _settextposition( row=2, col=1+18*(Cr%4));                                 
      sprintf( b, "TOF%s  A   B   C %s",TOF_crate[Cr].name, VBAR); 
      _outtext( b);
    }
    for (Cr=4*(page-1); Cr<4*page; Cr++) {
      for (Ch=0; Ch<14; Ch++) {
        _settextposition( row=3+Ch, col=1+18*(Cr%4));                                 
        sprintf( b, "%s", &TOF_crate[Cr].channel[Ch].name[2]); 
        _outtext( b);
        _settextposition( row, col+=17);                                 
        _outtext( VBAR);
      }
    }
  }
  
  if (page == 1) {
    for (Cr=0; Cr<2; Cr++) {
      _settextposition( row=17, col=1+36*Cr);
      sprintf( b, "ACC:%s%s", ACC_name[Cr][0], VBAR);
      _outtext(b);
      _settextposition( row=20, col=1+36*Cr);
      sprintf( b, "ACC:%s%s", ACC_name[Cr][1], VBAR);
      _outtext(b);
      for (Ch=0; Ch<4; Ch++) {
        _settextposition( row=18+Ch+Ch/2, col=1+36*Cr);
        _outtext( "   ");
        _settextposition( row, col+=35);
        _outtext( VBAR); 
      }
    }
  } 

  if (page == 2) {
    for (plane=0; plane<2; plane++) {
      for (mod=0; mod<nb_of_ATC_mods[plane]; mod++) {
        _settextposition( row=17+3*plane, col=1+12*mod);
        sprintf( b, "---ATC %s--%s", ATC[plane][mod].name, VBAR);
        _outtext(b);
        Cr = ATC[plane][mod].SMC_node_nb_0;
        _settextposition( row+1, col=1+12*mod);
        sprintf( b, " %s", SMC_name[Cr]);
        _outtext(b);
        _settextposition( row+1, col=1+12*mod+10);
        sprintf( b, " %s", VBAR);
        _outtext(b);
        Cr = ATC[plane][mod].SMC_node_nb_1;
        _settextposition( row+2, col=1+12*mod);
        sprintf( b, " %s", SMC_name[Cr]);
        _outtext(b);
        _settextposition( row+2, col=1+12*mod+10);
        sprintf( b, " %s", VBAR);
        _outtext(b);
      }
    }
  } 
  
  if (page == 3) {
    char *lines[21] = {
  //        1         2         3         4         5         6         7
  // 3456789012345678901234567890123456789012345678901234567890123456789012
  "                                                                        ", // 2  
  "  ------------TOF.01-TOF.31-TOF.41-TOF.71-TOF.03-TOF.33-TOF.43-TOF.73   ", // 3  
  "                                                                        ", // 4  
  "  SFET00......XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX   ", // 5
  "  SFET01......XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX   ", // 6
  "  SFET02......XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX   ", // 7
  "  SFET03......XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX.XXXXXX   ", // 8
  "                                                                        ", // 9  
  "                                                                        ", //10  
  "                                                                        ", //11  
  "                                                                        ", //12  
  "                                                                        ", //13  
  "                                                                        ", //14  
  "                                                                        ", //15  
  "                                                                        ", //16  
  "                                                                        ", //17  
  "                                                                        ", //18  
  "                                                                        ", //19  
  "                                                                        ", //20  
  "                                                                        ", //21  
  "                                                                        "};//22  
    for (i=0; i<21; i++) {
      _settextposition( 2+i, 1);
      _outtext( lines[i]);
    }
  }

}

//------------------------------------------------------------------------------

void display_CNT_data( int page) {

  int plane, mod, Cr, Ch, PM, k;
  time_t now;
  short row, col;
  
  if (page_frozen) return;

  now = time( NULL);
    
  if (page == 1 || page == 2) {
    for (Cr=4*(page-1); Cr<4*page; Cr++) {
      if (TOF_HV_time[Cr]) data_are_absent = FALSE;
      else                 data_are_absent = TRUE;
      if (now - TOF_HV_time[Cr] > TOF_HV_time_staleness) data_are_old = TRUE;
      else                                               data_are_old = FALSE;
      for (Ch=0; Ch<14; Ch++) {
        for (PM=0; PM<3; PM++) {
          _settextposition( row=3+Ch, col=6+18*(Cr%4)+4*PM);                                 
          set_value_colors( NORMAL);
          sprintf( b, "%4d", TOF_HV[Cr][Ch][PM]); 
          _outtext( b);
        }
      }
    }
  }

  if (page == 1) {
    for (Cr=0; Cr<2; Cr++) {
      if (ACC_HV_time[Cr]) data_are_absent = FALSE;
      else                 data_are_absent = TRUE;
      if (now - ACC_HV_time[Cr] > ACC_HV_time_staleness) data_are_old = TRUE;
      else                                               data_are_old = FALSE;
      for (Ch=0; Ch<4; Ch++) {
        _settextposition( row=18+Ch+Ch/2, col=1+36*Cr);                                 
        set_value_colors( NORMAL);
        sprintf( b, "%2.2X", ACC_code[Cr][Ch]);
        _outtext( b);
        for (PM=0; PM<8; PM++) {
          _settextposition( row=18+Ch+Ch/2, col=4+36*Cr+4*PM);                                 
          set_value_colors( NORMAL);
          sprintf( b, "%4d", ACC_HV[Cr][Ch][PM]);
          _outtext( b);
        }
      }
    }
  }

  if (page == 2) {
    for (plane=0; plane<2; plane++) {
      for (mod=0; mod<nb_of_ATC_mods[plane]; mod++) {
        Cr = ATC[plane][mod].SMC_node_nb_0;
        if (ATC_HV_time[Cr]) data_are_absent = FALSE;
        else                 data_are_absent = TRUE;
        if (now - ATC_HV_time[Cr] > ATC_HV_time_staleness) data_are_old = TRUE;
        else                                               data_are_old = FALSE;
        _settextposition( row=17+3*plane+1, col=1+12*mod+6);
        set_value_colors( NORMAL);
        sprintf( b, "%4d", ATC_HV[plane][mod][0]);
        _outtext(b);
        Cr = ATC[plane][mod].SMC_node_nb_1;
        if (ATC_HV_time[Cr]) data_are_absent = FALSE;
        else                 data_are_absent = TRUE;
        if (now - ATC_HV_time[Cr] > ATC_HV_time_staleness) data_are_old = TRUE;
        else                                               data_are_old = FALSE;
        _settextposition( row+1, col=1+12*mod+6);
        set_value_colors( NORMAL);
        sprintf( b, "%4d", ATC_HV[plane][mod][1]);
        _outtext(b);
      }
    }
  } 

  if (page == 3) {
    for (Cr=0; Cr<8; Cr++) {
      if (TOF_temp_time[Cr]) data_are_absent = FALSE;
      else                   data_are_absent = TRUE;
      if (now - TOF_temp_time[Cr] > TOF_temp_time_staleness) data_are_old = TRUE;
      else                                                   data_are_old = FALSE;
      for (mod=0; mod<4; mod++) {
        _settextposition( row=5+mod, col=15+7*Cr);                                 
        set_value_colors( TOF_temp_status[Cr][mod]);
        sprintf( b, " %4.1f ", TOF_temp[Cr][mod]);
        _outtext( b);
      }
    }
  }

}

//------------------------------------------------------------------------------

void display_CNT_page( int page) {
  short row, col;
  switch (page) {
  case 1:
    display_headline( "TOF & ACC COUNTERS HV PAGE 1/2");
    break;
  case 2:
    display_headline( "TOF & ATC COUNTERS HV PAGE 2/2");
    break;
  case 3:
    display_headline( "SFET BOARD TEMP PAGE");
    break;
  default:
    return;
  }
  if (new_page) {
    display_CNT_data_frame( page);
    display_CNT_right_panel( row=2, col=73);
  }
  display_CNT_data( page);
}

//------------------------------------------------------------------------------


