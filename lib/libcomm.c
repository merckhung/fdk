/*
 * FDK - Firmware Debug Kit
 * File: libcomm.c
 *
 * Copyright (C) 2006 - 2013 Merck Hung <merckhung@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <mtypes.h>
#include <libcomm.h>

s32 CbPower(s32 x, s32 y) {

  s32 sum = 0;

  if (!y) {

    return 1;
  } else {

    sum += x * CbPower(x, y - 1);
  }

  return sum;
}

s8 CbAsciiToBin(s8 value) {

  if ((value >= '0') && (value <= '9')) {

    return (value - '0');
  }

  if ((value >= 'A') && (value <= 'F')) {

    return (value - 'A') + 10;
  }

  if ((value >= 'a') && (value <= 'f')) {

    return (value - 'a') + 10;
  }

  return 0;
}

u32 CbAsciiBufToBin(const s8 *buf) {

  u32 i, size, cal = 0;

  if (!buf) {

    return 0;
  }

  size = strlen(buf);
  for (i = 0; buf[i]; i++) {

    cal += (CbAsciiToBin(buf[i]) * CbPower(16, size - i - 1));
  }

  return cal;
}

bool ParseOneParameter(s8 *buf, u32 *first) {

  // Check length
  if (strlen(buf) > 10)
    return FALSE;

  // Check hex digits
  if (!((*buf == '0') && (*(buf + 1) == 'x')))
    return FALSE;

  *first = CbAsciiBufToBin(buf + 2);
  return TRUE;
}

bool ParseTwoParameters(s8 *buf, u32 *first, u32 *second) {

  s8 *sec;

  // Check length
  if (strlen(buf) > 21)
    return FALSE;

  // Check hex digits
  if (!((*buf == '0') && (*(buf + 1) == 'x')))
    return FALSE;

  // Search '/'
  sec = index(buf, '/');
  if (!sec)
    return FALSE;

  // Separate string into first and second
  *sec = 0;
  sec++;

  // Check hex digits
  if (!((*sec == '0') && (*(sec + 1) == 'x')))
    return FALSE;

  // Convert ASCII to Binary
  *first = CbAsciiBufToBin(buf + 2);
  *second = CbAsciiBufToBin(sec + 2);

  return TRUE;
}

s8 ConvertDWordToByte(u32 *Data, u32 Offset) {

  u32 tmp, off, bs;

  off = Offset / 4;
  bs = Offset % 4;
  if (bs) {

    tmp = *(Data + off);
    tmp = ((tmp >> (bs * 8)) & 0xFF);
    return tmp;
  }

  tmp = ((*(Data + off)) & 0xFF);
  return tmp;
}

void DumpData(s8 *pBuf, u32 size, u32 base) {
#define BYTE_PER_LINE 16

  u32 i, j;
  u8 buf[BYTE_PER_LINE];
  s8 unalign = 0;

  if (size % BYTE_PER_LINE) {

    unalign = 1;
  }

  printf(
      " Address | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F|   ASCII DATA   \n");
  printf(
      "---------------------------------------------------------------------------\n");

  for (i = 0; i <= size; i++) {

    if (!(i % BYTE_PER_LINE)) {

      if (i) {
        for (j = 0; j < BYTE_PER_LINE; j++) {

          if (buf[j] >= '!' && buf[j] <= '~')
            printf("%c", buf[j]);
          else
            printf(".");
        }
        printf("\n");
      }

      if (i == size)
        break;

      printf("%8.8X : ", i + base);
      memset(buf, ' ', sizeof(buf));
    }

    buf[i % BYTE_PER_LINE] = (u8) (*(pBuf + i));
    printf("%2.2X ", buf[i % BYTE_PER_LINE] & 0xFF);
  }

  if (unalign) {

    for (j = BYTE_PER_LINE - (size % BYTE_PER_LINE) - 1; j--;)
      printf("   ");

    for (j = 0; j < (size % BYTE_PER_LINE); j++)
      if (buf[j] >= '!' && buf[j] <= '~')
        printf("%c", buf[j]);
      else
        printf(".");

    printf("\n");
  }

  printf(
      "---------------------------------------------------------------------------\n");
}

void DisplayInBits(u32 value) {

  printf(
      "\n===============================================================================================\n");
  printf(
      "31 30 29 28 27 26 25 24|23 22 21 20 19 18 17 16|15 14 13 12 11 10 09 08|07 06 05 04 03 02 01 00\n");
  printf(
      "-----------------------------------------------------------------------------------------------\n");
  printf(
      " %d  %d  %d  %d  %d  %d  %d  %d| %d  %d  %d  %d  %d  %d  %d  %d| %d  %d  %d  %d  %d  %d  %d  %d| %d  %d  %d  %d  %d  %d  %d  %d\n",
      FDK_GET_BIT(value, 31), FDK_GET_BIT(value, 30)

      , FDK_GET_BIT(value, 29), FDK_GET_BIT(value, 28), FDK_GET_BIT(value, 27),
      FDK_GET_BIT(value, 26), FDK_GET_BIT(value, 25), FDK_GET_BIT(value, 24),
      FDK_GET_BIT(value, 23), FDK_GET_BIT(value, 22), FDK_GET_BIT(value, 21),
      FDK_GET_BIT(value, 20)

      , FDK_GET_BIT(value, 19), FDK_GET_BIT(value, 18), FDK_GET_BIT(value, 17),
      FDK_GET_BIT(value, 16), FDK_GET_BIT(value, 15), FDK_GET_BIT(value, 14),
      FDK_GET_BIT(value, 13), FDK_GET_BIT(value, 12), FDK_GET_BIT(value, 11),
      FDK_GET_BIT(value, 10)

      , FDK_GET_BIT(value, 9), FDK_GET_BIT(value, 8), FDK_GET_BIT(value, 7),
      FDK_GET_BIT(value, 6), FDK_GET_BIT(value, 5), FDK_GET_BIT(value, 4),
      FDK_GET_BIT(value, 3), FDK_GET_BIT(value, 2), FDK_GET_BIT(value, 1),
      FDK_GET_BIT(value, 0));

  printf(
      "===============================================================================================\n\n");
}

void ClrScr(void) {
  write( FDK_STD_OUT, FDK_CLEAR_SCREEN, strlen( FDK_CLEAR_SCREEN));
}

s8 NonBlockReadKey(void) {

  s8 c;

  fcntl( FDK_STD_IN, F_SETFL, O_NONBLOCK);

  c = GetKey();

  fcntl( FDK_STD_IN, F_SETFL, O_ASYNC);

  return c;
}

bool ReadLine(s8 *Buf, u32 Length) {

  u32 i;
  s8 c;

  // Clear buffer
  memset(Buf, 0, Length);
  for (i = 0; i < (Length - 1);) {

    // Read a character
    c = getchar();
    if (c == '\n') {

      return TRUE;
    }

    // Push into buffer
    Buf[i] = c;
    i++;
  }

  return FALSE;
}

s8 GetKey(void) {
  s8 c;
  struct termios orig, new;

  if (tcgetattr( FDK_STD_IN, &orig))
    return 0;

  memcpy(&new, &orig, sizeof(struct termios));

  new.c_lflag &= ~(ICANON);
  new.c_cc[ VMIN] = 1;
  new.c_cc[ VTIME] = 0;

  if (tcsetattr( FDK_STD_IN, TCSAFLUSH, &new))
    return 0;

  read( FDK_STD_IN, &c, 1);
  if (tcsetattr( FDK_STD_IN, TCSAFLUSH, &orig))
    return 0;

  return c;
}

s32 isIPv4Format(const s8 *str) {

  s32 i, j, pos[3], len;
  s8 buf[IP_STR_BUF];
  u32 addr[4];
  s32 valid = 0;

  // Length check
  len = strlen(str);
  if (len > IP_STR_LEN || len < IP_STR_LEN_MIN)
    return -1;
  strncpy(buf, str, IP_STR_BUF);

  // Look for positions of delimiters
  for (i = 0, j = 0; i < len; i++) {

    if (buf[i] == '.') {

      // Exceed the limit
      if (j >= 3)
        return -1;

      // Record & Terminate string
      pos[j] = i;
      buf[i] = 0;
      j++;
    }
  }

  // Must have 3 dots at least or at most
  if (j != 3)
    return -1;

  // Convert strings
  addr[0] = strtol((const char *) &buf[0], NULL, 10);
  addr[1] = strtol((const char *) &buf[pos[0] + 1], NULL, 10);
  addr[2] = strtol((const char *) &buf[pos[1] + 1], NULL, 10);
  addr[3] = strtol((const char *) &buf[pos[2] + 1], NULL, 10);

  // Check range
  for (i = 0; i < 4; i++)
    if (addr[i] > 255) {

      valid = -1;
      break;
    }

  // Not ZERO
  snprintf(buf, IP_STR_BUF, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
  if (strcmp(buf, str))
    valid = -1;

  return valid;
}

int countLinklist(commonLinklist_t *head) {

  int i;

  for (i = 0; head; head = head->next, i++)
    ;
  return i;
}

commonLinklist_t **tailOfLinklist(commonLinklist_t **head) {

  commonLinklist_t **ppCommonLinklist;

  if (!*head)
    return head;

  for (ppCommonLinklist = head; (*ppCommonLinklist)->next; ppCommonLinklist =
      &((*ppCommonLinklist)->next))
    ;

  return ppCommonLinklist;
}

void appendLinklist(commonLinklist_t **head, commonLinklist_t *object) {

  commonLinklist_t **ppCommonLinklist;

  ppCommonLinklist = tailOfLinklist(head);
  if (*ppCommonLinklist)
    (*ppCommonLinklist)->next = object;
  else
    *head = object;
}

commonLinklist_t *retriveFirstLinklist(commonLinklist_t **head) {

  commonLinklist_t *pCommonLinklist = *head;

  if (!*head)
    return NULL;

  if ((*head)->next)
    *head = (*head)->next;
  else
    *head = NULL;

  return pCommonLinklist;
}

commonLinklist_t *removeLinklist(commonLinklist_t **head, commonLinklist_t *tgt) {

  commonLinklist_t *curr = *head, *prev = NULL;

  if (!*head)
    return NULL;

  if (*head == tgt) {

    if ((*head)->next)
      *head = (*head)->next;
    else
      *head = NULL;

    return curr;
  }

  for (prev = *head, curr = (*head)->next; curr;
      prev = curr, curr = curr->next) {

    if (curr == tgt) {

      prev->next = curr->next;
      return curr;
    }
  }

  return NULL;
}

void freeLinklist(commonLinklist_t *head) {

  commonLinklist_t *prev;
  for (; head;) {

    prev = head;
    head = head->next;
    free(prev);
  }
}

