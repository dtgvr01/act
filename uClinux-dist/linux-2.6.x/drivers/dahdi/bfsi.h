/*
  bfsi.h
  David Rowe
  Dec 1 2006

  Functions for Linux device drivers on the Blackfin that
  support interfacing the Blackfin to Silicon Labs chips.
*/

#ifndef __BFSI__

void bfsi_spi_write_8_bits(u16 chip_select, u8 bits);
u8 bfsi_spi_read_8_bits(u16 chip_select);
void bfsi_spi_init(int baud, u16 chip_select_mask);

int bfsi_sport_init(
  void (*isr_callback)(u32 *read_samples, u32 *write_samples),
  int samples,
  int wideband,
  int debug
  );

void bfsi_reset(int reset_bit);
void bfsi_sport_close(void);

#endif

