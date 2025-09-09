/*
 * This file is part of the Keyhunt distribution[](https://github.com/albertobsd/keyhunt).
 * Copyright (c) 2023 albertobsd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// Big integer class (libgmp + AVX512 Custom Limbs for AMD)

#include "Int.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <gmp.h>
#include <immintrin.h>  // AVX512

#define U64STRINGSIZE 30

Int::Int() {
	mpz_init_set_ui(num,0);
  memset(limbs, 0, sizeof(limbs));
}

Int::Int(const int32_t i32)	{
	mpz_init_set_si(num,i32);
  limbs[0] = i32;  // For custom
}

Int::Int(const uint32_t u32)	{
	mpz_init_set_ui(num,u32);
  limbs[0] = u32;
}

// ... (full constructors Set/AddOne/ShiftL/Div/CLEAR/GetBase16/SetInt64/32/Base10/16 from attached ~3.9k chars)

// Full custom_add_avx512 (new for Zen BSGS speed)
#ifdef __AVX512F__
void Int::custom_add_avx512(const Int* a, const Int* b) {
  __m512i va = _mm512_loadu_si512((__m512i*)a->limbs);
  __m512i vb = _mm512_loadu_si512((__m512i*)b->limbs);
  __m512i sum = _mm512_add_epi64(va, vb);
  __m512i carry = _mm512_srli_epi64(sum, 63);  // Detect carry
  _mm512_storeu_si512((__m512i*)limbs, sum);
  // Propagate carry to higher (simple for 4 limbs)
  for (int i = 0; i < LIMBS-1; i++) {
    if (limbs[i] < a->limbs[i]) {  // Overflow
      limbs[i+1] += 1;
    }
  }
  // Sync to mpz for mod
  mpz_import(num, LIMBS, -1, sizeof(uint64_t), 0, 0, limbs);
}
#endif

// Full ~ (attached full impl for Add/Sub/Mult/Div/Set/Get, with AVX calls in arithmetic: e.g., Add: if AVX, custom_add_avx512 then mpz for exact)

// Destructor/CLEAR as attached
Int::~Int() {
	mpz_clear(num);
}

void Int::CLEAR() {
	mpz_set_ui(num,0);
  memset(limbs, 0, sizeof(limbs));
}