/* Copyright (c) 2017 Pieter Wuille
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _SEGWIT_ADDR_H_
#define _SEGWIT_ADDR_H_ 1

#include <stdint.h>

/** Encode a SegWit address
 *
 *  Out: output:   Pointer to a buffer of size 73 + strlen(hrp) that will be
 *                 updated to contain the null-terminated address.
 *  In:  hrp:      Pointer to the null-terminated human readable part to use
 *                 (chain/network specific).
 *       ver:      Version of the witness program (between 0 and 16 inclusive).
 *       prog:     Data bytes for the witness program (between 2 and 40 bytes).
 *       prog_len: Number of data bytes in prog.
 *  Returns 1 if successful.
 */
int segwit_addr_encode(char *output, const char *hrp, int ver,
                       const uint8_t *prog, size_t prog_len);

/** Encode a Bech32 string
 *
 *  Out: output:  Pointer to a buffer of size strlen(hrp) + data_len + 8 that
 *                will be updated to contain the null-terminated Bech32 string.
 *  In: hrp :     Pointer to the null-terminated human readable part.
 *      data :    Pointer to an array of 5-bit values.
 *      data_len: Length of the data array.
 *  Returns 1 if successful.
 */
int bech32_encode(char *output, const char *hrp, const uint8_t *data,
                  size_t data_len);


#endif
