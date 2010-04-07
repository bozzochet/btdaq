main() {
  int i, j, k;
  unsigned char shift_register, feedback_pattern, feedback_bit;
  unsigned char random_codes[512];
  unsigned char random_sequence[4096];

  unsigned char initial_load  = 0xFF;
  unsigned char feedback_mask = 0x95;

  shift_register = initial_load;

  for (k=0; k<512; k++) {
    random_codes[k] = shift_register;
    if (shift_register == initial_load) printf( "period at k=%d\n", k);
    for (j=0; j<8; j++) {
      feedback_pattern = shift_register & feedback_mask;
//    printf( "feedback_pattern = %2.2X  ", (unsigned char) feedback_pattern);
      feedback_bit = 0;
      for (i=0; i<8; i++) {
        feedback_bit      ^= feedback_pattern & 1;
        feedback_pattern >>= 1;
      }
//    printf( "feedback_bit = %1.1X\n", (unsigned char) feedback_bit);
      shift_register <<= 1;
      shift_register  |= feedback_bit;
    }
//  printf( "k = %3.3d, shift_register = %2.2X\n", k, (unsigned char) shift_register);
  }
  
  for (k=0; k<255; k++) {
    printf( " 0x%2.2X,", random_codes[k]);
    if ((k+1) % 8 == 0) printf( "\n");
  }
  printf( "\n");
  
  for (k=0; k<2*255; k+=2) {
    if (k % 16 == 0) printf( "\n");
    printf( " 0x%2.2X%2.2X,", random_codes[k], random_codes[k+1]);
  }
  printf( "\n");
  
}
