#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *MONTHS[] = {
    "Invalid", "January", "February",  "March",   "April",    "May",     "June",
    "July",    "August",  "September", "October", "November", "December"};

// Leap year fixer.
int is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Day of Year (DoY) to Month and Day. Kudos @linguanerd @ SO
void yddd_to_date(int year, int doy, int *month, int *day) {
  int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (is_leap_year(year))
    days_in_month[2] = 29;

  int m = 1;
  while (m <= 12 && doy > days_in_month[m]) {
    doy -= days_in_month[m];
    m++;
  }
  *month = m;
  *day = doy;
}

// Remove spaces from hubcode.
void remove_spaces(char *str) {
  char *src = str, *dst = str;
  while (*src) {
    if (*src != ' ')
      *dst++ = *src;
    src++;
  }
  *dst = '\0';
}

// CMC Magnetics format: [A-Z][A-L][0-9][0-9].
int parse_cmc(const char *hubcode, int *year, int *month, int *day) {
  if (strlen(hubcode) < 4)
    return 0;
  if (!isupper((unsigned char)hubcode[0]) ||
      !isupper((unsigned char)hubcode[1]))
    return 0;
  if (!isdigit((unsigned char)hubcode[2]) ||
      !isdigit((unsigned char)hubcode[3]))
    return 0;

  *year = 1990 + (hubcode[0] - 'A');
  *month = hubcode[1] - 'A' + 1;
  char day_str[3] = {hubcode[2], hubcode[3], '\0'};
  *day = atoi(day_str);

  return (*month >= 1 && *month <= 12 && *day >= 1 && *day <= 31);
}

// Daxon Technology format: [A-Z][A-Z][0-9][0-9]...[0-9][0-9].
int parse_daxon(const char *hubcode, int *year, int *month, int *day) {
  if (strlen(hubcode) < 9)
    return 0;
  if (!isupper((unsigned char)hubcode[0]) ||
      !isupper((unsigned char)hubcode[1]))
    return 0;
  for (int i = 2; i < 9; i++) {
    if (!isdigit((unsigned char)hubcode[i]))
      return 0;
  }

  *year = 1999 + (hubcode[1] - 'A');
  char month_str[3] = {hubcode[2], hubcode[3], '\0'};
  char day_str[3] = {hubcode[7], hubcode[8], '\0'};
  *month = atoi(month_str);
  *day = atoi(day_str);

  return (*month >= 1 && *month <= 12 && *day >= 1 && *day <= 31);
}

// Standard format (YDDD).
int parse_standard(const char *hubcode, int decade, int *year, int *month,
                   int *day) {
  if (strlen(hubcode) < 4)
    return 0;
  for (int i = 0; i < 4; i++) {
    if (!isdigit((unsigned char)hubcode[i]))
      return 0;
  }

  int y_digit = hubcode[0] - '0';
  *year = decade + y_digit;

  char doy_str[4] = {hubcode[1], hubcode[2], hubcode[3], '\0'};
  int doy = atoi(doy_str);

  if (doy < 1 || doy > 366)
    return 0;

  yddd_to_date(*year, doy, month, day);
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <manufacturer> <hubcode> [decade]\n", argv[0]);
    printf("Manufacturers:\n");
    printf("  CMC Magnetics, Daxon Technology, Ritek Corporation, MJC Pte Ltd "
           "(Singapore),\n");
    printf("  Optodisc Technology, TDK Corporation (Luxembourg), Mitsubishi "
           "Chemical Corporation [CD-R],\n");
    printf("  Mitsubishi Chemical Corporation [DVD-R], Taiyo Yuden, Mitsui "
           "Toatsu Chemicals + MAM-E,\n");
    printf("  Moser Baer India, Prodisc, Eastman Kodak Company\n");
    printf("\nNote: Decade defaults to 2000 if not specified (used for "
           "standard YDDD format).\n");
    return 1;
  }

  char *manufacturer = argv[1];
  char hubcode[256];
  strncpy(hubcode, argv[2], sizeof(hubcode) - 1);
  hubcode[sizeof(hubcode) - 1] = '\0';

  int decade = 2000;
  if (argc >= 4) {
    decade = atoi(argv[3]);
  }

  remove_spaces(hubcode);

  int year = 0, month = 0, day = 0;
  int success = 0;

  if (strcmp(manufacturer, "CMC Magnetics") == 0) {
    success = parse_cmc(hubcode, &year, &month, &day);
  } else if (strcmp(manufacturer, "Daxon Technology") == 0) {
    success = parse_daxon(hubcode, &year, &month, &day);
  } else {
    // All other listed manufacturers use the standard YDDD format.
    success = parse_standard(hubcode, decade, &year, &month, &day);
  }

  if (success) {
    printf("%s %d, %d\n", MONTHS[month], day, year);
  } else {
    printf("No date string found in the provided hubcode.\n");
  }

  return 0;
}
